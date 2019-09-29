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
		case ade::hash::hash_ascii_string_as_lower("I8") 				:	pf = gap::image::pixelformat::I8; 			break;
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
		case gap::image::pixelformat::I8				:	return("I8");
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


TargetImage			
CreateTargetImage(const SourceImage & source,bool little_endian)
{
	TargetImage	target( source.width, source.height, source.target_pixelformat );
	
	auto & data = target.data;
	int iout = 0;

	switch(target.pixelformat)
	{
		//-------------------------------------------------------------------------
		case gap::image::pixelformat::ARGB8888 :
		//-------------------------------------------------------------------------
			for(int y=0;y<source.height;++y)
			{
				for(int x=0;x<source.width;++x)
				{
					std::uint32_t pixel = source.get_pixel(x,y);
					for(int c=0;c<4;++c)
						data[iout++] = (little_endian ? (pixel >> (c*8)) : (pixel >> ((3-c)*8)) ) & 0x0FF; 
				}
			}	
			break;		

		//-------------------------------------------------------------------------
		case gap::image::pixelformat::RGB888 :	
		//-------------------------------------------------------------------------
			for(int y=0;y<source.height;++y)
			{
				for(int x=0;x<source.width;++x)
				{
					std::uint32_t pixel = source.get_pixel(x,y);
					for(int c=0;c<3;++c)
						data[iout++] = (little_endian ? (pixel >> (c*8)) : (pixel >> ((2-c)*8)) ) & 0x0FF; 
				}
			}	
			break;		

		//-------------------------------------------------------------------------
		case gap::image::pixelformat::RGB565 :
		//-------------------------------------------------------------------------
			for(int y=0;y<source.height;++y)
			{
				for(int x=0;x<source.width;++x)
				{
					std::uint32_t pixel = source.get_pixel(x,y);
					int out = ((pixel >> 8) & 0x0F800) | ((pixel >> 5) & 0x07E0) | ((pixel >> 3) & 0x01F);
					data[iout++] = (little_endian		? out & 0x0FF : (out >> 8) & 0x0FF);
					data[iout++] = (!little_endian 	? out & 0x0FF : (out >> 8) & 0x0FF);
				}
			}	
			break;		

		//-------------------------------------------------------------------------
		case gap::image::pixelformat::ARGB1555 :	
		//-------------------------------------------------------------------------
			for(int y=0;y<source.height;++y)
			{
				for(int x=0;x<source.width;++x)
				{
					std::uint32_t pixel = source.get_pixel(x,y);
					int out = ((pixel >> 16) & 0x08000) | ((pixel >> 9) & 0x07C00) | ((pixel >> 6) & 0x03E0) | ((pixel >> 3) & 0x01F);
					data[iout++] = (little_endian		? out & 0x0FF : (out >> 8) & 0x0FF);
					data[iout++] = (!little_endian 	? out & 0x0FF : (out >> 8) & 0x0FF);
				}
			}	
			break;		

		//-------------------------------------------------------------------------
		case gap::image::pixelformat::ARGB4444 :
		//-------------------------------------------------------------------------
			for(int y=0;y<source.height;++y)
			{
				for(int x=0;x<source.width;++x)
				{
					std::uint32_t pixel = source.get_pixel(x,y);
					int out = ((pixel >> 16) & 0x0F000) | ((pixel >> 12) & 0x0F00) | ((pixel >> 8) & 0x0F0) | ((pixel >> 4) & 0x0F);
					data[iout++] = (little_endian		? out & 0x0FF : (out >> 8) & 0x0FF);
					data[iout++] = (!little_endian 	? out & 0x0FF : (out >> 8) & 0x0FF);
				}
			}	
			break;		

		//-------------------------------------------------------------------------
		case gap::image::pixelformat::L8 :
		//-------------------------------------------------------------------------
			for(int y=0;y<source.height;++y)
			{
				for(int x=0;x<source.width;++x)
				{
					std::uint32_t pixel = source.get_pixel(x,y);
					data[iout++] = (((pixel >> 16) & 0x0FF) + ((pixel >> 8) & 0x0FF) + (pixel & 0x0FF)) / 3;
				}
			}	
			break;		

		//-------------------------------------------------------------------------
		case gap::image::pixelformat::AL44 :
		//-------------------------------------------------------------------------
			for(int y=0;y<source.height;++y)
			{
				for(int x=0;x<source.width;++x)
				{
					std::uint32_t pixel = source.get_pixel(x,y);
					data[iout++] = ((pixel >> 24) & 0x0F0) | (((((pixel >> 16) & 0x0FF) + ((pixel >> 8) & 0x0FF) + (pixel & 0x0FF)) / 3) >> 4);
				}
			}	
			break;		

		//-------------------------------------------------------------------------
		case gap::image::pixelformat::AL88			:	
		//-------------------------------------------------------------------------
			for(int y=0;y<source.height;++y)
			{
				for(int x=0;x<source.width;++x)
				{
					std::uint32_t pixel = source.get_pixel(x,y);
					const std::uint8_t l = (((pixel >> 16) & 0x0FF) + ((pixel >> 8) & 0x0FF) + (pixel & 0x0FF)) / 3;
					const std::uint8_t a = ((pixel >> 24) & 0x0FF);
					data[iout++] = (little_endian		? l : a);
					data[iout++] = (!little_endian 	? a : l);
				}
			}	
			break;		


		//-------------------------------------------------------------------------
		case gap::image::pixelformat::L4 :
		//-------------------------------------------------------------------------
			for(int y=0;y<source.height;++y)
			{
				for(int x=0;x<source.width;x+=2)
				{
					std::uint32_t pixel1 = source.get_pixel(x,y);
					std::uint32_t pixel2 = source.get_pixel(x+1,y);
					const std::uint8_t l1 = ((((pixel1 >> 16) & 0x0FF) + ((pixel1 >> 8) & 0x0FF) + (pixel1 & 0x0FF)) / 3) & 0x0F0;
					const std::uint8_t l2 = ((((pixel2 >> 16) & 0x0FF) + ((pixel2 >> 8) & 0x0FF) + (pixel2 & 0x0FF)) / 3) & 0x0F0;
					data[iout++] = (l1 >> 4) | l2;
				}
			}	
			break;		

		//-------------------------------------------------------------------------
		case gap::image::pixelformat::A8 :
		//-------------------------------------------------------------------------
			for(int y=0;y<source.height;++y)
			{
				for(int x=0;x<source.width;++x)
				{
					std::uint32_t pixel = source.get_pixel(x,y);
					data[iout++] = (pixel >> 24) & 0x0FF;
				}
			}	
			break;		

		//-------------------------------------------------------------------------
		case gap::image::pixelformat::A4 :
		//-------------------------------------------------------------------------
			for(int y=0;y<source.height;++y)
			{
				for(int x=0;x<source.width;x+=2)
				{
					std::uint32_t pixel1 = source.get_pixel(x,y);
					std::uint32_t pixel2 = source.get_pixel(x+1,y);
					const std::uint8_t a1 = (pixel1 >> 28) & 0x0F;
					const std::uint8_t a2 = (pixel2 >> 28) & 0x0F;
					data[iout++] = a1 | (a2 << 4);
				}
			}	
			break;		

		//-------------------------------------------------------------------------
		case gap::image::pixelformat::I8 :
		//-------------------------------------------------------------------------
			if(!source.palette.empty())
				target.palette = source.palette;
			else
			{
				// TODO: 
				//target.palette = create_palette(source,256);
				std::cout << "CreateTargetImage: create_palette is unimplemented, sorry!\n";
				break;
			}
			
			for(int y=0;y<source.height;++y)
			{
				for(int x=0;x<source.width;++x)
				{
					std::uint32_t pixel = source.get_pixel(x,y);
					int index = target.palette.find_colour(pixel);

					for(int c=0;c<4;++c)
						data[iout++] = (little_endian ? (pixel >> (c*8)) : (pixel >> ((3-c)*8)) ) & 0x0FF; 
				}
			}	
			break;		

		default : 
			break;
	}

	return target;

}




} // namespace gap::image

