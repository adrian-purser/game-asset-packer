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
Assets::add_image(int group_index,gap::image::Image & image)
{
	if((group_index < 0) || (group_index >= m_max_image_groups))
		return -1;

	auto & group = m_image_groups[group_index];
	int index = group.current_index;

	if(group.current_index >= group.images.size())
		group.images.resize(group.current_index+1);

	group.images[group.current_index++] = image;

	return index;
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
Assets::enumerate_image_groups(std::function<bool(uint32_t group_number)> callback) const
{
	int group_index = 0;
	for(const auto & group : m_image_groups)
	{
		if(!group.images.empty())
			if(!callback(group_index))
				return;
		++group_index;
	}	
}

void
Assets::enumerate_group_images(int group_number,std::function<bool(int image_index,const gap::image::Image & image)> callback) const
{
	std::cout << "Enumerating group images: group = " << group_number << '\n';

	if((group_number>=0) && (group_number < m_image_groups.size()))
	{
		const auto & group = m_image_groups[group_number];
		int image_index = 0;

		for(const auto & image : group.images)
			if(!callback(image_index++,image))
				return;
	}
}

std::uint32_t 	
Assets::get_target_image_offset(int index, int x,int y) const
{
	if((index<0) || (index>=m_source_images.size()))
		return 0;

	return gap::image::pixelformat::image_pixel_offset(m_source_images[index]->target_pixelformat(),x,y,m_source_images[index]->width());
}

std::uint32_t 	
Assets::get_target_line_stride(int index) const
{
	if((index<0) || (index>=m_source_images.size()))
		return 0;

	return m_source_images[index]->width();
}

std::uint8_t 		
Assets::get_target_pixelformat(int index) const
{
	if((index<0) || (index>=m_source_images.size()))
		return 0;

	return m_source_images[index]->target_pixelformat();
}


std::vector<uint8_t>	
Assets::get_target_subimage(int index, int x, int y, int width, int height, uint8_t pixel_format, bool big_endian) const
{
	if((index<0) || (index>=m_source_images.size()))
	{
		std::cerr << "get_target_subimage: Unknown Image: " << index << std::endl;
 		return {};
	}
	
	return m_source_images[index]->create_sub_target_data(x, y, width, height, pixel_format, big_endian);

}




} // namespace gap::assets

