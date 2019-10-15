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
#define GAPCMD_IMAGEARRAY				"imagearray"
#define GAPCMD_EXPORT						"export"

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
		case ade::hash::hash_ascii_string_as_lower(GAPCMD_IMAGEARRAY) :	result = command_imagearray(line_number,cmd); 	break;
		case ade::hash::hash_ascii_string_as_lower(GAPCMD_EXPORT) :			result = command_export(line_number,cmd); 			break;

		default : 
			std::cerr << "GAP: Unknown command '" << cmd.command << "'\n";
			result = -1;
			break;
	}

	return result;
}

void
ParserGAP::enumerate_exports(std::function<bool(const gap::exporter::ExportInfo &)> callback)
{
	for(const auto & exportinfo : m_export_info)
		if(!callback(exportinfo))
			break;
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

	int group = -1;
	int base = 0;

	for(const auto & [key,value] : command.args)
	{
		auto hash = ade::hash::hash_ascii_string_as_lower(key.c_str(),key.size());
		
		switch(hash)
		{
			case ade::hash::hash_ascii_string_as_lower("group") 	:	group	= std::strtol(value.c_str(),nullptr,10); 	break;
			case ade::hash::hash_ascii_string_as_lower("base") 		:	base	= std::strtol(value.c_str(),nullptr,10); 	break;
			default : 
				// TODO: Warning - unknown arg
				break;
		}
	}
	
	if(group >= 0)
	{
		m_current_image_group = group;
		if(base > 0)
			m_p_assets->set_group_base(group,base);
	}
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
			case ade::hash::hash_ascii_string_as_lower("x") 			:	image.x 				= std::strtol(value.c_str(),nullptr,10); 	break;
			case ade::hash::hash_ascii_string_as_lower("y") 			:	image.y 				= std::strtol(value.c_str(),nullptr,10); 	break;
			case ade::hash::hash_ascii_string_as_lower("w") 			:	
			case ade::hash::hash_ascii_string_as_lower("width") 	:	image.width			= std::strtol(value.c_str(),nullptr,10); 	break;
			case ade::hash::hash_ascii_string_as_lower("h") 			:	
			case ade::hash::hash_ascii_string_as_lower("height")	:	image.height		= std::strtol(value.c_str(),nullptr,10); 	break;
			case ade::hash::hash_ascii_string_as_lower("xo") 			:	
			case ade::hash::hash_ascii_string_as_lower("xorigin")	:	image.x_origin	= std::strtol(value.c_str(),nullptr,10); 	break;
			case ade::hash::hash_ascii_string_as_lower("yo") 			:	
			case ade::hash::hash_ascii_string_as_lower("yorigin")	:	image.y_origin	= std::strtol(value.c_str(),nullptr,10); 	break;
			case ade::hash::hash_ascii_string_as_lower("pf") 			:	
			case ade::hash::hash_ascii_string_as_lower("format")	:	image.pixel_format = gap::image::parse_pixelformat_name(value); break;
			case ade::hash::hash_ascii_string_as_lower("name") 		:	image.name 		= value; break;
			default : 
				// TODO: Warning - unknown arg
				break;
		}
	}

	image.source_image	= m_current_source_image;

	if(image.pixel_format == 0)
		image.pixel_format = m_p_assets->get_target_pixelformat(m_current_source_image);
		
	m_p_assets->add_image(m_current_image_group,image);

	return 0;
}

