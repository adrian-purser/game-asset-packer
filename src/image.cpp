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
#include <cmath>
#include "image.h"
#include "adepng.h"

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
	if(decode.decode(file.data(),file.size(),4))
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

SourceImage::SourceImage(int width,int height,const std::uint32_t * p_data,int line_offset)
	: m_width(width)
	, m_height(height)
{
	m_source_data.reserve(width*height);
	for(int y=0;y<height;++y,p_data+=line_offset)
		for(int x=0;x<width;++x) 	
			m_source_data.push_back(*p_data++);
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


std::vector<uint8_t>	
SourceImage::create_sub_target_data(int x, int y, int width, int height, uint8_t pixel_format, bool big_endian)
{
	std::vector<uint8_t>	data;

	data.reserve(gap::image::pixelformat::image_data_size(pixel_format,width,height));
	int bpp = gap::image::pixelformat::bytes_per_pixel(pixel_format);

	switch(pixel_format)
	{
		case gap::image::pixelformat::L4 :
		case gap::image::pixelformat::A4 :
		case gap::image::pixelformat::I8 :
			std::cerr << "create_sub_target_data: Unsupported Pixel Format!" << std::endl;
			break;

		default :
			for(int iy=0;iy<height;++iy)
			{
				for(int ix=0;ix<width;++ix)
				{
					std::uint32_t pixel = gap::image::pixelformat::encode_pixel(get_pixel(x+ix,y+iy),pixel_format);
					for(int c=0;c<bpp;++c)
						data.push_back((big_endian ? (pixel >> (((bpp-1)-c)*8))  : (pixel >> (c*8)) ) & 0x0FF); 
				}
			}	
			break;
	}

	return data;
}

/*
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
*/

std::unique_ptr<SourceImage>	
SourceImage::duplicate_subimage(int x, int y, int width, int height)
{
	return std::make_unique<SourceImage>(width,height,get_pixel_address(x,y),m_width-width);
}

void
SourceImage::rotate(float angle,int & originx, int & originy)
{
	if(angle == 0.0f)
		return;

	const int ox = originx;
	const int oy = originy;

	if(angle == 90.0f)				{rotate_90(); 	originx = m_width-oy; 	originy = ox;}	
	else if(angle == 180.0f)	{rotate_180();	originx = m_width-ox; 	originy = m_height-oy;}
	else if(angle == 270.0f)	{rotate_270();	originx = oy; 					originy = m_height-ox;}
	else
	{
		const float rad 	= (180.0f - angle) * (M_PI / 180.0f);
		const float s			= sin(rad);
		const float c			= cos(rad);

		const int 	side 	= (std::max(m_width,m_height) * 3);
		const int 	half 	= side/2;

		std::vector<uint32_t> buffer;
		buffer.reserve((side*3) * (side*3));
		int minx = side-1;
		int maxx = 0;
		int miny = side-1;
		int maxy = 0;

		for(int y=0;y<side;++y)
		{
			for(int x=0;x<side;++x)
			{
				const float xx = (float)(half-x);
				const float yy = (float)(half-y);
				const float xr = (xx*c) - (yy*s);
				const float yr = (xx*s) + (yy*c);
				int xi = (int)xr + originx;
				int yi = (int)yr + originy;
				float xf = xr + (float)originx;
				float yf = yr + (float)originy;

				if((xi<0) || (yi<0) || (xi>=m_width) || (yi>=m_height))
					buffer.push_back(0);
				else
				{
					auto colour = get_pixel(xf,yf);
					buffer.push_back(colour);
					if(colour != 0)
					{
						if(x > maxx)		maxx = x;
						if(x < minx)		minx = x;
						if(y > maxy)		maxy = y;
						if(y < miny)		miny = y;
					}
				}
			}
		}

		originx = half-minx;
		originy = half-miny;

		m_width 	= (maxx - minx) + 1;
		m_height 	= (maxy - miny) + 1;

		m_source_data.clear();
		m_source_data.reserve(m_width * m_height);
		
		for(int y=miny;y<=maxy;++y)
			for(int x=minx;x<=maxx;++x)
				m_source_data.push_back(buffer[(y*side) + x]);
	}
	
}

void
SourceImage::rotate_90()
{
	std::vector<uint32_t>		buffer;
	buffer.reserve(m_source_data.size());

	for(int x=0;x<m_width;++x)
		for(int y=m_height-1;y>=0;--y)
			buffer.push_back(get_pixel(x,y));

	std::swap(m_source_data,buffer);
	std::swap(m_width,m_height);
}

void
SourceImage::rotate_180()
{
	std::vector<uint32_t>		buffer;
	buffer.reserve(m_source_data.size());

	for(int y=m_height-1;y>=0;--y)
		for(int x=m_width-1;x>=0;--x)
			buffer.push_back(get_pixel(x,y));

	std::swap(m_source_data,buffer);
}

void
SourceImage::rotate_270()
{
	std::vector<uint32_t>		buffer;
	buffer.reserve(m_source_data.size());

	for(int x=m_width-1;x>=0;--x)
		for(int y=0;y<m_height;++y)
			buffer.push_back(get_pixel(x,y));

	std::swap(m_source_data,buffer);
	std::swap(m_width,m_height);
}

void
SourceImage::horizontal_flip()
{

}

void
SourceImage::vertical_flip()
{

}

} // namespace gap::image

