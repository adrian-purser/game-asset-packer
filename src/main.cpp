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

int
main(int argc,char ** argv)
{
	Configuration config;

	auto status = parse_command_line(argc,argv,config);
	if(status)
		return status;

	std::cout << "Loading '" << config.input_file << "'\n";

	return 0;
}

