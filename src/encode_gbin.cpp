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
#include <string_view>
#include <utility>
#include <format>
#include <print>

#include "encode_gbin.h"

#define HEADER_SIZE		32
#define VERSION "02"

enum
{
	HEADER_FLAG_BIG_ENDIAN			= (1<<0),
	HEADER_FLAG_EXECUTABLE			= (1<<1)
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
	if( std::cmp_less_equal((index + size) , data.size()) )
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
encode_header(	std::vector<std::uint8_t> & 	data,
								std::string_view 							name,
								const gap::assets::Assets & 	assets,
								const gap::Configuration & 		config )
{
	data.reserve(data.size()+HEADER_SIZE);

	//---------------------------------------------------------------------------
	// Calculate flags
	//---------------------------------------------------------------------------
	uint8_t flags = config.b_big_endian ? HEADER_FLAG_BIG_ENDIAN : 0;

	// ----- Search for a CODE file. Set executable flag if found. -----
	assets.enumerate_files([&](const gap::assets::FileInfo & fileinfo)->bool
	{
		if( fileinfo.type == ade::hash::fourcc("CODE") )
		{
			flags |= HEADER_FLAG_EXECUTABLE;
			return false;
		}
		return true;
	});

	//---------------------------------------------------------------------------
	// Encode Chunk
	//---------------------------------------------------------------------------
	fourcc_append("GBIN",data);
	data.push_back(HEADER_SIZE);
	data.push_back(flags);
	data.push_back(VERSION[0]);
	data.push_back(VERSION[1]);

	// ----- Placeholder for CRC. Will be filled in later -----
	fourcc_append("CRC-",data);

	// ----- Name -----
	for(int i=0;i<12;++i)
		data.push_back( std::cmp_less(i, name.size()) ? name[i] : 0);

	// ----- Reserved Fields -----
	fourcc_append("xxxx",data);
	fourcc_append("xxxx",data);

}


/*
IMAG Chunk
----------

Version 01
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

Version 02
----------

          -------------------->
        0        1        2        3
    +--------+--------+--------+--------+
$00 |   I    |   M    |   A    |   G    |    FourCC defines chunk type - 4 Bytes
    +--------+--------+--------+--------+
$04 |               SIZE                |    Chunk Size - 4 Bytes
    +========+========+========+========+    Image - 16 bytes
$08 |      WIDTH      |      HEIGHT     |
    +-----------------+-----------------+
    |    X-Origin     |    Y-Origin     |
    +-----------------+--------+--------+
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
	uint16_t			width;
	uint16_t			height;
	uint16_t			x_origin;
	uint16_t			y_origin;
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
		std::string	name;
		uint16_t 		base		= 0;			// The id of the first image in the group.
		uint16_t		size		= 0;
		uint32_t		index		= 0;			// The image index of the first image in the group.
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

	bool b_have_image_data = false;
	assets.enumerate_image_groups( [&](const std::string & /*name*/ ,uint32_t /*group_number*/,uint16_t /*base*/, uint16_t /*size*/ )->bool	{	b_have_image_data = true; return false; });
	assets.enumerate_tilesets( [&](const gap::tileset::TileSet & /*tileset*/)->bool {	b_have_image_data = true; return false; });

	if(b_have_image_data)
	{
		std::cout << "Encoding Chunk IMGD\n";

		fourcc_append("IMGD",data);
		fourcc_append("size",data);

		assets.enumerate_image_groups([&](const std::string & name,uint32_t group_number,uint16_t base, uint16_t size)->bool
			{
//				std::cout << "  GROUP: " << group_number << " BASE: " << base << " INDEX: " << images.size() << '\n';

				groups[group_number].name 	= name;
				groups[group_number].index 	= images.size();
				groups[group_number].base 	= base;
				groups[group_number].size 	= size;

				if(group_number > max_group)
					max_group = group_number;

				//-----------------------------------------------------------------------
				//	IMAGES
				//-----------------------------------------------------------------------
				assets.enumerate_group_images(group_number,[&](int /*image_index*/,const gap::image::Image & image)->bool
				{
					auto p_image = assets.get_source_subimage(image.source_image,image.x,image.y,image.width,image.height);

					if(image.b_hflip)	p_image->horizontal_flip();
					if(image.b_vflip)	p_image->vertical_flip();

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
/*
					std::cout << "get_target_subimage(x:0,y:0,w:" << (int)imag.width << ",h:" << (int)imag.height
										<< ",pf: " << image.pixel_format << ") = " << imgdata.size() << " bytes"
										<< " name:" << image.name << '\n';
*/
					data.insert(end(data),begin(imgdata),end(imgdata));
					auto sz = (data.size() + 3) & ~3;
					if(sz > data.size())
						data.resize(sz);

					image_offset = data.size() - (chunk_offset+8);
					return true;
				});
				return true;
			});
	}

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

			std::cout << std::format("GBIN:TILESET: id={}, name={}, tilesize = {}x{}, {} tiles\n",tileset.id,tileset.name,tileset.tile_width,tileset.tile_height,tileset.tiles.size());

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
				if(tile.transform & gap::tileset::FLIP_HORZ)	p_image->horizontal_flip();
				if(tile.transform & gap::tileset::FLIP_VERT)	p_image->vertical_flip();

				auto imgdata = p_image->create_sub_target_data(0,0,tileset.tile_width,tileset.tile_height,tileset.pixel_format,config.b_big_endian);

	//					auto imgdata = assets.get_target_subimage(tile.source_image,tile.x,tile.y,tileset.tile_width,tileset.tile_height,tileset.pixel_format,config.b_big_endian);
	//			std::cout << std::format("  TILE: x:{} y:{} dim:{}x{}, datasize:{}\n",tile.x,tile.y,p_image->width(),p_image->height(),imgdata.size());
				data.insert(end(data),begin(imgdata),end(imgdata));
			}

			auto sz = (data.size() + 3) & ~3;
			if(sz > data.size())
				data.resize(sz);

			image_offset = data.size() - (chunk_offset+8);

			return true;
		});


	endian_insert(data,std::uint32_t(data.size()-(chunk_offset+8)),chunk_offset+4,4,config.b_big_endian);

	//---------------------------------------------------------------------------
	//	Image Chunk [IMAG]
	//---------------------------------------------------------------------------
	if(!images.empty())
	{
		std::cout << "Encoding Chunk IMAG: " << images.size() << " images\n";

		chunk_offset = data.size();
		fourcc_append("IMAG",data);
		fourcc_append("size",data);

		data.reserve(chunk_offset + (sizeof(IMAGChunkEntry) * images.size()));

		for(const auto & image : images)
		{
			endian_append(data,	image.width,		2,	config.b_big_endian);
			endian_append(data,	image.height,		2,	config.b_big_endian);
			endian_append(data,	image.x_origin,	2,	config.b_big_endian);
			endian_append(data,	image.y_origin,	2,	config.b_big_endian);
			endian_append(data,	0,							2,	config.b_big_endian);
			data.push_back(image.pixel_format);
			data.push_back(image.palette);
			endian_append(data,image.image_data_offset,4,config.b_big_endian);
		}

		endian_insert(data,std::uint32_t(data.size()-(chunk_offset+8)),chunk_offset+4,4,config.b_big_endian);
	}

	//---------------------------------------------------------------------------
	//	Image Groups [IGRP]
	//---------------------------------------------------------------------------
	std::cout << "Encoding Chunk IGRP\n";

	chunk_offset = data.size();
	fourcc_append("IGRP",data);
	fourcc_append("size",data);

	data.reserve(chunk_offset + (24 * (max_group+1)));

	for(uint32_t i=0;i<=max_group;++i)
	{
		const auto & group = groups[i];
//		std::cout << "  GROUP: " << i << " BASE: " << group.base << " INDEX: " << group.index << '\n';

		endian_append(data,group.base,2,config.b_big_endian);
		endian_append(data,group.size,2,config.b_big_endian);
		endian_append(data,group.index,4,config.b_big_endian);

		for(size_t i=0;i<15;++i)	data.push_back( i<group.name.size() ? group.name[i] : 0 );
			data.push_back(0);
	}

	endian_insert(data,std::uint32_t(data.size()-(chunk_offset+8)),chunk_offset+4,4,config.b_big_endian);

	//---------------------------------------------------------------------------
	//	Image Seqnence [ISEQ]
	//---------------------------------------------------------------------------
	if(assets.image_sequence_count() > 0)
	{
		// ----- Encode the IFRMs first and keep a record of the frame indices -----
		std::cout << "Encoding Chunk IFRM\n";

		std::vector<uint32_t> frames;
		uint32_t frame_index = 0;

		chunk_offset = data.size();
		fourcc_append("IFRM",data);
		fourcc_append("size",data);

		assets.enumerate_image_sequences([&]([[maybe_unused]] uint32_t id, const gap::assets::ImageSequence & imgseq)->bool
			{
				std::cout << std::format("  ISEQ: {:3} : {} COUNT: {}\n", id, imgseq.name, imgseq.frames.size());

				frames.push_back(frame_index);
				frame_index += imgseq.frames.size();
				for(const auto & frame : imgseq.frames)
				{
					endian_append(data, frame.group, 2, config.b_big_endian);
					endian_append(data, frame.image, 2, config.b_big_endian);
					data.push_back(frame.time);
					data.push_back(frame.x);
					data.push_back(frame.y);
					data.push_back(0);
					std::cout << std::format("    FRAME: group:{} image:{}\n", frame.group, frame.image );
				}

				return true;
			});
		endian_insert(data,std::uint32_t(data.size()-(chunk_offset+8)),chunk_offset+4,4,config.b_big_endian);

		// ----- Encode the ISEQs using the fram indices that we saved earlier -----
		std::cout << "Encoding Chunk ISEQ\n";
		frame_index = 0;

		chunk_offset = data.size();
		fourcc_append("ISEQ",data);
		fourcc_append("size",data);
		data.reserve(data.size() + (assets.image_sequence_count() * 8));

		assets.enumerate_image_sequences([&]([[maybe_unused]] uint32_t id, const gap::assets::ImageSequence & imgseq)->bool
			{
				data.push_back((uint8_t)imgseq.mode);
				data.push_back(0); // reserved
				endian_append(data, imgseq.frames.size(), 2, config.b_big_endian);
				endian_append(data, frames[frame_index++], 4, config.b_big_endian);
				return true;
			});
		endian_insert(data,std::uint32_t(data.size()-(chunk_offset+8)),chunk_offset+4,4,config.b_big_endian);
	}	
		/*
		assets.enumerate_image_sequences([&](uint32_t id, const gap::assets::ImageSequence & imgseq)->bool
			{
				std::cout << std::format("  ISEQ: {:3} : {} COUNT: {}\n", id, imgseq.name, imgseq.frames.size());

				chunk_offset = data.size();
				data.reserve(chunk_offset + 4 + (8 * imgseq.frames.size()));

				fourcc_append("ISEQ",data);
				fourcc_append("size",data);

				data.push_back((uint8_t)imgseq.mode);
				data.push_back(0); // reserved
				endian_append(data, imgseq.frames.size(), 2, config.b_big_endian);

				for(const auto & frame : imgseq.frames)
				{
					endian_append(data, frame.group, 2, config.b_big_endian);
					endian_append(data, frame.image, 2, config.b_big_endian);
					data.push_back(frame.time);
					data.push_back(frame.x);
					data.push_back(frame.y);
					data.push_back(0);

					std::cout << std::format("    FRAME: group:{} image:{}\n", frame.group, frame.image );
				}

				endian_insert(data,std::uint32_t(data.size()-(chunk_offset+8)),chunk_offset+4,4,config.b_big_endian);
				return true;
			});
			*/
	

	//---------------------------------------------------------------------------
	//	Tilesets [TSET]
	//---------------------------------------------------------------------------
	if(!tilesets.empty())
	{
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

}


