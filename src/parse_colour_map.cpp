//=============================================================================
//	FILE:						parse_colour_map.cpp
//	SYSTEM:				 	game asset packer
//	DESCRIPTION:
//-----------------------------------------------------------------------------
//	COPYRIGHT:			(C)Copyright 2022 Adrian Purser. All Rights Reserved.
//	LICENCE:				MIT
//	MAINTAINER:			AJP - Adrian Purser <ade@arcadestuff.com>
//	CREATED:				11-MAY-2022 Adrian Purser <ade@arcadestuff.com>
//=============================================================================

#include <iostream>
#include <fstream>
#include <cstdint>
#include <vector>
#include <string>
#include <memory>
#include <mutex>
#include <filesystem>
#include "parse_colour_map.h"
#include "utility/loadfile.h"
#include "utility/tokenize.h"
#include "filesystem.h"
#include "export.h"


//=============================================================================
//
//	GIMP PALETTE
//
//=============================================================================
/*
GIMP Palette
#Palette Name: Zenith
#Description: My current main palette. A diverse range of colours with moderate saturation, great for designing videogames or making intricate and colorful artworks.
#Colors: 47
24	20	59	18143b
49	37	110	31256e
99	62	138	633e8a
163	80	147	a35093
230	106	106	e66a6a
255	164	128	ffa480
*/


static
gap::assets::ColourMap
load_gimp_palette( const std::string & filename, gap::FileSystem & filesystem )
{
	const auto data = filesystem.load(filename);

	std::string line;
	int line_number = 0;

	auto parse_line = [](const std::string & line, gap::assets::ColourMap & cmap)
		{
			auto tokens = ade::tokenize(line," \t");
			if(tokens.size() >= 3)
			{
				uint32_t c[3];
				for(int i=0;i<3;++i)
				{
					auto token = std::string(tokens[i]);
					c[i] = std::strtoul(token.c_str(),nullptr,10) & 0x0FF;
				}
				cmap.colourmap.push_back( 0x0FF000000 | (c[0]<<16) | (c[1]<<8) | c[2] );
			}
		};

	gap::assets::ColourMap cmap;
	cmap.source = filename;

	for(auto ch : data)
	{
		if((ch == 13) || (ch == 10))
		{
			auto pos = line.find_first_of("#");
			if(pos != std::string::npos)
				line = line.substr(0,pos);
			if( !line.empty() && !(line.substr(0,12) == "GIMP Palette") )
				parse_line(line,cmap);
			line.clear();
		}
		else
			line.push_back(ch);
	}

	if( !line.empty() && (line.substr(0,12) != "GIMP Palette") )
		parse_line(line,cmap);

	return cmap;
}



gap::assets::ColourMap 	load_colour_map( const std::string & filename, gap::FileSystem & filesystem )
{
	std::filesystem::path path(filename);
	auto ext = path.extension().string();
	std::transform(begin(ext),end(ext),begin(ext),::toupper);

	if(ext == ".GPL")		return load_gimp_palette(filename, filesystem);


	return {};
}

