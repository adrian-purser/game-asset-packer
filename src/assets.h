//=============================================================================
//	FILE:					assets.h
//	SYSTEM:				Game Asset Packer
//	DESCRIPTION:
//-----------------------------------------------------------------------------
//  COPYRIGHT:		(C)Copyright 2019 Adrian Purser. All Rights Reserved.
//	LICENCE:
//	MAINTAINER:		AJP - Adrian Purser <ade&arcadestuff.com>
//	CREATED:			25-SEP-2019 Adrian Purser <ade&arcadestuff.com>
//=============================================================================
#ifndef GUARD_ADE_GAMES_ASSET_PACKER_ASSETS_H
#define GUARD_ADE_GAMES_ASSET_PACKER_ASSETS_H

#include <vector>
#include "image.h"

namespace gap::assets
{

class Assets
{
private:
	struct ImageGroup
	{
		std::vector<gap::image::Image>		images;
		int																current_index = 0;
	};


	static const int 															m_max_image_groups = 16;
	std::vector<gap::image::SourceImage>					m_source_images;
	std::array<ImageGroup,m_max_image_groups>			m_image_groups;

public:
	Assets() = default;
	Assets(const Assets &) = delete;
	Assets & operator=(const Assets &) = delete;

	int							add_source_image(gap::image::SourceImage && image);
	int							add_image(int group,gap::image::Image & image);

	void						dump();

};


} // namespace gap::assets

#endif // ! defined GUARD_ADE_GAMES_ASSET_PACKER_ASSETS_H