//=============================================================================
//
//	COLOURMAP CHUNKS
//
//=============================================================================

static
int
encode_colourmap_chunks(std::vector<std::uint8_t> & data,const gap::assets::Assets & assets,const gap::Configuration & config)
{
	if(assets.colourmap_count() == 0)
		return 0;

	//---------------------------------------------------------------------------
	//	COLR Chunk
	//---------------------------------------------------------------------------
	std::cout << "Encoding COLR Chunk...\n";

	auto chunk_offset = data.size();
	fourcc_append("COLR",data);
	fourcc_append("size",data);	

	std::vector<uint32_t> cmap_indices;
	
	assets.enumerate_colourmaps([&](const gap::assets::ColourMap & cmap)->bool
		{
			cmap_indices.push_back(data.size() - chunk_offset - 8);
			data.reserve(data.size()+(cmap.colourmap.size()*4));

			for(uint32_t colour : cmap.colourmap)
			{
				for(int i=0;i<4;++i,colour >>= 8)
					data.push_back(colour & 0x0FF);
			}
			return true;
		});
	endian_insert(data,std::uint32_t(data.size()-(chunk_offset+8)),chunk_offset+4,4,config.b_big_endian);

	//---------------------------------------------------------------------------
	//	CMAP Chunk
	//---------------------------------------------------------------------------
	std::cout << "Encoding CMAP Chunk...\n";

	chunk_offset = data.size();
	fourcc_append("CMAP",data);
	fourcc_append("size",data);	

	uint32_t index = 0;
	assets.enumerate_colourmaps([&](const gap::assets::ColourMap & cmap)->bool
		{
			endian_append(data,cmap.colourmap.size(),2,config.b_big_endian);
			endian_append(data,cmap_indices[index++],2,config.b_big_endian);
			return true;
		});
	endian_insert(data,std::uint32_t(data.size()-(chunk_offset+8)),chunk_offset+4,4,config.b_big_endian);

	/*
	assets.enumerate_colourmaps([&](const gap::assets::ColourMap & cmap)->bool
		{
			data.reserve(chunk_offset + 16 + (cmap.colourmap.size() * 4));

			for(size_t i=0;i<15;++i)	data.push_back( i<cmap.name.size() ? cmap.name[i] : 0 );
			data.push_back(0);

			endian_append(data,cmap.colourmap.size(),2,config.b_big_endian);
			endian_append(data,0,2,config.b_big_endian);

			for(uint32_t colour : cmap.colourmap)
			{
				for(int i=0;i<4;++i,colour >>= 8)
					data.push_back(colour & 0x0FF);
			}

			endian_insert(data,std::uint32_t(data.size()-(chunk_offset+8)),chunk_offset+4,4,config.b_big_endian);

			return true;
		});
*/
	return 0;
}

