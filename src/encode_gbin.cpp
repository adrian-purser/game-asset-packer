//=============================================================================
//	FILE:					encode_gbin.cpp
//	SYSTEM:				Game Asset Packer
//	DESCRIPTION:
//-----------------------------------------------------------------------------
//  COPYRIGHT:		(C)Copyright 2019 Adrian Purser. All Rights Reserved.
//	LICENCE:
//	MAINTAINER:		AJP - Adrian Purser <ade&arcadestuff.com>
//	CREATED:			30-SEP-2019 Adrian Purser <ade&arcadestuff.com>
//=============================================================================

#include <iostream>
#include "encode_gbin.h"

#define HEADER_SIZE		16
#define VERSION "01"

enum
{
	HEADER_FLAG_BIG_ENDIAN
};


template<typename T>
static void
endian_append(std::vector<std::uint8_t> & data,T value,int size,bool big_endian)
{
	for(int i=0;i<size;++i)
		data.push_back((big_endian ? value >> (((size-1)-i) * 8) : value >> (i*8)) & 0x0FF);
}

template<typename T>
static void
endian_insert(std::vector<std::uint8_t> & data,const T & value,int index,int size,bool big_endian)
{
	if((index + size) <= data.size())
	{
		for(int i=0;i<size;++i)
			data[index+i] = (big_endian ? value >> (((size-1)-i) * 8) : value >> (i*8)) & 0x0FF;
	}
}

static inline void
fourcc_append(const char * fourcc,std::vector<std::uint8_t> & data)
{
	data.insert(end(data),fourcc,fourcc+4);
}


