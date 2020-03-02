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

namespace gap::exporter
{

enum
{
	TYPE_UNKNOWN,
	TYPE_GBIN
};

enum
{
	FORMAT_UNKNOWN,
	FORMAT_BINARY,
	FORMAT_C_ARRAY,
	FORMAT_CPP_VECTOR,
	FORMAT_CPP_STDARRAY
};

struct ExportInfo
{
	std::string			filename;
	std::string			name;
	int							type;
	int							format;
};

int		export_assets(gap::assets::Assets & assets,const gap::exporter::ExportInfo & info,const gap::Configuration & config);

} // namespace gap::exporter

#endif // ! defined GUARD_ADE_GAMES_ASSET_PACKER_EXPORT_H
