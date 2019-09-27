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

namespace gap::assets
{


int
Assets::add_source_image(gap::image::SourceImage && image)
{
	int index = m_source_images.size();
	m_source_images.push_back(std::move(image));
	return index;
}

void
Assets::dump()
{

	std::cout << 	"ASSETS:\n"
								"================================================================================\n";

	std::cout << "Image Sources:\n";
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
}


} // namespace gap::assets

