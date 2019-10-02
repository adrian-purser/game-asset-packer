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
endian_insert(std::vector<std::uint8_t> & data,int index,int size,T value,bool big_endian)
{
	std::cout << "ENDIANINSERT: sizeof(T) = " << size << " data.size = " << data.size() << " index = " << index << std::endl;
	
	if((index + size) <= data.size())
	{
		for(int i=0;i<size;++i)
			data[index+i] = (big_endian ? value >> (((size-1)-i) * 8) : value >> (i*8)) & 0x0FF;
	}
}

static inline void
fourcc_insert(const char * fourcc,std::vector<std::uint8_t> & data,int index)
{
	for(int i=0;i<4;++i)
		data[index+i] = fourcc[i];
}


namespace gap
{


static
std::vector<std::uint8_t>		
encode_header(const gap::Configuration & config,std::uint32_t crc32)
{
	std::vector<std::uint8_t>		header(HEADER_SIZE);	

	header[0] = 'G';
	header[1] = 'B';
	header[2] = 'I';
	header[3] = 'N';
	header[4] = HEADER_SIZE;
	header[5] = config.b_big_endian ? HEADER_FLAG_BIG_ENDIAN : 0;
	header[6] = VERSION[0];
	header[7] = VERSION[1];
	
	endian_insert(header,8,4,std::uint32_t(crc32),config.b_big_endian);

	return header;
}

static
std::vector<std::uint8_t>		
encode_image_chunks(const gap::assets::Assets & assets,const gap::Configuration & config)
{
	std::vector<std::uint8_t>		chunk_image_data(8);
	std::vector<std::uint8_t>		chunk_image_meta(8);
	std::vector<std::uint32_t>	image_offsets;

	fourcc_insert("IMGD",chunk_image_data,0);
	fourcc_insert("SIMG",chunk_image_meta,0);

	std::uint32_t	image_offset = 0;
	assets.enumerate_source_images([&](const gap::image::SourceImage & image)
		{
			image_offsets.push_back(image_offset);
			auto data_size = (image.target_data_size() + 0x0F) & ~0x0F;
			const auto & target_data = image.target_data();
			
			chunk_image_data.insert(end(chunk_image_data),begin(target_data),end(target_data));

			image_offset += data_size; 
			assert(image_offset == (chunk_image_data.size() - 8));
		});
}

std::vector<std::uint8_t>		
encode_gbin(const gap::assets::Assets & assets,const gap::Configuration & config)
{

	auto image_chunks = encode_image_chunks(assets,config);
	// TODO : Create Target Images from the Source Images.
	// TODO :	Encode the source image data block
	// TODO : Encode the source image metadata block

	std::uint32_t crc32 = 0; // TODO: Calculate the crc from all of the chunks.
	auto header = encode_header(config,crc32);
	return header;
}

} // namespace gap