//=============================================================================
//
//	FILE CHUNKS
//
//=============================================================================

// struct FDIRChunkEntry
// {
// 	uint8_t 			type[4];
// 	uint8_t				datasize[4];
// 	uint8_t				name[16];
// };

static
int
encode_file_chunks(std::vector<std::uint8_t> & data,const gap::assets::Assets & assets,const gap::Configuration & config)
{
	if(assets.file_count() == 0)
		return 0;

	std::cout << "Encoding FILE Chunks...\n";

	//---------------------------------------------------------------------------
	//	FDAT Chunk
	//---------------------------------------------------------------------------

	std::vector<uint32_t>	fdat_indices;

	auto chunk_offset = data.size();
	fourcc_append("FDAT",data);
	fourcc_append("size",data);

	assets.enumerate_files([&](const gap::assets::FileInfo & fileinfo)->bool
		{
			std::println("Encoding File - '{}' as '{}' size: {}", fileinfo.source_path, fileinfo.name, fileinfo.data.size());
			fdat_indices.push_back(data.size()-chunk_offset-8);
			data.insert(end(data),begin(fileinfo.data),end(fileinfo.data));

			// ----- Align the data to 4 byte boundary -----
			{
				auto sz = (data.size() + 3) & ~3;
				if(sz > data.size())
					data.resize(sz,0);
			}
			return true;
		});

	endian_insert(data,std::uint32_t(data.size()-(chunk_offset+8)),chunk_offset+4,4,config.b_big_endian);

	//---------------------------------------------------------------------------
	//	FDIR Chunk
	//---------------------------------------------------------------------------
	chunk_offset = data.size();
	fourcc_append("FDIR",data);
	fourcc_append("size",data);

	int fdat_index = 0;
	assets.enumerate_files([&](const gap::assets::FileInfo & fileinfo)->bool
		{
			const uint8_t 	namesize = std::min<uint8_t>(fileinfo.name.size(),15);
			const uint32_t	datasize = fileinfo.data.size();

			endian_append(data,fileinfo.type,4,config.b_big_endian);
			for(int i=0;i<4;++i) 	data.push_back((datasize >> (i*8)) & 0x0FF);
			endian_append(data, fdat_indices[fdat_index++], 4, config.b_big_endian);
			for(size_t i=0;i<16;++i)	data.push_back( i<namesize ? fileinfo.name[i] : 0 );

			return true;
		});

	endian_insert(data,std::uint32_t(data.size()-(chunk_offset+8)),chunk_offset+4,4,config.b_big_endian);

	/*
	assets.enumerate_files([&](const gap::assets::FileInfo & fileinfo)->bool
		{
			std::cout << "  Encoding File - '" << fileinfo.source_path << "' as '" << fileinfo.name
								<< "' size: " << fileinfo.data.size()
								<< '\n';

			auto chunk_offset = data.size();
			fourcc_append("FILE",data);
			fourcc_append("size",data);

			const uint8_t 	namesize 				= std::min<uint8_t>(fileinfo.name.size(),15);
			const uint32_t	datasize				= fileinfo.data.size();
			const uint32_t 	total_datasize 	= ((fileinfo.data.size()+3) & ~0x03);

			data.reserve(chunk_offset + sizeof(FILEChunkEntry) + total_datasize );

//			for(int i=3;i>=0;--i) data.push_back((fileinfo.type >> (i*8)) & 0x0FF);
			endian_append(data,fileinfo.type,4,config.b_big_endian);
			for(int i=0;i<4;++i) 	data.push_back((datasize >> (i*8)) & 0x0FF);
			for(size_t i=0;i<sizeof(FILEChunkEntry::name);++i)	data.push_back( i<namesize ? fileinfo.name[i] : 0 );

			// Filedata
			data.insert(end(data),begin(fileinfo.data),end(fileinfo.data));
			{
				auto sz = (data.size() + 3) & ~3;
				if(sz > data.size())
					data.resize(sz);
			}

			endian_insert(data,std::uint32_t(data.size()-(chunk_offset+8)),chunk_offset+4,4,config.b_big_endian);

			return false;
		});
*/
	return 0;
}

