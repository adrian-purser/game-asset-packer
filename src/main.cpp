//=============================================================================
//	FILE:					main.cpp
//	SYSTEM:				Game Asset Packer
//	DESCRIPTION:
//-----------------------------------------------------------------------------
//  COPYRIGHT:		(C)Copyright 2019 Adrian Purser. All Rights Reserved.
//	LICENCE:
//	MAINTAINER:		AJP - Adrian Purser <ade&arcadestuff.com>
//	CREATED:			24-SEP-2019 Adrian Purser <ade&arcadestuff.com>
//=============================================================================
#include <iostream>

#include "configuration.h"
#include "build.h"
#include "tests/tests.h"

int
main(int argc,char ** argv)
{
	gap::Configuration config;

	auto status = gap::parse_command_line(argc,argv,config);
	if(status)
		return status;

	gap::FileSystem	filesystem;

	// Mount Packages. If there are no mount points then mount the current directory.
	if(config.mount_points.empty())
		filesystem.mount(".","/");
	else
	{
		for(const auto & mp : config.mount_points)
			filesystem.mount(mp.path,mp.mountpoint);
	}
	
	if(!config.test_mode.empty())
		return run_test(config, filesystem);

	return gap::build(config, filesystem);
}

