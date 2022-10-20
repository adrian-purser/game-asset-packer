//=============================================================================
//	FILE:					loadfile.h
//	SYSTEM:	
//	DESCRIPTION:	Load a binary file into a vector
//-----------------------------------------------------------------------------
//  COPYRIGHT:		(C) Copyright 2019 Adrian Purser. All Rights Reserved.
//	LICENCE:			MIT - See LICENSE file for details
//	MAINTAINER:		Adrian Purser <ade@adrianpurser.co.uk>
//	CREATED:			06-JUN-2019 Adrian Purser <ade@adrianpurser.co.uk>
//=============================================================================
#ifndef GUARD_ADE_LOADFILE_H
#define GUARD_ADE_LOADFILE_H

#include <iostream>
#include <fstream>
#include <cstdint>
#include <vector>
#include <string>
#include <string_view>

namespace ade
{

inline 
std::vector<std::uint8_t>
loadfile(const std::string_view filename)
{
	std::ifstream file;
	std::vector<std::uint8_t> data;

	file.open(filename.data(),std::ios_base::in | std::ios_base::binary | std::ios_base::ate);
	if(!file.fail())
	{
		file.seekg(0, std::ios::end);
		std::streampos len = file.tellg();

		if(len>0)
		{
			data.resize(len);
			file.seekg(0,std::ios::beg);
			file.read(reinterpret_cast<char *>(data.data()),len);
		}
	}
	return data;
}

} // namespace ade

#endif // ! defined GUARD_ADE_LOADFILE_H