//=============================================================================
//
//	TILEMAP CHUNKS
//
//=============================================================================
/*
TMAP Chunk - Sparse Tile Map
----------------------------

          -------------------->
        0        1        2        3
    +--------+--------+--------+--------+
$00 |   T    |   M    |   A    |   P    |    FourCC defines chunk type - 4 Bytes
    +--------+--------+--------+--------+
$04 |               SIZE                |    Chunk Size - 4 Bytes
    +===================================+
$08 |            TILEMAP ID             |
    +-----------------------------------+
$0C |      WIDTH      |     HEIGHT      |
    +-----------------+-----------------+
$10 |            INDEX COUNT            |
    +-----------------------------------+
$10 |            INDEX OFFSET           |    Offset into the TMIX chunk
    +-----------------------------------+
$14 |            BLOCK COUNT            |
    +-----------------------------------+
$14 |            BLOCK OFFSET           |    Offset into the TMBL chunk 
    +--------+--------+-----------------+
$18 | BSIZE  | TSIZE  |      ---        |
    +--------+--------+-----------------+

WIDTH   - Width of tilemap in tiles.
HEIGHT  - Width of tilemap in tiles.
BSIZE 	-	Block Size in tiles. Block volume = BSIZE * BSIZE
TSIZE   - Tile Size in bytes.

Block Size in bytes = BSIZE * BSIZE * TSIZE



TMIX Chunk - Sparse Tile Map Index Data
---------------------------------------

          -------------------->
        0        1        2        3
    +--------+--------+--------+--------+
$00 |   T    |   M    |   I    |   X    |    FourCC defines chunk type - 4 Bytes
    +--------+--------+--------+--------+
$04 |               SIZE                |    Chunk Size - 4 Bytes
    +===================================+
$08 | INDICES ....                      |    Indices for all of the tilemaps
    :                 :                 :    Each tilemaps indices are rounded up to multiple of 4 bytes.
    :                 :                 :    The TMAP chunk contains an index into this data.
    +-----------------------------------+


TMBL Chunk - Sparse Tile Map Block Data
---------------------------------------

          -------------------->
        0        1        2        3
    +--------+--------+--------+--------+
$00 |   T    |   M    |   B    |   L    |    FourCC defines chunk type - 4 Bytes
    +--------+--------+--------+--------+
$04 |               SIZE                |    Chunk Size - 4 Bytes
    +===================================+
$08 | BLOCKS ....                       |    Block data for all tilemaps.
    :                 :                 :    Each tilemaps block data is rounded up to multiple of 4 bytes.
    :                 :                 :    The TMAP chunk contains an index into this data.
    +-----------------------------------+

*/

