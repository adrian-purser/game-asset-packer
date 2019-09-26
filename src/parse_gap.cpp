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
#include "parse_gap.h"
#include "utility/hash.h"

#define GAPCMD_IMAGE		"image"

namespace gap
{

static int	parse_line(std::string_view line,int line_number,gap::assets::Assets & assets,gap::Filesystem & filesystem);

std::unique_ptr<gap::assets::Assets>		
parse_gap_file(std::string_view source,gap::Filesystem & filesystem)
{
	auto p_assets = std::make_unique<gap::assets::Assets>();

	std::string_view::size_type linestart = 0;
	int linenumber = 1;
	bool b_error = false;

	while((linestart != std::string_view::npos) && !b_error)
	{
		auto linebreak = source.find('\n',linestart);
		auto line = (linebreak == std::string_view::npos) ? source.substr(linestart) : source.substr(linestart,linebreak-linestart);

		if(parse_line(line,linenumber,*(p_assets.get()),filesystem))
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

	return p_assets;
}

static int command_image(int line_number,const std::vector<std::string> & tokens,gap::assets::Assets & assets,gap::Filesystem & filesystem)
{
	if(tokens.size() < 2)
	{
		std::cerr << "Line " << line_number << ": IMAGE: Missing image path!\n";
		return -1;
	}

	auto image = gap::image::load(tokens[0],filesystem);

	return 0;
}

static int
parse_line(std::string_view line,int line_number,gap::assets::Assets & assets,gap::Filesystem & filesystem)
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

	for (; it != reg_end; ++it) 
    tokens.push_back(it->str());

	//---------------------------------------------------------------------------
	// Process the command
	//---------------------------------------------------------------------------
	int result = 0;
	auto hash = ade::hash::hash_ascii_string_as_lower(tokens[0].c_str(),tokens[0].size());

	switch(hash)
	{
		case ade::hash::hash_ascii_string_as_lower(GAPCMD_IMAGE) :	result = command_image(line_number,tokens,assets,filesystem); break;;

		default : 
			std::cerr << "GAP: Unknown command '" << tokens[0] << "'\n";
			result = -1;
			break;
	}

	return result;
}


} // namespace gap

