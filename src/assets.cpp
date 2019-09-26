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

#include "assets.h"

namespace gap::assets
{


int
Assets::add_source_image(gap::Image && image)
{
	int index = m_source_images.size();
	m_source_images.push_back(std::move(image));
}



} // namespace gap::assets