int 
ParserGAP::command_imagearray(int line_number, const CommandLine & command)
{
	int 		x 			= 0;
	int 		y 			= 0;
	int 		width 	= 0;
	int 		height 	= 0;
	int 		xcount 	= 0;
	int 		ycount 	= 0;
	int 		xorigin	= 0;
	int 		yorigin	= 0;
	uint8_t format 	= 0;

	for(const auto & [key,value] : command.args)
	{
		auto hash = ade::hash::hash_ascii_string_as_lower(key.c_str(),key.size());
		switch(hash)
		{
			case ade::hash::hash_ascii_string_as_lower("x") 			:	x 				= std::strtol(value.c_str(),nullptr,10); 	break;
			case ade::hash::hash_ascii_string_as_lower("y") 			:	y 				= std::strtol(value.c_str(),nullptr,10); 	break;
			case ade::hash::hash_ascii_string_as_lower("w") 			:	
			case ade::hash::hash_ascii_string_as_lower("width") 	:	width			= std::strtol(value.c_str(),nullptr,10); 	break;
			case ade::hash::hash_ascii_string_as_lower("h") 			:	
			case ade::hash::hash_ascii_string_as_lower("height")	:	height		= std::strtol(value.c_str(),nullptr,10); 	break;
			case ade::hash::hash_ascii_string_as_lower("xc") 			:	
			case ade::hash::hash_ascii_string_as_lower("xcount")	:	xcount		= std::strtol(value.c_str(),nullptr,10); 	break;
			case ade::hash::hash_ascii_string_as_lower("yc") 			:	
			case ade::hash::hash_ascii_string_as_lower("ycount")	:	ycount		= std::strtol(value.c_str(),nullptr,10); 	break;
			case ade::hash::hash_ascii_string_as_lower("xo") 			:	
			case ade::hash::hash_ascii_string_as_lower("xorigin")	:	xorigin		= std::strtol(value.c_str(),nullptr,10); 	break;
			case ade::hash::hash_ascii_string_as_lower("yo") 			:	
			case ade::hash::hash_ascii_string_as_lower("yorigin")	:	yorigin		= std::strtol(value.c_str(),nullptr,10); 	break;
			case ade::hash::hash_ascii_string_as_lower("pf") 			:	
			case ade::hash::hash_ascii_string_as_lower("format")	:	format 		= gap::image::parse_pixelformat_name(value); break;
			default : 
				// TODO: Warning - unknown arg
				break;
		}
	}

	if(xcount <= 0) 	return on_error(line_number,std::string("Invalid/Missing 'xcount' parameter!"));
	if(ycount <= 0) 	return on_error(line_number,std::string("Invalid/Missing 'ycount' parameter!"));
	if(width <= 0) 		return on_error(line_number,std::string("Invalid/Missing 'width' parameter!"));
	if(height <= 0) 	return on_error(line_number,std::string("Invalid/Missing 'height' parameter!"));

	for(int yi = 0;yi < ycount;++yi)
	{
		for(int xi = 0;xi < xcount;++xi)
		{
			gap::image::Image	image;
			image.x 						= xi * width;
			image.y 						= yi * height;
			image.x_origin			= xorigin;
			image.y_origin			= yorigin;
			image.width					= width;
			image.height 				= height;
			image.pixel_format	= format;
			image.source_image	= m_current_source_image;
			m_p_assets->add_image(m_current_image_group,image);
		}
	}

	return 0;
}


int 
ParserGAP::command_export(int line_number, const CommandLine & command)
{
	gap::exporter::ExportInfo exportinfo;

	for(const auto & [key,value] : command.args)
	{
		auto hash = ade::hash::hash_ascii_string_as_lower(key.c_str(),key.size());
		switch(hash)
		{
			case ade::hash::hash_ascii_string_as_lower("filename") :	
			case ade::hash::hash_ascii_string_as_lower("path") :		
				exportinfo.filename = value;	
				break;

			case ade::hash::hash_ascii_string_as_lower("type") 			:		
				{
					auto valhash = ade::hash::hash_ascii_string_as_lower(value.c_str(),value.size());
					switch(valhash)
					{
						case ade::hash::hash_ascii_string_as_lower("gbin") :	exportinfo.type = gap::exporter::TYPE_GBIN; break;
						default :																							return on_error(line_number,std::string("Unknown export type! - ") + value);
					}
				}
				break;

			case ade::hash::hash_ascii_string_as_lower("format") 			:		
				{
					auto valhash = ade::hash::hash_ascii_string_as_lower(value.c_str(),value.size());
					switch(valhash)
					{
						case ade::hash::hash_ascii_string_as_lower("binary") :				exportinfo.format = gap::exporter::FORMAT_BINARY; 			break;
						case ade::hash::hash_ascii_string_as_lower("c_array") :				exportinfo.format = gap::exporter::FORMAT_C_ARRAY; 			break;
						case ade::hash::hash_ascii_string_as_lower("cpp_vector") :		exportinfo.format = gap::exporter::FORMAT_CPP_VECTOR; 	break;
						case ade::hash::hash_ascii_string_as_lower("cpp_stdarray") :	exportinfo.format = gap::exporter::FORMAT_CPP_STDARRAY; break;
						default :																											return on_error(line_number,std::string("Unknown export format! - ") + value);
					}
				}
				break;

			default : 
				// TODO: Warning - unknown arg
				break;
		}
	}

	m_export_info.push_back(exportinfo);

	return 0;
}

} // namespace gap

