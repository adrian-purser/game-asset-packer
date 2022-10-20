//=============================================================================
//	FILE:					tokenize.h
//	SYSTEM:	
//	DESCRIPTION:	Split a string into tokens using specified delimeters
//-----------------------------------------------------------------------------
//  COPYRIGHT:		(C) Copyright 2019 Adrian Purser. All Rights Reserved.
//	LICENCE:			MIT - See LICENSE file for details
//	MAINTAINER:		Adrian Purser <ade@adrianpurser.co.uk>
//	CREATED:			09-JUN-2019 Adrian Purser <ade@adrianpurser.co.uk>
//=============================================================================
#ifndef GUARD_ADE_TOKENIZE_H
#define GUARD_ADE_TOKENIZE_H

#include <iostream>
#include <fstream>
#include <cstdint>
#include <vector>
#include <string>
#include <string_view>

namespace ade
{

inline 
std::vector<std::string_view>
tokenize(	std::string_view str,
					std::string_view delimeters = ",",
					std::string_view quotes = "\"'" )
{
	std::vector<std::string_view> tokens;

	std::string_view::size_type index = 0;

	while(index < str.size())
	{
		auto ch = str[index];

		if((ch == ' ') || (ch == '\t'))
		{
			++index;
			continue;
		}

		auto qpos = quotes.find_first_of(ch);
		if(qpos != std::string_view::npos)
		{
			++index;
			if(index < str.size())
			{
				auto qt = quotes[qpos];
				auto qepos = str.find_first_of(qt,index);
				if(qepos == std::string_view::npos)
				{
					tokens.push_back(str.substr(index));
					index = str.size();
				}
				else
				{
					tokens.push_back(str.substr(index,qepos-index));
					index = qepos+1;

					auto dpos = str.find_first_of(delimeters,index);
					index = (dpos == std::string_view::npos ? str.size() : dpos + 1); 
				}
			}
		}
		else
		{
			auto dpos = delimeters.find_first_of(ch);
			if(dpos != std::string_view::npos)
			{
				tokens.push_back(std::string_view());
				index = dpos+1;
			}
			else
			{
				auto depos = str.find_first_of(delimeters,index);
				if(depos == std::string_view::npos)
				{
					tokens.push_back(str.substr(index));
					index = str.size();
				}
				else
				{
					tokens.push_back(str.substr(index,depos-index));
					index = depos+1;
				}
			}
		}
	}

	return tokens;
}

} // namespace ade


#endif // ! defined GUARD_ADE_TOKENIZE_H
