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

std::uint8_t 
parse_pixelformat_name(const std::string & name)
{
	auto hash = ade::hash::hash_ascii_string_as_lower(name.c_str(),name.size());
	std::uint8_t pf = 0;

	switch(hash)
	{
		case ade::hash::hash_ascii_string_as_lower("ARGB8888") 	:	pf = gap::image::pixelformat::ARGB8888; break;
		case ade::hash::hash_ascii_string_as_lower("RGB888") 		:	pf = gap::image::pixelformat::RGB888; 	break;
		case ade::hash::hash_ascii_string_as_lower("RGB565") 		:	pf = gap::image::pixelformat::RGB565; 	break;
		case ade::hash::hash_ascii_string_as_lower("ARGB1555") 	:	pf = gap::image::pixelformat::ARGB1555; break;
		case ade::hash::hash_ascii_string_as_lower("ARGB4444") 	:	pf = gap::image::pixelformat::ARGB4444; break;
		case ade::hash::hash_ascii_string_as_lower("L8") 				:	pf = gap::image::pixelformat::L8; 			break;
		case ade::hash::hash_ascii_string_as_lower("AL44") 			:	pf = gap::image::pixelformat::AL44; 		break;
		case ade::hash::hash_ascii_string_as_lower("AL88") 			:	pf = gap::image::pixelformat::AL88;			break;
		case ade::hash::hash_ascii_string_as_lower("L4") 				:	pf = gap::image::pixelformat::L4; 			break;
		case ade::hash::hash_ascii_string_as_lower("A8") 				:	pf = gap::image::pixelformat::A8; 			break;
		case ade::hash::hash_ascii_string_as_lower("A4") 				:	pf = gap::image::pixelformat::A4; 			break;
		default : break;
	}

	return pf;
}

std::string			
get_pixelformat_name(std::uint8_t pixelformat)
{
	switch(pixelformat)
	{
		case gap::image::pixelformat::ARGB8888 	:	return("ARGB8888");
		case gap::image::pixelformat::RGB888 		:	return("RGB888");
		case gap::image::pixelformat::RGB565		:	return("RGB565");
		case gap::image::pixelformat::ARGB1555	:	return("ARGB1555");
		case gap::image::pixelformat::ARGB4444	:	return("ARGB4444");
		case gap::image::pixelformat::L8				:	return("L8");
		case gap::image::pixelformat::AL44			:	return("AL44");
		case gap::image::pixelformat::AL88			:	return("AL88");
		case gap::image::pixelformat::L4				:	return("L4");
		case gap::image::pixelformat::A8				:	return("A8");
		case gap::image::pixelformat::A4				:	return("A4");
	}
	return std::string();
}


SourceImage			
load(const std::string & filename,gap::FileSystem & filesystem)
{
	auto file = filesystem.load(filename);
	if(file.empty())
	{
		std::cerr << "LOADIMAGE: Failed to load image '" << filename << "'\n";
		return {};
	}

	SourceImage image;
	
	adepng::PNGDecode decode;
	if(decode.decode2(file.data(),file.size(),4))
	{
		std::cerr << "IMAGE: Failed to decode image!\n";
		return {};
	}

	image.width		= decode.width();
	image.height	= decode.height();
	image.offset	= 0;
	
	switch(decode.source_components())
	{
		case 1 : 	image.source_pixelformat	= gap::image::pixelformat::A8; break;
		case 2 : 	image.source_pixelformat	= gap::image::pixelformat::AL88; break;
		case 3 : 	image.source_pixelformat	= gap::image::pixelformat::RGB888; break;
		case 4 : 	image.source_pixelformat	= gap::image::pixelformat::ARGB8888; break;
		default :	image.source_pixelformat	= 0;
	}

	image.data.resize(image.width * image.height);

	auto p_img_data = reinterpret_cast<std::uint8_t *>(image.data.data());
//	std::copy(decode.get_data(),decode.get_data()+(image.width * image.height * 4),p_img_data);

	std::cout << "LOADIMAGE: " << decode.width() << 'x' << decode.height() 
						<< " components=" << decode.components() 
						<< " source_pixelformat=" << gap::image::get_pixelformat_name(image.source_pixelformat)
						<< std::endl;

	return image;
}


} // namespace gap::image

