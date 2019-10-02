//=============================================================================
//	FILE:						parse_gap.cpp
//	SYSTEM:				 	game asset packer
//	DESCRIPTION:
//-----------------------------------------------------------------------------
//	COPYRIGHT:			(C)Copyright 2019 Adrian Purser. All Rights Reserved.
//	LICENCE:				MIT
//	MAINTAINER:			AJP - Adrian Purser <ade@arcadestuff.com>
//	CREATED:				25-SEP-2019 Adrian Purser <ade@arcadestuff.com>
//=============================================================================

#include <iostream>
#include <regex>
#include <string>
#include <algorithm>
#include "parse_gap.h"
#include "utility/hash.h"

#define GAPCMD_LOADIMAGE				"loadimage"
#define GAPCMD_IMAGE						"image"
#define GAPCMD_IMAGEGROUP				"imagegroup"

namespace gap
{

ParserGAP::ParserGAP(gap::FileSystem & filesystem)
 : m_filesystem(filesystem)
 {
 }

std::unique_ptr<gap::assets::Assets>		
ParserGAP::parse(std::string_view source)
{
	m_p_assets = std::make_unique<gap::assets::Assets>();

	std::string_view::size_type linestart = 0;
	int linenumber = 1;
	bool b_error = false;

	while((linestart != std::string_view::npos) && !b_error)
	{
		auto linebreak = source.find('\n',linestart);
		auto line = (linebreak == std::string_view::npos) ? source.substr(linestart) : source.substr(linestart,linebreak-linestart);

		if(parse_line(line,linenumber))
		{
			b_error = true;
			continue;
		}

		if(linebreak == std::string_view::npos)
			break;

		linestart = linebreak+1;
		++linenumber;
	}

	if(b_error)
		return nullptr;

	return std::move(m_p_assets);
}


int
ParserGAP::parse_line(std::string_view line,int line_number)
{
	//---------------------------------------------------------------------------
	//	Tokenise the line string
	//---------------------------------------------------------------------------
	if(line.empty())
		return 0;

	gap::CommandLine	cmd;
	int 							iarg = 0;
	std::string 			token;
	bool							b_quotes = false;

	auto parse_arg = 	[&](int line_number,const std::string & token)->std::pair<std::string,std::string>
										{
											auto pos = token.find('=');
											if(pos == std::string::npos)
											{
												on_error(line_number,"Missing value for '" + token + "'!");
												return {};
											}
											auto key = token.substr(0,pos);
											auto val = token.substr(pos+1);
											if(key.empty() || val.empty())
											{
												on_error(line_number,"Syntax Error!");
												return {};
											}
											return {key,val};
										};

	for(auto ch : line)
	{
		bool b_finished = false;

		switch(ch)
		{
			case ' ' :
			case '\t' :
				if(!token.empty())
					token.push_back(ch);
				break;
				
			case '#' :
				b_finished = true;
				break;

			case ',' :
				if(!b_quotes)
				{
					if(iarg == 0)
					{
						if(token.empty())
							return on_error(line_number,"Missing command!");
						cmd.command = token;
					}
					else if(!token.empty())
					{
						auto arg = parse_arg(line_number,token);
						if(arg.first.empty())
							return -1;
						cmd.args[arg.first] = arg.second;
					}
					++iarg;
					token.clear();
				}
				else
					token.push_back(ch);
				break;

			case '"' :
				b_quotes = !b_quotes;
				break;

			case 8 :
			case 10 :
			case 13 :
				break;
			
			default :
				token.push_back(ch);
				break;
		}
	
		if(b_finished)
			break;
	}

		
	if(!token.empty())
	{
		if(iarg == 0)
		{
			if(token.empty())
				return on_error(line_number,"Missing command!");
			cmd.command = token;
		}
		else if(!token.empty())
		{
			auto arg = parse_arg(line_number,token);
			if(arg.first.empty())
				return -1;
			cmd.args[arg.first] = arg.second;
		}
	}

	if(cmd.command.empty())
		return 0;

	//---------------------------------------------------------------------------
	// Process the command
	//---------------------------------------------------------------------------
	
	int result = 0;

	auto hash = ade::hash::hash_ascii_string_as_lower(cmd.command.c_str(),cmd.command.size());

	switch(hash)
	{
		case ade::hash::hash_ascii_string_as_lower(GAPCMD_LOADIMAGE) :	result = command_loadimage(line_number,cmd); 		break;
		case ade::hash::hash_ascii_string_as_lower(GAPCMD_IMAGEGROUP) :	result = command_imagegroup(line_number,cmd); 	break;
		case ade::hash::hash_ascii_string_as_lower(GAPCMD_IMAGE) :			result = command_image(line_number,cmd); 				break;

		default : 
			std::cerr << "GAP: Unknown command '" << cmd.command << "'\n";
			result = -1;
			break;
	}

	return result;
}



int 
ParserGAP::command_loadimage(int line_number, const CommandLine & command)
{
	
	std::string src;
	std::string format;

	for(const auto & [key,value] : command.args)
	{
		auto hash = ade::hash::hash_ascii_string_as_lower(key.c_str(),key.size());
		switch(hash)
		{
			case ade::hash::hash_ascii_string_as_lower("src") 		:	src 		= value; break;
			case ade::hash::hash_ascii_string_as_lower("format") 	:	format 	= value; break;
			default : 
				// TODO: Warning - unknown arg
				break;
		}
	}

	if(src.empty())
		return on_error(line_number,"Missing image path!");

	auto p_image = gap::image::load(src,m_filesystem);
	if(p_image == nullptr)
		return on_error(line_number,std::string("Failed to load image! - ") + src);

	if(!format.empty())
	{
		auto pf = gap::image::parse_pixelformat_name(format);
		if(pf == 0)
			return on_error(line_number,std::string("Unknown Pixel Format! - ") + format);
		p_image->set_target_pixelformat(pf);
	}

	m_current_source_image = m_p_assets->add_source_image(std::move(p_image));

	std::cout << "  Image added into slot " << m_current_source_image << std::endl;
	
	return 0;
}

int 
ParserGAP::command_imagegroup(int line_number, const CommandLine & command)
{

	// TODO :
	
	return 0;
}

int 
ParserGAP::command_image(int line_number, const CommandLine & command)
{
	gap::image::Image	image;

	for(const auto & [key,value] : command.args)
	{
		auto hash = ade::hash::hash_ascii_string_as_lower(key.c_str(),key.size());
		switch(hash)
		{
			case ade::hash::hash_ascii_string_as_lower("x") 			:	image.x 			= std::strtol(value.c_str(),nullptr,10); 	break;
			case ade::hash::hash_ascii_string_as_lower("y") 			:	image.y 			= std::strtol(value.c_str(),nullptr,10); 	break;
			case ade::hash::hash_ascii_string_as_lower("w") 			:	
			case ade::hash::hash_ascii_string_as_lower("width") 	:	image.width		= std::strtol(value.c_str(),nullptr,10); 	break;
			case ade::hash::hash_ascii_string_as_lower("h") 			:	
			case ade::hash::hash_ascii_string_as_lower("height")	:	image.height	= std::strtol(value.c_str(),nullptr,10); 	break;
			case ade::hash::hash_ascii_string_as_lower("name") 		:	image.name 		= value; break;
			default : 
				// TODO: Warning - unknown arg
				break;
		}
	}

	image.source_image	= m_current_source_image;
	m_p_assets->add_image(m_current_image_group,image);

	return 0;
}


} // namespace gap

