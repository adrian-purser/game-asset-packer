//=============================================================================
//	FILE:					test_tilemap.cpp
//	SYSTEM:				Game Asset Packer
//	DESCRIPTION:
//-----------------------------------------------------------------------------
//  COPYRIGHT:		(C)Copyright 2025 Adrian Purser. All Rights Reserved.
//	LICENCE:
//	MAINTAINER:		AJP - Adrian Purser <ade&arcadestuff.com>
//	CREATED:			13-MAR-2025 Adrian Purser <ade&arcadestuff.com>
//=============================================================================
#include "test_tilemap.h"
#include "tilemap.h"

int	
test_tilemap(const gap::Configuration & config, gap::FileSystem & filesystem)
{
	if(config.args.size() < 2)
		return -1;

	auto p_tilemap = gap::tilemap::load(config.args[0], config.args[1], filesystem);


	return 0;
}

