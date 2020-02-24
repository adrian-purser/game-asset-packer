//=============================================================================
//	FILE:						tileset.h
//	SYSTEM:				 	game asset packer
//	DESCRIPTION:
//-----------------------------------------------------------------------------
//	COPYRIGHT:			(C)Copyright 2020 Adrian Purser. All Rights Reserved.
//	LICENCE:				MIT
//	MAINTAINER:			AJP - Adrian Purser <ade@arcadestuff.com>
//	CREATED:				24-FEB-2020 Adrian Purser <ade@arcadestuff.com>
//=============================================================================
#ifndef GUARD_ADE_GAME_ASSET_PACKER_TILESET_H
#define GUARD_ADE_GAME_ASSET_PACKER_TILESET_H

#include <cstdint>
#include <vector>
#include <string>
#include "filesystem.h"
#include "utility/hash.h"
#include "image.h"

namespace gap::tileset
{

enum
{
	ROTATE_0,
	ROTATE_90,
	ROTATE_180,
	ROTATE_270,
	
	FLIP_HORZ			= 0b00000100,
	FLIP_VERT			= 0b00001000
};

struct Tile
{
	uint32_t	x							= 0;
	uint32_t	y							=	0;
	uint16_t	source_image	= 0;			
	uint16_t	transform			= 0;
};

struct TileSet
{
	std::string							name;
	int											id 						= -1;
	int											tile_width 		= -1;
	int 										tile_height 	= -1;
	int											pixel_format 	= 0;
	std::vector<Tile>				tiles;
};

} // namespace gap

#endif // ! defined GUARD_ADE_GAME_ASSET_PACKER_TILESET_H
