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

namespace gap
{

static int	parse_line(std::string_view line,int line_number,gap::assets::Assets * p_assets);

std::unique_ptr<gap::assets::Assets>		
parse_gap(std::string_view source)
{
	auto p_assets = std::make_unique<gap::assets::Assets>();

	std::string_view::size_type linestart = 0;
	int linenumber = 1;
	bool b_error = false;

	while((linestart != std::string_view::npos) && !b_error)
	{
		auto linebreak = source.find('\n',linestart);
		auto line = (linebreak == std::string_view::npos) ? source.substr(linestart) : source.substr(linestart,linebreak-linestart);

		if(parse_line(line,linenumber,p_assets.get()))
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

static int process_command(const std::vector<std::string> & tokens)
{


	return 0;
}

static int
parse_line(std::string_view line,int line_number,gap::assets::Assets * p_assets)
{
	std::string l(line);
	std::regex re(R"((,)(?=(?:[^"]|"[^"]*")*$))");
	std::sregex_token_iterator it(l.begin(), l.end(), re, -1);
	std::sregex_token_iterator reg_end;

	std::vector<std::string> tokens;

	for (; it != reg_end; ++it) 
    tokens.push_back(it->str());

	return process_command(tokens);
}


} // namespace gap

