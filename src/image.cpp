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


std::unique_ptr<SourceImage>
load(const std::string & filename,gap::FileSystem & filesystem)
{
	auto file = filesystem.load(filename);
	if(file.empty())
	{
		std::cerr << "LOADIMAGE: Failed to load image '" << filename << "'\n";
		return nullptr;
	}

	
	adepng::PNGDecode decode;
	if(decode.decode2(file.data(),file.size(),4))
	{
		std::cerr << "IMAGE: Failed to decode image!\n";
		return nullptr;
	}

	int width					= decode.width();
	int height				= decode.height();
	int pixelformat		= 0;
	
	switch(decode.source_components())
	{
		case 1 : 	pixelformat	= gap::image::pixelformat::A8;			 break;
		case 2 : 	pixelformat	= gap::image::pixelformat::AL88;		 break;
		case 3 : 	pixelformat	= gap::image::pixelformat::RGB888;	 break;
		case 4 : 	pixelformat	= gap::image::pixelformat::ARGB8888; break;
		default :	pixelformat	= 0;
	}

	auto p_image = std::make_unique<SourceImage>(width,height,decode.get_data());

	p_image->set_source_pixelformat(pixelformat);
	p_image->set_target_pixelformat(pixelformat);

	std::cout << "LOADIMAGE: " << decode.width() << 'x' << decode.height() 
						<< " components=" << decode.components() 
						<< " source_pixelformat=" << gap::image::get_pixelformat_name(pixelformat)
						<< std::endl;

	return p_image;
}


