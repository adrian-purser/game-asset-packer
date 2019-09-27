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

namespace param::loadimage 		{ enum {	CMD,	PATH,		PIXELFORMAT }; }
namespace param::imagegroup 	{ enum {	CMD,	GROUP,	ARG_COUNT 	}; }
namespace param::image 				{ enum {	CMD,	X,			Y,					WIDTH,				HEIGHT,		VARARGS }; }


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

	m_p_assets->dump();
	
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

	auto comment 	= line.find('#');
	auto ws 			= line.find_last_not_of(" \t\f\v\n\r",comment == std::string_view::npos ? line.size() : comment-1);
	auto eol 			= (comment == std::string_view::npos ? (ws == std::string_view::npos ? 0 : ws) : (ws == std::string_view::npos ? 0 : std::min(comment,ws)));
	if(eol == 0)
		return 0;

	std::string l(line.substr(0,eol+1));
	std::regex re(R"((,)(?=(?:[^"]|"[^"]*")*$))");
	std::sregex_token_iterator it(l.begin(), l.end(), re, -1);
	std::sregex_token_iterator reg_end;

	std::vector<std::string> tokens;

	for (;it != reg_end; ++it) 
	{
		auto token = it->str();
		token.erase(std::remove(begin(token),end(token),'"'),end(token));
    tokens.push_back(token);
	}

	//---------------------------------------------------------------------------
	// Process the command
	//---------------------------------------------------------------------------
	int result = 0;
	auto hash = ade::hash::hash_ascii_string_as_lower(tokens[0].c_str(),tokens[0].size());

	switch(hash)
	{
		case ade::hash::hash_ascii_string_as_lower(GAPCMD_LOADIMAGE) :	result = command_loadimage(line_number,tokens); break;
		case ade::hash::hash_ascii_string_as_lower(GAPCMD_IMAGEGROUP) :	result = command_imagegroup(line_number,tokens); break;
		case ade::hash::hash_ascii_string_as_lower(GAPCMD_IMAGE) :			result = command_image(line_number,tokens); break;

		default : 
			std::cerr << "GAP: Unknown command '" << tokens[0] << "'\n";
			result = -1;
			break;
	}

	return result;
}



int 
ParserGAP::command_loadimage(int line_number,const std::vector<std::string> & tokens)
{
	const auto argc = tokens.size();

	std::cout << "COMMAND:LOADIMAGE: ";
	for(const auto & token : tokens)
		std::cout << " [" << token << ']';
	std::cout << std::endl;

	if(tokens.size() <= param::loadimage::PATH)
		return on_error(line_number,"Missing image path!");

	auto image = gap::image::load(tokens[param::loadimage::PATH],m_filesystem);
	if(image.data.empty())
		return on_error(line_number,std::string("Failed to load image! - ") + tokens[param::loadimage::PATH]);

	if(tokens.size() > param::loadimage::PIXELFORMAT)
	{
		auto pf = gap::image::parse_pixelformat_name(tokens[param::loadimage::PIXELFORMAT]);
		if(pf == 0)
			return on_error(line_number,std::string("Unknown Pixel Format! - ") + tokens[param::loadimage::PIXELFORMAT]);
		image.target_pixelformat = pf;
	}
	else
		image.target_pixelformat = image.source_pixelformat;

	m_current_source_image = m_p_assets->add_source_image(std::move(image));

	std::cout << "  Image added into slot " << m_current_source_image << std::endl;
	return 0;
}

int 
ParserGAP::command_imagegroup(int line_number,const std::vector<std::string> & tokens)
{
	const auto argc = tokens.size();

	return 0;
}

int 
ParserGAP::command_image(int line_number,const std::vector<std::string> & tokens)
{
	const auto argc = tokens.size();

	return 0;
}


} // namespace gap

