//=============================================================================
//	FILE:						configuration.cpp
//	SYSTEM:				 	game asset packer
//	DESCRIPTION:
//-----------------------------------------------------------------------------
//	COPYRIGHT:			(C)Copyright 2019 Adrian Purser. All Rights Reserved.
//	LICENCE:				MIT
//	MAINTAINER:			AJP - Adrian Purser <ade@arcadestuff.com>
//	CREATED:				24-SEP-2019 Adrian Purser <ade@arcadestuff.com>
//=============================================================================

#include "config.h"
#include "programoptions/program_options.h"
#include "configuration.h"

int
parse_command_line(int argc,char ** argv,Configuration & out_config)
{
	//-------------------------------------------------------------------------
	//  Setup the command line parser.
	//-------------------------------------------------------------------------
	program_options::OptionGroup grp_general;
	grp_general.add_option("help","Display help message");
	grp_general.add_option("version","Display version information");
	grp_general.add_option("output,o","Output filename","Filename");

	program_options::Parser parser;
	parser.add_group("",grp_general);

	//-------------------------------------------------------------------------
	//  Parse the command line.
	//-------------------------------------------------------------------------
	program_options::ParserResults values;
	parser.parse(argc,(const char **)argv,values);

	if(values.options.count("help"))
	{
		std::cout << parser << std::endl;
		return 1;
	}
	else if(values.options.count("version"))
	{
		std::clog << PACKAGE_STRING << std::endl;
		return 1;
	}

	//-------------------------------------------------------------------------
	//	Process the options.
	//-------------------------------------------------------------------------
	if(values.options.count("output"))	out_config.output_file = values.options["output"].back();

	//-------------------------------------------------------------------------
	//	Process the args.
	//-------------------------------------------------------------------------
	if(values.args.size() > 0)
		out_config.input_file = values.args.front();

	return 0;
}
