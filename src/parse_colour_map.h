//=============================================================================
//	FILE:						parse_colour_map.h
//	SYSTEM:				 	game asset packer
//	DESCRIPTION:
//-----------------------------------------------------------------------------
//	COPYRIGHT:			(C)Copyright 2022 Adrian Purser. All Rights Reserved.
//	LICENCE:				MIT
//	MAINTAINER:			AJP - Adrian Purser <ade@arcadestuff.com>
//	CREATED:				11-MAY-2022 Adrian Purser <ade@arcadestuff.com>
//=============================================================================
#ifndef GUARD_ADE_GAME_ASSET_PACKER_PARSE_COLOUR_MAP_H
#define GUARD_ADE_GAME_ASSET_PACKER_PARSE_COLOUR_MAP_H

#include <cstdint>
#include <vector>
#include <string>
#include "assets.h"


gap::assets::ColourMap 	load_colour_map( const std::string & filename, gap::FileSystem & filesystem );


#endif // ! defined GUARD_ADE_GAME_ASSET_PACKER_PARSE_COLOUR_MAP_H