void
SourceImage::create_target_data(bool big_endian)
{
	auto & data = m_target_data;
	int iout = 0;

	data.resize(gap::image::pixelformat::image_data_size(m_target_pixelformat,m_width,m_height));

	switch(m_target_pixelformat)
	{
		//-------------------------------------------------------------------------
		case gap::image::pixelformat::ARGB8888 :
		//-------------------------------------------------------------------------
			for(int y=0;y<m_height;++y)
			{
				for(int x=0;x<m_width;++x)
				{
					std::uint32_t pixel = get_pixel(x,y);
					for(int c=0;c<4;++c)
						data[iout++] = (big_endian ? (pixel >> ((3-c)*8))  : (pixel >> (c*8)) ) & 0x0FF; 
				}
			}	
			break;		

		//-------------------------------------------------------------------------
		case gap::image::pixelformat::RGB888 :	
		//-------------------------------------------------------------------------
			for(int y=0;y<m_height;++y)
			{
				for(int x=0;x<m_width;++x)
				{
					std::uint32_t pixel = get_pixel(x,y);
					for(int c=0;c<3;++c)
						data[iout++] = (big_endian ? (pixel >> ((2-c)*8)) : (pixel >> (c*8)) ) & 0x0FF; 
				}
			}	
			break;		

		//-------------------------------------------------------------------------
		case gap::image::pixelformat::RGB565 :
		//-------------------------------------------------------------------------
			for(int y=0;y<m_height;++y)
			{
				for(int x=0;x<m_width;++x)
				{
					std::uint32_t pixel = get_pixel(x,y);
					int out = ((pixel >> 8) & 0x0F800) | ((pixel >> 5) & 0x07E0) | ((pixel >> 3) & 0x01F);
					data[iout++] = (!big_endian	? out & 0x0FF : (out >> 8) & 0x0FF);
					data[iout++] = (big_endian 	? out & 0x0FF : (out >> 8) & 0x0FF);
				}
			}	
			break;		

		//-------------------------------------------------------------------------
		case gap::image::pixelformat::ARGB1555 :	
		//-------------------------------------------------------------------------
			for(int y=0;y<m_height;++y)
			{
				for(int x=0;x<m_width;++x)
				{
					std::uint32_t pixel = get_pixel(x,y);
					int out = ((pixel >> 16) & 0x08000) | ((pixel >> 9) & 0x07C00) | ((pixel >> 6) & 0x03E0) | ((pixel >> 3) & 0x01F);
					data[iout++] = (!big_endian	? out & 0x0FF : (out >> 8) & 0x0FF);
					data[iout++] = (big_endian 	? out & 0x0FF : (out >> 8) & 0x0FF);
				}
			}	
			break;		

		//-------------------------------------------------------------------------
		case gap::image::pixelformat::ARGB4444 :
		//-------------------------------------------------------------------------
			for(int y=0;y<m_height;++y)
			{
				for(int x=0;x<m_width;++x)
				{
					std::uint32_t pixel = get_pixel(x,y);
					int out = ((pixel >> 16) & 0x0F000) | ((pixel >> 12) & 0x0F00) | ((pixel >> 8) & 0x0F0) | ((pixel >> 4) & 0x0F);
					data[iout++] = (!big_endian	? out & 0x0FF : (out >> 8) & 0x0FF);
					data[iout++] = (big_endian 	? out & 0x0FF : (out >> 8) & 0x0FF);
				}
			}	
			break;		

		//-------------------------------------------------------------------------
		case gap::image::pixelformat::L8 :
		//-------------------------------------------------------------------------
			for(int y=0;y<m_height;++y)
			{
				for(int x=0;x<m_width;++x)
				{
					std::uint32_t pixel = get_pixel(x,y);
					data[iout++] = (((pixel >> 16) & 0x0FF) + ((pixel >> 8) & 0x0FF) + (pixel & 0x0FF)) / 3;
				}
			}	
			break;		

		//-------------------------------------------------------------------------
		case gap::image::pixelformat::AL44 :
		//-------------------------------------------------------------------------
			for(int y=0;y<m_height;++y)
			{
				for(int x=0;x<m_width;++x)
				{
					std::uint32_t pixel = get_pixel(x,y);
					data[iout++] = ((pixel >> 24) & 0x0F0) | (((((pixel >> 16) & 0x0FF) + ((pixel >> 8) & 0x0FF) + (pixel & 0x0FF)) / 3) >> 4);
				}
			}	
			break;		

		//-------------------------------------------------------------------------
		case gap::image::pixelformat::AL88			:	
		//-------------------------------------------------------------------------
			for(int y=0;y<m_height;++y)
			{
				for(int x=0;x<m_width;++x)
				{
					std::uint32_t pixel = get_pixel(x,y);
					const std::uint8_t l = (((pixel >> 16) & 0x0FF) + ((pixel >> 8) & 0x0FF) + (pixel & 0x0FF)) / 3;
					const std::uint8_t a = ((pixel >> 24) & 0x0FF);
					data[iout++] = (!big_endian	? l : a);
					data[iout++] = (big_endian 	? a : l);
				}
			}	
			break;		


		//-------------------------------------------------------------------------
		case gap::image::pixelformat::L4 :
		//-------------------------------------------------------------------------
			for(int y=0;y<m_height;++y)
			{
				for(int x=0;x<m_width;x+=2)
				{
					std::uint32_t pixel1 = get_pixel(x,y);
					std::uint32_t pixel2 = get_pixel(x+1,y);
					const std::uint8_t l1 = ((((pixel1 >> 16) & 0x0FF) + ((pixel1 >> 8) & 0x0FF) + (pixel1 & 0x0FF)) / 3) & 0x0F0;
					const std::uint8_t l2 = ((((pixel2 >> 16) & 0x0FF) + ((pixel2 >> 8) & 0x0FF) + (pixel2 & 0x0FF)) / 3) & 0x0F0;
					data[iout++] = (l1 >> 4) | l2;
				}
			}	
			break;		

		//-------------------------------------------------------------------------
		case gap::image::pixelformat::A8 :
		//-------------------------------------------------------------------------
			for(int y=0;y<m_height;++y)
			{
				for(int x=0;x<m_width;++x)
				{
					std::uint32_t pixel = get_pixel(x,y);
					data[iout++] = (pixel >> 24) & 0x0FF;
				}
			}	
			break;		

		//-------------------------------------------------------------------------
		case gap::image::pixelformat::A4 :
		//-------------------------------------------------------------------------
			for(int y=0;y<m_height;++y)
			{
				for(int x=0;x<m_width;x+=2)
				{
					std::uint32_t pixel1 = get_pixel(x,y);
					std::uint32_t pixel2 = get_pixel(x+1,y);
					const std::uint8_t a1 = (pixel1 >> 28) & 0x0F;
					const std::uint8_t a2 = (pixel2 >> 28) & 0x0F;
					data[iout++] = a1 | (a2 << 4);
				}
			}	
			break;		

		//-------------------------------------------------------------------------
		case gap::image::pixelformat::I8 :
		//-------------------------------------------------------------------------
			if(m_palette.empty())
			{
				// TODO: 
				//target.palette = create_palette(source,256);
				std::cout << "CreateTargetImage: create_palette is unimplemented, sorry!\n";
				break;
			}
			
			for(int y=0;y<m_height;++y)
			{
				for(int x=0;x<m_width;++x)
				{
					std::uint32_t pixel = get_pixel(x,y);
					int index = m_palette.find_colour(pixel);

					for(int c=0;c<4;++c)
						data[iout++] = (big_endian ?  (pixel >> ((3-c)*8)) : (pixel >> (c*8)) ) & 0x0FF; 
				}
			}	
			break;		

		default : 
			break;
	}
}

SourceImage::SourceImage(int width,int height,const std::uint32_t * p_data,int line_offset)
	: m_width(width)
	, m_height(height)
{
	m_source_data.assign(p_data,p_data + ((width + line_offset) * height));
}

SourceImage::SourceImage(int width,int height,const std::uint8_t * p_data,int line_offset)
	: m_width(width)
	, m_height(height)
{
	m_source_data.reserve(width*height);
	int i=0;
	for(int y=0;y<height;++y,p_data+=line_offset*4)
		for(int x=0;x<width;++x,++i,p_data += 4) 	
			m_source_data.push_back(p_data[0] | (p_data[1]<<8) | (p_data[2]<<16) | (p_data[3]<<24));
}


} // namespace gap::image

