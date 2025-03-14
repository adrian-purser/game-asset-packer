//=============================================================================
//	FILE:					maths.h
//	SYSTEM:				
//	DESCRIPTION:
//-----------------------------------------------------------------------------
//  COPYRIGHT:		(C) Copyright 2022 Adrian Purser. All Rights Reserved.
//	LICENCE:			MIT - See LICENSE file for details
//	MAINTAINER:		Adrian Purser <ade@adrianpurser.co.uk>
//	CREATED:			08-MAR-2020 Adrian Purser <ade@adrianpurser.co.uk>
//=============================================================================
#ifndef GUARD_ADE_GAME_ASSET_PACKER_MATHS_H
#define GUARD_ADE_GAME_ASSET_PACKER_MATHS_H

#include <cstdint>

namespace gap::maths
{

consteval bool 	is_powerof2(std::size_t v) 											{return v && ((v & (v - 1)) == 0); }
constexpr bool	is_multiple_of(std::size_t v, std::size_t d)		{return (v%d)==0;}

} // namespace gap::maths

#endif // ! defined GUARD_ADE_GAME_ASSET_PACKER_MATHS_H
