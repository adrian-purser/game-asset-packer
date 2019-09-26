//=============================================================================
//	FILE:						parse_gap.h
//	SYSTEM:				 	game asset packer
//	DESCRIPTION:
//-----------------------------------------------------------------------------
//	COPYRIGHT:			(C)Copyright 2019 Adrian Purser. All Rights Reserved.
//	LICENCE:				MIT
//	MAINTAINER:			AJP - Adrian Purser <ade@arcadestuff.com>
//	CREATED:				25-SEP-2019 Adrian Purser <ade@arcadestuff.com>
//=============================================================================
#ifndef GUARD_ADE_GAME_ASSET_PACKER_PARSE_GAP_H
#define GUARD_ADE_GAME_ASSET_PACKER_PARSE_GAP_H

#include <cstdint>
#include <vector>
#include <string>
#include <memory>
#include "assets.h"

namespace gap
{

std::unique_ptr<gap::assets::Assets>		parse_gap(std::string_view source);

} // namespace gap

#endif // ! defined GUARD_ADE_GAME_ASSET_PACKER_PARSE_GAP_H
