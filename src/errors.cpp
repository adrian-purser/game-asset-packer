//=============================================================================
//	FILE:					errors.cpp
//	SYSTEM:
//	DESCRIPTION:	
//-----------------------------------------------------------------------------
//  COPYRIGHT:		(C) Copyright 2025 Adrian Purser. All Rights Reserved.
//	LICENCE:			MIT - See LICENSE file for details
//	MAINTAINER:		Adrian Purser <ade@adrianpurser.co.uk>
//	CREATED:			10-MAR-2025 Adrian Purser <ade@adrianpurser.co.uk>
//=============================================================================
#include <string>
#include "errors.h"

namespace gap
{

const char *
GAPErrorCategory::name() const noexcept
{
	return "gap";
}

std::string
GAPErrorCategory::message(int ev) const
{
	switch( static_cast<gap::error>(ev) )
	{
		case	gap::error::none :								return "None";
		case	gap::error::failed :							return "Operation Failed";
		case	gap::error::invalid_operation :		return "Invalid Operation";
		case	gap::error::invalid_argument :		return "Invalid Argument";
	}
	return "(Unknown Error!)";
}


const GAPErrorCategory sg_gap_error_category;

std::error_code make_error_code(gap::error e)
{
	return std::error_code(static_cast<int>(e), sg_gap_error_category);
}

} // namespace gap


