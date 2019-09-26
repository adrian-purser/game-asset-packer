//=============================================================================
//	FILE:						image.cpp
//	SYSTEM:				 	game asset packer
//	DESCRIPTION:
//-----------------------------------------------------------------------------
//	COPYRIGHT:			(C)Copyright 2019 Adrian Purser. All Rights Reserved.
//	LICENCE:				MIT
//	MAINTAINER:			AJP - Adrian Purser <ade@arcadestuff.com>
//	CREATED:				25-SEP-2019 Adrian Purser <ade@arcadestuff.com>
//=============================================================================
#include <iostream>
#include "image.h"
#include "adepng/adepng.h"

namespace gap::image
{

Image			
load(const std::string & filename,gap::FileSystem & filesystem)
{
	auto file = filesystem.load(filename);
	if(file.empty())
		return {};

	Image image;
	
	adepng::PNGDecode decode;
	if(decode.decode2(file.data(),file.size(),4))
	{
		std::cerr << "IMAGE: Failed to decode image!\n";
		return {};
	}

	std::cout << "LOADIMAGE: " << decode.width() << 'x' << decode.height() << " components=" << decode.components << std::endl;

	return image;
}


} // namespace gap::image