static
int
encode_tilemap_chunks(std::vector<std::uint8_t> & data,const gap::assets::Assets & assets,const gap::Configuration & config)
{
	if(assets.tilemap_count() == 0)
		return 0;

	std::cout << "Encoding TileMap Chunks...\n";

	std::vector<uint32_t>	index_offsets;
	std::vector<uint32_t>	block_offsets;

	//---------------------------------------------------------------------------
	// Encode TMIX TileMap Index Chunk
	//---------------------------------------------------------------------------

	auto chunk_offset = data.size();
	fourcc_append("TMIX",data);
	fourcc_append("size",data);

	assets.enumerate_tilemaps([&](const gap::tilemap::TileMap & tilemap)->bool
		{
			auto indices = tilemap.indices();
			data.reserve(data.size() + (indices.size() * 2));
			index_offsets.push_back(data.size() - chunk_offset - 8);

			// ----- Write index data -----
			for(uint16_t index : indices)
				endian_append(data,index,2,config.b_big_endian);

			// ----- Align to 4-byte boundary -----
//			auto sz = (data.size() + 3) & ~3;
//			if(sz > data.size())
//				data.resize(sz);

			return true;
		});
	endian_insert(data,std::uint32_t(data.size()-(chunk_offset+8)),chunk_offset+4,4,config.b_big_endian);

	//---------------------------------------------------------------------------
	// Encode TMBL TileMap Block Chunk
	//---------------------------------------------------------------------------

	chunk_offset = data.size();
	fourcc_append("TMBL",data);
	fourcc_append("size",data);

	assets.enumerate_tilemaps([&](const gap::tilemap::TileMap & tilemap)->bool
		{
			block_offsets.push_back(data.size() - chunk_offset - 8);

			// ----- Get the block data and reserve space for it -----
			const auto block_data = tilemap.block_data();
			auto tilesize = tilemap.tile_size();
			data.reserve(block_data.size() * tilesize);

			// ----- Write the block data -----
			for(uint64_t value : block_data)
				endian_append(data,value,tilesize,config.b_big_endian);

			// ----- Align to 4-byte boundary -----
			auto sz = (data.size() + 3) & ~3;
			if(sz > data.size())
				data.resize(sz);

			return true;
		});
	endian_insert(data,std::uint32_t(data.size()-(chunk_offset+8)),chunk_offset+4,4,config.b_big_endian);

	//---------------------------------------------------------------------------
	// Encode TMAP TileMap Chunk
	//---------------------------------------------------------------------------

	chunk_offset = data.size();
	fourcc_append("TMAP",data);
	fourcc_append("size",data);

	int index = 0;
	assets.enumerate_tilemaps([&](const gap::tilemap::TileMap & tilemap)->bool
		{
			auto tilesize = tilemap.tile_size();
			auto indices 	= tilemap.indices();

			endian_append(data,tilemap.id(),4,config.b_big_endian);

			// ----- TileMap Dimensions -----
			endian_append(data,tilemap.width(),2,config.b_big_endian);
			endian_append(data,tilemap.height(),2,config.b_big_endian);

			// ----- Index Info -----
			endian_append(data,indices.size(),4,config.b_big_endian);
			endian_append(data,index_offsets[index],4,config.b_big_endian);

			// ----- Block Info -----
			endian_append(data,tilemap.active_block_count(),4,config.b_big_endian);
			endian_append(data,block_offsets[index],4,config.b_big_endian);

			// ----- Block and Tile sizes
			endian_append(data,tilemap.block_size(),1,config.b_big_endian);
			endian_append(data,tilesize,1,config.b_big_endian);
			endian_append(data,0U,2,config.b_big_endian);

			++index;
			return true;
		});
	endian_insert(data,std::uint32_t(data.size()-(chunk_offset+8)),chunk_offset+4,4,config.b_big_endian);


	/*
	assets.enumerate_tilemaps([&](const gap::tilemap::TileMap & tilemap)->bool
		{
			auto indices = tilemap.indices();
			auto chunk_offset = data.size();
			fourcc_append("TMAP",data);
			fourcc_append("size",data);

			auto tilesize = tilemap.tile_size();
			endian_append(data,tilemap.id(),4,config.b_big_endian);
			endian_append(data,tilemap.width(),2,config.b_big_endian);
			endian_append(data,tilemap.height(),2,config.b_big_endian);
			endian_append(data,indices.size(),4,config.b_big_endian);
			endian_append(data,tilemap.active_block_count(),4,config.b_big_endian);
			endian_append(data,tilemap.block_size(),1,config.b_big_endian);
			endian_append(data,tilesize,1,config.b_big_endian);
			endian_append(data,0U,6,config.b_big_endian);

			// INDICES
			for(uint16_t index : indices)
				endian_append(data,index,2,config.b_big_endian);

			// Align to 4-byte boundary
			auto sz = (data.size() + 3) & ~3;
			if(sz > data.size())
				data.resize(sz);

			// BLOCKS
			const auto block_data = tilemap.block_data();
			for(uint64_t value : block_data)
				endian_append(data,value,tilesize,config.b_big_endian);

			// Align to 4-byte boundary
			sz = (data.size() + 3) & ~3;
			if(sz > data.size())
				data.resize(sz);

			endian_insert(data,std::uint32_t(data.size()-(chunk_offset+8)),chunk_offset+4,4,config.b_big_endian);

			tilemap.print();
			return true;
		});
*/
	return 0;
}


std::vector<std::uint8_t>
encode_gbin(std::string_view name, const gap::assets::Assets & assets,const gap::Configuration & config)
{

	std::vector<std::uint8_t> data;
	int errors = 0;

	encode_header(data,name,assets,config);
	//encode_image_chunks(data,assets,config);
	encode_packed_image_chunks(data,assets,config);
	errors += encode_tilemap_chunks(data,assets,config);
	errors += encode_colourmap_chunks(data,assets,config);
	errors += encode_file_chunks(data,assets,config);

	//---------------------------------------------------------------------------
	//	End [ENDC]
	//---------------------------------------------------------------------------
	fourcc_append("ENDC",data);
	endian_append(data,0,4,config.b_big_endian);

//	std::uint32_t crc32 = 0; // TODO: Calculate the crc from all of the chunks.

	return data;
}

} // namespace gap

