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
	chunk_offset = data.size();
	fourcc_append("SIMG",data);
	fourcc_append("size",data);

	image_offset = 0;
	assets.enumerate_source_images([&](int image_index,const gap::image::SourceImage & image)->bool
		{
			auto data_size = (image.target_data_size() + 0x0F) & ~0x0F;
			endian_append(data,image.width(),2,config.b_big_endian);
			endian_append(data,image.height(),2,config.b_big_endian);
			endian_append(data,image_offset,4,config.b_big_endian);
			data.push_back(image.target_pixelformat());
			data.resize(data.size() + 3,0);
			image_offset += data_size; 
			return true;
		});

	endian_insert(data,std::uint32_t(data.size()-(chunk_offset+8)),chunk_offset+4,4,config.b_big_endian);

	//---------------------------------------------------------------------------
	//	Images
	//---------------------------------------------------------------------------
	chunk_offset = data.size();
	fourcc_append("SIMG",data);
	fourcc_append("size",data);

	assets.enumerate_images([&](int group_index,int image_index,const gap::image::Image & image)->bool
		{
			#error Finish this
			return true;
		});

	endian_insert(data,std::uint32_t(data.size()-(chunk_offset+8)),chunk_offset+4,4,config.b_big_endian);

}

std::vector<std::uint8_t>		
encode_gbin(const gap::assets::Assets & assets,const gap::Configuration & config)
{

	std::vector<std::uint8_t> data;

	encode_header(data,config);
	encode_image_chunks(data,assets,config);

	std::uint32_t crc32 = 0; // TODO: Calculate the crc from all of the chunks.

	return data;
}

} // namespace gap

