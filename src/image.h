//=============================================================================
//	FILE:						image.h
//	SYSTEM:				 	game asset packer
//	DESCRIPTION:
//-----------------------------------------------------------------------------
//	COPYRIGHT:			(C)Copyright 2019 Adrian Purser. All Rights Reserved.
//	LICENCE:				MIT
//	MAINTAINER:			AJP - Adrian Purser <ade@arcadestuff.com>
//	CREATED:				25-SEP-2019 Adrian Purser <ade@arcadestuff.com>
//=============================================================================
#ifndef GUARD_ADE_GAME_ASSET_PACKER_IMAGE_H
#define GUARD_ADE_GAME_ASSET_PACKER_IMAGE_H

#include <cstdint>
#include <vector>
#include <string>
#include "filesystem.h"

namespace gap::image
{

namespace pixelformat
{

enum
{
	ARGB8888 = 1,
	RGB888,
	RGB565,
	ARGB1555,
	ARGB4444,
	L8,
	AL44,
	AL88,
	L4,
	A8,
	A4
};

constexpr int	bytes_per_pixel(const std::uint8_t	pixel_format)
							{
								switch(pixel_format)
								{
									case gap::image::pixelformat::ARGB8888 :		return 4;
									case gap::image::pixelformat::RGB888 :			return 3;
									case gap::image::pixelformat::RGB565 :			
									case gap::image::pixelformat::ARGB1555 :
									case gap::image::pixelformat::ARGB4444 :	
									case gap::image::pixelformat::AL88 :				return 2;
									case gap::image::pixelformat::L8 :
									case gap::image::pixelformat::AL44 :
									case gap::image::pixelformat::A8 :					return 1;
									default : break;
								}
								return 0;
							}

} // namespace pixelformat


struct Image
{
	std::vector<std::uint32_t>	data;
	int													width 				= 0;
	int													height 				= 0;
	int													offset 				= 0;					// The offset from the end of a line to the start of the next line in pixels.

	Image 			duplicate_subimage(int x, int y, int width, int height)
							{
								Image image;
								// TODO:
								return image;
							}
};

Image			load(const std::string & filename,gap::FileSystem & filesystem);

} // namespace gap::image

#endif // ! defined GUARD_ADE_GAME_ASSET_PACKER_IMAGE_H
