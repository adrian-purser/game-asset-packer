//=============================================================================
//	FILE:					assets.cpp
//	SYSTEM:				Game Asset Packer
//	DESCRIPTION:
//-----------------------------------------------------------------------------
//  COPYRIGHT:		(C)Copyright 2019 Adrian Purser. All Rights Reserved.
//	LICENCE:
//	MAINTAINER:		AJP - Adrian Purser <ade&arcadestuff.com>
//	CREATED:			25-SEP-2019 Adrian Purser <ade&arcadestuff.com>
//=============================================================================
#include <iostream>
#include <utility>
#include <format>
#include "assets.h"
#include "utility/format.h"

namespace gap::assets
{


int
Assets::add_source_image(std::unique_ptr<gap::image::SourceImage> p_image)
{
	int index = m_source_images.size();
	m_source_images.push_back(std::move(p_image));
	return index;
}

int
Assets::add_image_group( std::string_view name, int base)
{
	if(image_group_exists(name))
		return -1;

	int index = m_image_groups.size();

//	m_image_groups.emplace_back(name,base);
	m_image_groups.emplace_back( ImageGroup {.name=std::string(name), .base = static_cast<uint16_t>(base), .images = {}} );

	return index; 
}

int
Assets::add_image(gap::image::Image & image)
{
	if(m_image_groups.empty())
		return -1;

	auto & group = m_image_groups.back();
	int index = group.images.size();
	group.images.push_back(image);

	return index;
}

int
Assets::add_image_sequence( std::string_view name, int mode )
{
	if(find_image_sequence( name ) >= 0)
		return -1;

	int index = m_image_sequences.size();
	ImageSequence seq;
	seq.name = name;
	seq.mode = mode;
	m_image_sequences.emplace_back(seq);
	return index;
}

int
Assets::add_image_frame( std::string_view group_name, std::string_view image_name, int time, int x, int y, int count)
{
	if(m_image_sequences.empty())
		return set_error( "Image Sequence does not exist!" );

	//---------------------------------------------------------------------------
	// Validate parameters
	//---------------------------------------------------------------------------

	if((time < 0) || (time > 255))
		return set_error( std::format("'time' value {} is out of ramge (0-255)!",time) );

	if((x < -128) || (x > 127))
		return set_error( std::format("'x' value {} is out of ramge!",x) );

	if((y < -128) || (y > 127))
		return set_error( std::format("'y' value {} is out of ramge!",y) );

	auto & imgseq = m_image_sequences.back();

	//---------------------------------------------------------------------------
	// Get group 
	//---------------------------------------------------------------------------
	int group_num = current_group();
	if(!group_name.empty()) 
		group_num = find_group(group_name);

	if(group_num < 0)
		return set_error( group_name.empty() ? "Group not available!" : std::format("Unknown group '{}'!",group_name) );

	const auto & group = m_image_groups[group_num];

	if(group.images.empty())
		return set_error( std::format("Group '{}' does not contain any images!",group.name) );

	//---------------------------------------------------------------------------
	// if count < 0 then add all images in the group 
	//---------------------------------------------------------------------------
	if(count < 0)
	{
		const int size = group.images.size();
		for(int i=0;i<size;++i)

		imgseq.frames.push_back({	.group 	= group_num,
															.image	= i,
															.time		= (uint8_t) time,
															.x			= (int8_t) x,
															.y			= (int8_t) y
														});
	}
	else
	{
		//-----------------------------------------------------------------------
		// Get image
		//-----------------------------------------------------------------------
		int image_num = -1;
		
		if(!image_name.empty())
		{
			image_num = find_image(group_num, image_name);
			if(image_num < 0)
				return set_error( std::format("Image '{}' not found in group '{}'!",image_name,group.name) );
		}

		//-----------------------------------------------------------------------
		// Add the image frame
		//-----------------------------------------------------------------------
		imgseq.frames.push_back({	.group 	= group_num,
															.image	= image_num,
															.time		= (uint8_t) time,
															.x			= (int8_t) x,
															.y			= (int8_t) y
														});
	}

	return 0;
}


int
Assets::add_file(FileInfo && file)
{
	m_files.emplace_back(std::forward<FileInfo>(file));
	return 0;
}

void
Assets::add_tile(int id, const gap::tileset::Tile & tile)
{
	if(auto p_tileset = get_tileset(id))
		p_tileset->tiles.push_back(tile);
}

gap::tileset::TileSet *
Assets::get_tileset(int id)
{
	for(auto & ts : m_tilesets)
		if(ts.id == id)
			return &ts;
	return nullptr;
}

int
Assets::find_group( std::string_view name )
{
	const int size = m_image_groups.size();

	for(int i=0; i<size; ++i)
		if(m_image_groups[i].name == name)
			return i;

	return -1;
}

int
Assets::find_image(int group_num, std::string_view image_name)
{
	if((group_num < 0) || std::cmp_greater_equal(group_num,m_image_groups.size()))
		return -1;

	const auto & group = m_image_groups[group_num];
	int size = group.images.size();

	for(int i=0;i<size;++i)
		if(group.images[i].name == image_name)
			return i;

	return -1;
}

int
Assets::find_image_sequence( std::string_view name )
{
	const int size = m_image_sequences.size();

	for(int i=0; i<size; ++i)
		if(m_image_sequences[i].name == name)
			return i;

	return -1;
}



void
Assets::dump()
{

	std::cout << 	"ASSETS:\n"
								"================================================================================\n";

	std::cout << "Image Sources:\n";
	std::cout << "-----------------------------------------------------------------------------\n";
	int index = 0;

	for(const auto & p_image : m_source_images)
	{
		auto str = std::to_string(index) + ':';
		str.resize(5,' ');
		str += std::to_string(p_image->width()) + 'x' + std::to_string(p_image->height());
		str.resize(15,' ');
		str += gap::image::get_pixelformat_name(p_image->source_pixelformat());
		str.resize(24,' ');
		str += "-> ";
		str += gap::image::get_pixelformat_name(p_image->target_pixelformat());

		std::cout << str << '\n';
		++index;
	}

	std::cout << "-----------------------------------------------------------------------------\n";
	std::cout << "Image Groups\n";
	std::cout << "-----------------------------------------------------------------------------\n";

	int group_index = 0;
	for(const auto & group : m_image_groups)
	{
		auto str = std::to_string(group_index) + ':';
		str.resize(4,' ');

		std::cout << "grp ";
		
		if(group.images.empty())
			std::cout << str << "empty\n";
		else
		{
			std::cout << str << std::to_string(group.images.size()) << " images\n";
			int img_index = 0;
			for(const auto & image : group.images)
			{
				str = std::string("  ") + std::to_string(img_index);
				str.resize(7,' ');
				if((image.width == 0) || (image.height == 0))
					std::cout << str << "empty\n";
				else
				{
					str += "($" + ade::format::hex_string((group_index << 28) | img_index,8) + ") ";
					int size = str.size();
					str += std::to_string(image.width) + 'x' + std::to_string(image.height) + " x:" + std::to_string(image.x) + " y:" + std::to_string(image.y);
					str.resize(size + 26,' ');
					str += image.name;
					std::cout << str << '\n';
				}
				++img_index;
			}
		}
		++group_index;
	}

	std::cout << "-----------------------------------------------------------------------------\n";
	std::cout << "Tile Sets\n";
	std::cout << "-----------------------------------------------------------------------------\n";	
	index = 0;
	for(const auto & tileset : m_tilesets)
	{
		auto str = std::to_string(tileset.id) + ':';
		str.resize(8,' ');
		str.append(tileset.name);
		str.resize(8+24,' ');
		str.append(std::to_string(tileset.tile_width));
		str.push_back('x');
		str.append(std::to_string(tileset.tile_height));
		str.resize(8+24+10,' ');
		str.append(std::to_string(tileset.tiles.size()));
		str.append(" tiles");
		std::cout << str << '\n';
	}

	std::cout << "\n\n";	
}

int
Assets::image_group_count() const noexcept
{
	int image_group_count = 0;
	for(const auto & group : m_image_groups)
		if(!group.images.empty())
			++image_group_count;
	return image_group_count;
}



void
Assets::enumerate_source_images(std::function<bool (int image_index,const gap::image::SourceImage &)> callback) const
{
	int index = 0;
	for(const auto & image : m_source_images)
		if(!callback(index++,*(image.get())))
			break;
}

void
Assets::enumerate_images(std::function<bool (int group,int image_index,const gap::image::Image &)> callback) const
{
	int group_index = 0;
	for(const auto & group : m_image_groups)
	{
		int image_index = 0;

		for(const auto & image : group.images)
			if(!callback(group_index,image_index++,image))
				return;

		++group_index;
	}
}

void
Assets::enumerate_image_groups(std::function<bool(const std::string & name, uint32_t group_number,uint16_t base, uint16_t size)> callback) const
{
	int group_index = 0;
	for(const auto & group : m_image_groups)
	{
		if(!group.images.empty())
			if(!callback(group.name,group_index,group.base, (uint16_t)group.images.size()))
				return;
		++group_index;
	}	
}

void
Assets::enumerate_group_images(int group_number,std::function<bool(int image_index,const gap::image::Image & image)> callback) const
{
//	std::cout << "Enumerating group images: group = " << group_number << '\n';

	if((group_number>=0) && ( std::cmp_less(group_number, m_image_groups.size())))
	{
		const auto & group = m_image_groups[group_number];
		int image_index = 0;

		for(const auto & image : group.images)
			if(!callback(image_index++,image))
				return;
	}
}

void
Assets::enumerate_image_sequences(std::function<bool(uint32_t sequence_number, const gap::assets::ImageSequence &)> callback) const
{
	const uint32_t size = m_image_sequences.size();
	for(uint32_t i=0; i<size;++i)
		if(!callback(i,m_image_sequences[i]))
			break;
}


void
Assets::enumerate_tilesets(std::function<bool(const gap::tileset::TileSet & tileset)> callback) const
{
	for(auto & tileset : m_tilesets)
		if(!callback(tileset))
			break;
}

void
Assets::enumerate_colourmaps(std::function<bool(const gap::assets::ColourMap &)> callback) const
{
	for(const auto & cmap : m_colour_maps)
		if(!callback(cmap))
			break;
}

void
Assets::enumerate_files(std::function<bool(const gap::assets::FileInfo & fileinfo)> callback) const
{
	for(auto & fileinfo : m_files)
		if(!callback(fileinfo))
			break;
}

std::uint32_t 	
Assets::get_target_image_offset(int index, int x,int y) const
{
	if((index<0) || ( std::cmp_greater_equal(index,m_source_images.size())))
		return 0;

	return gap::image::pixelformat::image_pixel_offset(m_source_images[index]->target_pixelformat(),x,y,m_source_images[index]->width());
}

std::uint32_t 	
Assets::get_target_line_stride(int index) const
{
	if((index<0) || (std::cmp_greater_equal(index,m_source_images.size())))
		return 0;

	return m_source_images[index]->width();
}

std::uint8_t 		
Assets::get_target_pixelformat(int index) const
{
	if((index<0) || ( std::cmp_greater_equal(index, m_source_images.size())))
		return 0;

	return m_source_images[index]->target_pixelformat();
}


std::vector<uint8_t>	
Assets::get_target_subimage(int index, int x, int y, int width, int height, uint8_t pixel_format, bool big_endian) const
{
	if((index<0) || ( std::cmp_greater_equal(index, m_source_images.size()) ))
	{
		std::cerr << "get_target_subimage: Unknown Image: " << index << std::endl;
 		return {};
	}
	
	return m_source_images[index]->create_sub_target_data(x, y, width, height, pixel_format, big_endian);

}

int
Assets::source_image_width(int index) const
{
	if((index<0) || ( std::cmp_greater_equal(index, m_source_images.size()) ))
		return 0;

	return m_source_images[index]->width();		
}

int
Assets::source_image_height(int index) const
{
	if((index<0) || ( std::cmp_greater_equal(index, m_source_images.size()) ))
		return 0;

	return m_source_images[index]->height();	
}



std::unique_ptr<gap::image::SourceImage>
Assets::get_source_subimage(int index, int x, int y, int width, int height) const 
{
	if((index<0) || ( std::cmp_greater_equal(index, m_source_images.size()) ))
	{
		std::cerr << "get_source_subimage: Unknown Image: " << index << std::endl;
 		return nullptr;
	}
	
	return m_source_images[index]->duplicate_subimage(x, y, width, height);
}

int
Assets::find_colour_map(const std::string & name )
{
	int index = 0;
	for(const auto & cmap : m_colour_maps)
	{
		if(cmap.name == name)
			return index;
		++index;
	}
	return -1;
}

const ColourMap *
Assets::get_colour_map(int index)
{
	return (index < 0) || std::cmp_greater_equal(index,m_colour_maps.size()) ? nullptr : &m_colour_maps[index];
}

int
Assets::add_colour_map(const ColourMap & cmap)
{
	if(find_colour_map(cmap.name) >= 0)
		return -1;

	int index = m_colour_maps.size();
	m_colour_maps.push_back(cmap);
	return index;

}


} // namespace gap::assets

