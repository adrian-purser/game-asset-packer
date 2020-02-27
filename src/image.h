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
#include "utility/hash.h"

namespace gap::image
{

namespace pixelformat
{
/*
enum
{
	DMA2D_ARGB8888						=	0x00U,
	DMA2D_RGB888							=	0x01U,
	DMA2D_RGB565							=	0x02U,
	DMA2D_ARGB1555						=	0x03U,
	DMA2D_ARGB4444						=	0x04U,
	DMA2D_L8									=	0x05U,
	DMA2D_AL44								=	0x06U,
	DMA2D_AL88								=	0x07U,
	DMA2D_L4									=	0x08U,
	DMA2D_A8									=	0x09U,
	DMA2D_A4									=	0x0AU
};
*/

enum
{
	ARGB8888,
	RGB888,
	RGB565,
	ARGB1555,
	ARGB4444,
	L8,
	AL44,
	AL88,
	L4,
	A8,
	A4,
	I8,								//	Indexed (256 colour palette)
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
									case gap::image::pixelformat::A8 :					
									case gap::image::pixelformat::I8 :					return 1;
									default : break;
								}
								return 0;
							}

constexpr int	image_data_size(const std::uint8_t	pixel_format, int width, int height)
							{
								switch(pixel_format)
								{
									case gap::image::pixelformat::ARGB8888 :		return 4 * width * height;
									case gap::image::pixelformat::RGB888 :			return 3 * width * height;
									case gap::image::pixelformat::RGB565 :			
									case gap::image::pixelformat::ARGB1555 :
									case gap::image::pixelformat::ARGB4444 :	
									case gap::image::pixelformat::AL88 :				return 2 * width * height;
									case gap::image::pixelformat::L8 :
									case gap::image::pixelformat::AL44 :
									case gap::image::pixelformat::A8 :					
									case gap::image::pixelformat::I8 :					return width * height;
									case gap::image::pixelformat::L4 :
									case gap::image::pixelformat::A4 :					return (width/2) * height;

									default : break;
								}
								return 0;
							}

constexpr uint32_t
encode_pixel(uint32_t colour,uint8_t pixel_format)
{
	switch(pixel_format)
	{
		case gap::image::pixelformat::ARGB8888 :		return colour;
		case gap::image::pixelformat::RGB888 :			return colour & 0x0FFFFFF;
		case gap::image::pixelformat::RGB565 :			return ((colour >> 8) & 0x0F800) | ((colour >> 5) & 0x07E0) | ((colour >> 3) & 0x01F);
		case gap::image::pixelformat::ARGB1555 :		return ((colour >> 16) & 0x08000) | ((colour >> 9) & 0x7C00) | ((colour >> 6) & 0x03E0) | ((colour >> 3) & 0x01F);
		case gap::image::pixelformat::ARGB4444 :		return ((colour >> 16) & 0x0F000) | ((colour >> 12) & 0x0F00) | ((colour >> 8) & 0x0F0) | ((colour >> 4) & 0x0F);
		case gap::image::pixelformat::AL88 :				return ((colour >> 16) & 0x0FF00) | (( ((colour >> 16) & 0x0FF) + ((colour >> 8) & 0x0FF) + (colour & 0x0FF) ) / 3);
		case gap::image::pixelformat::L8 :					return (((colour >> 16) & 0x0FF) + ((colour >> 8) & 0x0FF) + (colour & 0x0FF) ) / 3;
		case gap::image::pixelformat::AL44 :				return ((colour >> 24) & 0x0F0) | ((( ((colour >> 16) & 0x0FF) + ((colour >> 8) & 0x0FF) + (colour & 0x0FF) ) / 3) >> 4);
		case gap::image::pixelformat::A8 :					return ((colour >> 24) & 0x0FF00);
		case gap::image::pixelformat::I8 :					return 0;
		case gap::image::pixelformat::L4 :					return ((((colour >> 16) & 0x0FF) + ((colour >> 8) & 0x0FF) + (colour & 0x0FF) ) / 3) >> 4;
		case gap::image::pixelformat::A4 :					return ((colour >> 28) & 0x0F);

		default : break;
	}
	return 0;
}

constexpr
std::uint32_t 		
image_pixel_offset(const std::uint8_t	pixel_format,int x,int y,int stride)
{
	switch(pixel_format)
	{
		case gap::image::pixelformat::L4 :
		case gap::image::pixelformat::A4 :
			return ((stride * y) + x) / 2;	
			break;

		default :
			break; 
	}

	return ((stride * y) + x) * bytes_per_pixel(pixel_format);
}



} // namespace pixelformat

