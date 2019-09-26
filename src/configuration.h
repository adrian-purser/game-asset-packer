//=============================================================================
//	FILE:						configuration.h
//	SYSTEM:				 	game asset packer
//	DESCRIPTION:
//-----------------------------------------------------------------------------
//	COPYRIGHT:			(C)Copyright 2019 Adrian Purser. All Rights Reserved.
//	LICENCE:				MIT
//	MAINTAINER:			AJP - Adrian Purser <ade@arcadestuff.com>
//	CREATED:				24-SEP-2019 Adrian Purser <ade@arcadestuff.com>
//=============================================================================
#ifndef GUARD_ADE_GAME_ASSET_PACKER_CONFIGURATION_H
#define GUARD_ADE_GAME_ASSET_PACKER_CONFIGURATION_H

#include <cstdint>
#include <vector>
#include <string>

namespace gap
{

struct MountPoint
{
	std::string		path;
	std::string		mountpoint;
};


struct Configuration
{
	std::string										input_file			= "assets.gap";
	std::string										output_file;		// Overrides the filename from the input_file if non-empty.

	std::vector<MountPoint>				mount_points;
};

int	parse_command_line(int argc,char ** argv,Configuration & out_config);

} // namespace gap

#endif // ! defined GUARD_ADE_GAME_ASSET_PACKER_CONFIGURATION_H