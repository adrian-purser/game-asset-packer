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
#include "parse_gap.h"

namespace gap
{

int		
parse_gap(std::string_view source)
{
	std::string_view::size_type linestart = 0;
	int linenumber = 1;

	while(linestart != std::string_view::npos)
	{
		auto linebreak = source.find('\n',linestart);
		auto line = (linebreak == std::string_view::npos) ? source.substr(linestart) : source.substr(linestart,linebreak-linestart);

		auto ln_str = std::to_string(linenumber);
		ln_str.resize(6,' ');
	
		std::cout << ln_str << ':' << line << '\n';

		if(linebreak == std::string_view::npos)
			break;

		linestart = linebreak+1;
		++linenumber;
	}
	std::cout << std::endl;

	return 0;
}

} // namespace gap