namespace gap
{


static
void	
encode_header(std::vector<std::uint8_t> & data,const gap::Configuration & config)
{
	data.reserve(data.size()+HEADER_SIZE);	

	fourcc_append("GBIN",data);
	data.push_back(HEADER_SIZE);
	data.push_back(config.b_big_endian ? HEADER_FLAG_BIG_ENDIAN : 0);
	data.push_back(VERSION[0]);
	data.push_back(VERSION[1]);
	fourcc_append("CRC-",data);
	fourcc_append("xxxx",data);

}

static
void		
encode_image_chunks(std::vector<std::uint8_t> & data,const gap::assets::Assets & assets,const gap::Configuration & config)
{
	/*
	//---------------------------------------------------------------------------
	//	Image Data Chunk
	//---------------------------------------------------------------------------
	uint32_t chunk_offset = data.size();

	fourcc_append("IMGD",data);
	fourcc_append("size",data);

	std::uint32_t	image_offset = 0;
	assets.enumerate_source_images([&](int image_index,const gap::image::SourceImage & image)->bool
		{
			auto data_size = (image.target_data_size() + 0x0F) & ~0x0F;
			const auto & target_data = image.target_data();
			
			std::uint32_t index = data.size();
			data.resize(index+data_size,0);
			std::copy(begin(target_data),end(target_data),begin(data)+index);

			image_offset += data_size; 
			//assert(image_offset == (chunk_image_data.size() - 8));
			return true;
		});

	endian_insert(data,std::uint32_t(data.size()-(chunk_offset+8)),chunk_offset+4,4,config.b_big_endian);

	//---------------------------------------------------------------------------
	//	Source Image Metadata
	//---------------------------------------------------------------------------
	std::vector<std::uint32_t>	image_offsets;

	chunk_offset = data.size();
	fourcc_append("SIMG",data);
	fourcc_append("size",data);

	image_offset = 0;
	assets.enumerate_source_images([&](int image_index,const gap::image::SourceImage & image)->bool
		{
			image_offsets.push_back(image_offset);
			auto data_size = (image.target_data_size() + 0x0F) & ~0x0F;
			endian_append(data,image.width(),2,config.b_big_endian);
			endian_append(data,image.height(),2,config.b_big_endian);
			endian_append(data,image_offset,4,config.b_big_endian);
			data.push_back(image.target_pixelformat());
			data.push_back(0);
			data.push_back(0);
			data.push_back(0);
			image_offset += data_size; 
			return true;
		});

	endian_insert(data,std::uint32_t(data.size()-(chunk_offset+8)),chunk_offset+4,4,config.b_big_endian);

	//---------------------------------------------------------------------------
	//	Images
	//---------------------------------------------------------------------------
	chunk_offset = data.size();
	fourcc_append("IMAG",data);
	fourcc_append("size",data);

	assets.enumerate_images([&](int group_index,int image_index,const gap::image::Image & image)->bool
		{
			endian_append(data,image.width,2,config.b_big_endian);
			endian_append(data,image.height,2,config.b_big_endian);
			endian_append(data,assets.get_target_line_stride(image.source_image)-image.width,2,config.b_big_endian);
			data.push_back(assets.get_target_pixelformat(image.source_image));
			data.push_back(0); // Flags
			endian_append(data,image.x_origin,2,config.b_big_endian);
			endian_append(data,image.y_origin,2,config.b_big_endian);
			endian_append(data,image_offsets[image.source_image] + assets.get_target_image_offset(image.source_image,image.x,image.y),4,config.b_big_endian);
			return true;
		});

	endian_insert(data,std::uint32_t(data.size()-(chunk_offset+8)),chunk_offset+4,4,config.b_big_endian);
*/
}

/*
IMAG Chunk
----------

          -------------------->
        0        1        2        3
    +--------+--------+--------+--------+
$00 |   I    |   M    |   A    |   G    |    FourCC defines chunk type - 4 Bytes
    +--------+--------+--------+--------+
$04 |               SIZE                |    Chunk Size - 4 Bytes
    +========+========+========+========+    Image - 12 bytes
$08 | WIDTH  | HEIGHT |X-Origin|Y-Origin|
    +--------+--------+--------+--------+
    |   LINE OFFSET   | PixFmt |Pallete |
    +-----------------+--------+--------+
		|         IMAGE DATA OFFSET         |
    +=================+=================+
    :                 .                 :
		:                 .                 :
		|                 .                 |
    +-----------------------------------+

*/

struct IMAGChunkEntry
{
	uint8_t				width;
	uint8_t				height;
	uint8_t				x_origin;
	uint8_t				y_origin;	
	uint16_t			line_offset;
	uint8_t 			pixel_format;
	uint8_t				palette;
	uint32_t			image_data_offset;
};

struct TSETChunkEntry
{
	uint8_t 			width;
	uint8_t				height;
	uint8_t				pixel_format;
	uint8_t				palette;
	uint16_t			tile_count;
	uint16_t			id;
	uint32_t 			image_data_offset;
};

static const uint32_t TSET_SHUNK_SIZE = 12;

static
void		
encode_packed_image_chunks(std::vector<std::uint8_t> & data,const gap::assets::Assets & assets,const gap::Configuration & config)
{
	struct ImageGroup
	{
		uint16_t 	base		= 0;			// The value of the first image in the group.
		uint32_t	index		= 0;			// The index of the first image in the group.
	};

	std::vector<IMAGChunkEntry>		images;
	std::vector<TSETChunkEntry>		tilesets;
	std::array<ImageGroup,256>		groups;
	uint32_t max_group = 0;

	//---------------------------------------------------------------------------
	//	Image Data Chunk [IMGD]
	//---------------------------------------------------------------------------
	uint32_t chunk_offset = data.size();
	uint32_t image_offset = 0;
	
	std::cout << "Encoding Chunk IMGD\n";

	fourcc_append("IMGD",data);
	fourcc_append("size",data);

	assets.enumerate_image_groups([&](uint32_t group_number,uint16_t base)->bool
		{
			std::cout << "  GROUP: " << group_number << " BASE: " << base << " INDEX: " << images.size() << '\n';

			groups[group_number].index 	= images.size();
			groups[group_number].base 	= base;

			if(group_number > max_group)
				max_group = group_number;

			//-----------------------------------------------------------------------
			//	IMAGES
			//-----------------------------------------------------------------------
			assets.enumerate_group_images(group_number,[&](int image_index,const gap::image::Image & image)->bool
			{
				auto p_image = assets.get_source_subimage(image.source_image,image.x,image.y,image.width,image.height);
				int ox = image.x_origin;
				int oy = image.y_origin;
				p_image->rotate(image.angle,ox,oy);

				IMAGChunkEntry imag;

				imag.width							= p_image->width();
				imag.height							= p_image->height();
				imag.x_origin						= ox;
				imag.y_origin						= oy;
				imag.line_offset				= assets.get_target_line_stride(image.source_image)-image.width;
				imag.pixel_format				= image.pixel_format;
				imag.palette						= 0;  //TODO: Get the palette index
				imag.image_data_offset	=	image_offset;

				images.push_back(imag);
				
				auto imgdata = p_image->create_sub_target_data(0,0,imag.width,imag.height,image.pixel_format,config.b_big_endian);
				//auto imgdata = assets.get_target_subimage(image.source_image,image.x,image.y,image.width,image.height,image.pixel_format,config.b_big_endian);

				std::cout << "get_target_subimage(x:0,y:0,w:" << (int)imag.width << ",h:" << (int)imag.height << ",pf: " << image.pixel_format << ") = " << imgdata.size() << " bytes\n";

				data.insert(end(data),begin(imgdata),end(imgdata));
				auto sz = (data.size() + 3) & ~3;
				if(sz > data.size())
					data.resize(sz);

				image_offset = data.size() - (chunk_offset+8);
				return true;
			});

			//-----------------------------------------------------------------------
			//	TILESETS
			//-----------------------------------------------------------------------
			assets.enumerate_tilesets( [&](const gap::tileset::TileSet & tileset)->bool
			{
				TSETChunkEntry tset;
				tset.width							= tileset.tile_width;
				tset.height							= tileset.tile_height;
				tset.pixel_format				= tileset.pixel_format;
				tset.palette						= 0;
				tset.tile_count					= tileset.tiles.size();
				tset.id									= tileset.id;
				tset.image_data_offset 	= image_offset;

				tilesets.push_back(tset);

				for(const auto & tile : tileset.tiles)
				{
					// TODO: Handle image transform (flip, rotate etc)
					auto p_image = assets.get_source_subimage(tile.source_image,tile.x,tile.y,tileset.tile_width,tileset.tile_height);

					switch(tile.transform & 0x03)
					{
						case gap::tileset::ROTATE_90 	: p_image->rotate_90(); break;
						case gap::tileset::ROTATE_180	: p_image->rotate_180(); break;
						case gap::tileset::ROTATE_270	: p_image->rotate_270(); break;
					}

					auto imgdata = p_image->create_sub_target_data(0,0,tileset.tile_width,tileset.tile_height,tileset.pixel_format,config.b_big_endian);

//					auto imgdata = assets.get_target_subimage(tile.source_image,tile.x,tile.y,tileset.tile_width,tileset.tile_height,tileset.pixel_format,config.b_big_endian);
					data.insert(end(data),begin(imgdata),end(imgdata));
				}
				
				auto sz = (data.size() + 3) & ~3;
				if(sz > data.size())
					data.resize(sz);

				image_offset = data.size() - (chunk_offset+8);

				return true;
			});

			return true;
		});

	endian_insert(data,std::uint32_t(data.size()-(chunk_offset+8)),chunk_offset+4,4,config.b_big_endian);

	//---------------------------------------------------------------------------
	//	Image Chunk [IMAG]
	//---------------------------------------------------------------------------
	std::cout << "Encoding Chunk IMAG\n";

	chunk_offset = data.size();
	fourcc_append("IMAG",data);
	fourcc_append("size",data);

	data.reserve(chunk_offset + (sizeof(IMAGChunkEntry) * images.size()));

	for(const auto & image : images)
	{
		data.push_back(image.width);
		data.push_back(image.height);
		data.push_back(image.x_origin);
		data.push_back(image.y_origin);
		endian_append(data,0,2,config.b_big_endian);
		data.push_back(image.pixel_format);
		data.push_back(image.palette);
		endian_append(data,image.image_data_offset,4,config.b_big_endian);
	}

	endian_insert(data,std::uint32_t(data.size()-(chunk_offset+8)),chunk_offset+4,4,config.b_big_endian);

	//---------------------------------------------------------------------------
	//	Image Groups [IGRP]
	//---------------------------------------------------------------------------
	std::cout << "Encoding Chunk IGRP\n";

	chunk_offset = data.size();
	fourcc_append("IGRP",data);
	fourcc_append("size",data);

	data.reserve(chunk_offset + (8 * (max_group+1)));

	for(int i=0;i<=max_group;++i)
	{
		const auto & group = groups[i];
		std::cout << "  GROUP: " << i << " BASE: " << group.base << " INDEX: " << group.index << '\n';
		
		endian_append(data,group.base,2,config.b_big_endian);
		endian_append(data,0,2,config.b_big_endian);
		endian_append(data,group.index,4,config.b_big_endian);
	}

	endian_insert(data,std::uint32_t(data.size()-(chunk_offset+8)),chunk_offset+4,4,config.b_big_endian);

	//---------------------------------------------------------------------------
	//	Tilesets [TSET]
	//---------------------------------------------------------------------------
	chunk_offset = data.size();
	fourcc_append("TSET",data);
	fourcc_append("size",data);

	data.reserve(chunk_offset + (TSET_SHUNK_SIZE * tilesets.size()));

	for(const auto & tileset : tilesets)
	{
		data.push_back(tileset.width);
		data.push_back(tileset.height);
		data.push_back(tileset.pixel_format);
		data.push_back(tileset.palette);
		endian_append(data,tileset.tile_count,2,config.b_big_endian);
		endian_append(data,tileset.id,2,config.b_big_endian);
		endian_append(data,tileset.image_data_offset,4,config.b_big_endian);
	}

	endian_insert(data,std::uint32_t(data.size()-(chunk_offset+8)),chunk_offset+4,4,config.b_big_endian);

}

std::vector<std::uint8_t>		
encode_gbin(const gap::assets::Assets & assets,const gap::Configuration & config)
{

	std::vector<std::uint8_t> data;

	encode_header(data,config);
	//encode_image_chunks(data,assets,config);
	encode_packed_image_chunks(data,assets,config);

	std::uint32_t crc32 = 0; // TODO: Calculate the crc from all of the chunks.

	return data;
}

} // namespace gap