std::uint8_t 		parse_pixelformat_name(const std::string & name);
std::string			get_pixelformat_name(std::uint8_t pixelformat);

//=============================================================================
//	Palette
//=============================================================================
class Palette
{
private:
	std::vector<std::uint32_t>		m_palette;

public:
	Palette() = default;
	explicit Palette(int size) : m_palette(size) {}
	Palette(const std::vector<std::uint32_t> palette) : m_palette(palette) {}

	std::uint32_t & operator[](int index)		{return m_palette[index];}
	void						resize(int size)				{m_palette.resize(size);}
	bool						empty()	const 					{return m_palette.empty();}
	
	int							find_colour(std::uint32_t colour) const
									{
										int index = 0;
										for(auto c : m_palette)
										{
											if(c == colour)
												return index;
											++index;
										}
										return -1;	
									}

	void						set_colour(int index,std::uint32_t colour)
									{
										if(index>=0)
										{
											if(index >= m_palette.size())
												resize(index+1);
											m_palette[index] = colour; 
										}
									}
};

//=============================================================================
//	Source Image Data
//=============================================================================
class
SourceImage
{
private:
	std::vector<std::uint32_t>	m_source_data;
	Palette											m_palette;													// Palette if a palettized image
	int													m_width 							= 0;
	int													m_height 							= 0;
	int													m_line_offset 				= 0;					// The offset from the end of a line to the start of the next line in pixels.
	int													m_parent							= -1;					// The index of the parent image if this was copied as a subimage of another image.
	std::uint8_t 								m_source_pixelformat	= 0;
	std::uint8_t 								m_target_pixelformat	= 0;
	std::uint16_t								m_flags								= 0;

public:
	SourceImage() = default;
	SourceImage(int width,int height,const std::uint32_t * p_data = nullptr,int line_offset = 0);
	SourceImage(int width,int height,const std::uint8_t * p_data = nullptr,int line_offset = 0);
	
	int 															width() const 								{return m_width;}
	int 															height() const 								{return m_height;}
	std::uint8_t 											source_pixelformat() const 		{return m_source_pixelformat;}
	std::uint8_t 											target_pixelformat() const 		{return m_target_pixelformat;}
//	int																target_data_size() const 			{return m_target_data.size();}
//	const std::vector<std::uint8_t> &	target_data() const 					{return m_target_data;}

	void 															set_source_pixelformat(std::uint8_t pixelformat)		{m_source_pixelformat = pixelformat;}
	void 															set_target_pixelformat(std::uint8_t pixelformat)		{m_target_pixelformat = pixelformat;}

	std::uint32_t 		get_pixel(int x, int y) const
										{
											return m_source_data[(y * (m_width)) + x];
										}

	std::unique_ptr<SourceImage>	duplicate_subimage(int x, int y, int width, int height)
										{
											// TODO:
											return nullptr;
										}

//	void									create_target_data(bool big_endian);
	std::vector<uint8_t>	create_sub_target_data(int x, int y, int width, int height, uint8_t pixel_format, bool big_endian);
};

//=============================================================================
//	Target Image Data
//=============================================================================
/*
struct TargetImage
{
	std::vector<std::uint8_t>		data;
	Palette											palette;													// Palette if a palettized image
	std::uint32_t								chunk_offset				= 0;
	int													width 							= 0;
	int													height 							= 0;
	std::uint8_t 								pixelformat					= 0;

	TargetImage() = default;
	TargetImage( int w, int h, std::uint8_t pf )
		: data(gap::image::pixelformat::image_data_size(pf,w,h))
		, width(w)
		, height(h)
		, pixelformat(pf)
		{}

};
*/

//=============================================================================
//	Image - Describes image region. References Source Image
//=============================================================================
struct Image
{
	std::string				name;
	std::uint16_t			source_image 		= 0;			
	int								x								= 0;
	int								y								=	0;
	int								width						= 0;
	int								height					= 0;
	int								x_origin				= 0;
	int								y_origin				= 0;
	int								pixel_format		= 0;
};



std::unique_ptr<SourceImage>			load(const std::string & filename,gap::FileSystem & filesystem);
//TargetImage			CreateTargetImage(const SourceImage & source,bool big_endian);

} // namespace gap::image

#endif // ! defined GUARD_ADE_GAME_ASSET_PACKER_IMAGE_H
