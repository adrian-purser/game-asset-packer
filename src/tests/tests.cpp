//=============================================================================
//	FILE:					tests.cpp
//	SYSTEM:				Game Asset Packer
//	DESCRIPTION:
//-----------------------------------------------------------------------------
//  COPYRIGHT:		(C)Copyright 2025 Adrian Purser. All Rights Reserved.
//	LICENCE:
//	MAINTAINER:		AJP - Adrian Purser <ade&arcadestuff.com>
//	CREATED:			13-MAR-2025 Adrian Purser <ade&arcadestuff.com>
//=============================================================================
#include "tests.h"
#include "test_tilemap.h"

int	
run_test(const gap::Configuration & config, gap::FileSystem & filesystem)
{
	if(config.test_mode == "tilemap")		return test_tilemap(config, filesystem);
	else return -1;
	return 0;
}

