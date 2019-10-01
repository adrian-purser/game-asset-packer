//=============================================================================
//	FILE:					export.h
//	SYSTEM:				Game Asset Packer
//	DESCRIPTION:
//-----------------------------------------------------------------------------
//  COPYRIGHT:		(C)Copyright 2019 Adrian Purser. All Rights Reserved.
//	LICENCE:
//	MAINTAINER:		AJP - Adrian Purser <ade&arcadestuff.com>
//	CREATED:			30-SEP-2019 Adrian Purser <ade&arcadestuff.com>
//=============================================================================
#ifndef GUARD_ADE_GAMES_ASSET_PACKER_EXPORT_H
#define GUARD_ADE_GAMES_ASSET_PACKER_EXPORT_H

#include <vector>
#include <cstdint>
#include "assets.h"
#include "configuration.h"

namespace gap
{

int		export_assets(const gap::assets::Assets & assets,const gap::Configuration & config);

} // namespace gap

#endif // ! defined GUARD_ADE_GAMES_ASSET_PACKER_EXPORT_H
