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
Assets::add_source_image(gap::image::SourceImage && image)
{
	int index = m_source_images.size();
	m_source_images.push_back(std::move(image));
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

	for(const auto & image : m_source_images)
	{
		auto str = std::to_string(index) + ':';
		str.resize(5,' ');
		str += std::to_string(image.width) + 'x' + std::to_string(image.height);
		str.resize(15,' ');
		str += gap::image::get_pixelformat_name(image.source_pixelformat);
		str.resize(24,' ');
		str += "-> ";
		str += gap::image::get_pixelformat_name(image.target_pixelformat);

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
Assets::enumerate_source_images(std::function<bool (const gap::image::SourceImage &)> callback)
{
	for(const auto & image : m_source_images)
		if(!callback(image))
			break;
}


} // namespace gap::assets

