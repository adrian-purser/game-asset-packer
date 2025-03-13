//=============================================================================
//	FILE:					build.h
//	SYSTEM:				Game Asset Packer
//	DESCRIPTION:
//-----------------------------------------------------------------------------
//  COPYRIGHT:		(C)Copyright 2019 Adrian Purser. All Rights Reserved.
//	LICENCE:
//	MAINTAINER:		AJP - Adrian Purser <ade&arcadestuff.com>
//	CREATED:			24-SEP-2019 Adrian Purser <ade&arcadestuff.com>
//=============================================================================
#ifndef GUARD_ADE_GAMES_ASSET_PACKER_BUILD_H
#define GUARD_ADE_GAMES_ASSET_PACKER_BUILD_H

#include "configuration.h"
#include "filesystem.h"
#include "assets.h"

namespace gap
{

int build(const gap::Configuration & config, gap::FileSystem & filesystem);

} // namespace gap

#endif // ! defined GUARD_ADE_GAMES_ASSET_PACKER_BUILD_H