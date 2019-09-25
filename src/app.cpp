//=============================================================================
//	FILE:					app.cpp
//	SYSTEM:				Game Asset Packer
//	DESCRIPTION:
//-----------------------------------------------------------------------------
//  COPYRIGHT:		(C)Copyright 2019 Adrian Purser. All Rights Reserved.
//	LICENCE:
//	MAINTAINER:		AJP - Adrian Purser <ade&arcadestuff.com>
//	CREATED:			24-SEP-2019 Adrian Purser <ade&arcadestuff.com>
//=============================================================================

#include <iostream>
#include "app.h"

namespace gap
{

Application::Application(const gap::Configuration & config)
	: m_config(config)
{
}


int
Application::run()
{

	// Mount Packages
	for(const auto & mp : m_config.mount_points)
		m_filesystem.mount(mp.path,mp.mountpoint);

	return 0;
}



} // namespace gap

