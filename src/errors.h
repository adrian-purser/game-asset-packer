//=============================================================================
//	FILE:					errors.h
//	SYSTEM:
//	DESCRIPTION:	
//-----------------------------------------------------------------------------
//  COPYRIGHT:		(C) Copyright 2025 Adrian Purser. All Rights Reserved.
//	LICENCE:			MIT - See LICENSE file for details
//	MAINTAINER:		Adrian Purser <ade@adrianpurser.co.uk>
//	CREATED:			07-FEB-2025 Adrian Purser <ade@adrianpurser.co.uk>
//=============================================================================
#ifndef GUARD_ADE_GAME_ASSET_PACKER_ERRORS_H
#define GUARD_ADE_GAME_ASSET_PACKER_ERRORS_H

#include <string_view>
#include <system_error>

namespace gap
{

enum class error
{
	none = 0,

	failed,
	invalid_operation,
	invalid_argument
};

struct GAPErrorCategory : std::error_category
{
	const char * 	name() const noexcept override;
	std::string 	message(int ev) const override;
};


std::error_code make_error_code(gap::error);

} // namspace gap

namespace std { template <> struct is_error_code_enum<gap::error> : public true_type {}; }

#endif // ! defined GUARD_ADE_GAME_ASSET_PACKER_ERRORS_H
