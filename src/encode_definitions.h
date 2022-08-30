//=============================================================================
//	FILE:					encode_definitions.h
//	SYSTEM:				Game Asset Packer
//	DESCRIPTION:
//-----------------------------------------------------------------------------
//  COPYRIGHT:		(C)Copyright 2022 Adrian Purser. All Rights Reserved.
//	LICENCE:
//	MAINTAINER:		AJP - Adrian Purser <ade&arcadestuff.com>
//	CREATED:			30-AUG-2022 Adrian Purser <ade&arcadestuff.com>
//=============================================================================
#ifndef GUARD_ADE_GAMES_ASSET_PACKER_ENCODE_DEFINITIONS_H
#define GUARD_ADE_GAMES_ASSET_PACKER_ENCODE_DEFINITIONS_H

#include <vector>
#include <cstdint>
#include "assets.h"
#include "configuration.h"
#include "export.h"

namespace gap
{

std::vector<std::uint8_t>		encode_definitions(const gap::exporter::ExportInfo & exportinfo, const gap::assets::Assets & assets,const gap::Configuration & config);

} // namespace gap

#endif // ! defined GUARD_ADE_GAMES_ASSET_PACKER_ENCODE_DEFINITIONS_H
