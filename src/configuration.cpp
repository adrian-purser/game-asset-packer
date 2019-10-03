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

namespace gap
{

int
parse_command_line(int argc,char ** argv,Configuration & out_config)
{
	//-------------------------------------------------------------------------
	//  Setup the command line parser.
	//-------------------------------------------------------------------------
	program_options::OptionGroup grp_general;
	grp_general.add_option("help","Display help message");
	grp_general.add_option("version","Display version information");
	grp_general.add_option("mount,m","Mount Package","<Path>,<MountPoint>");

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
	if(values.options.count("mount"))
	{
		for(const auto & str : values.options["mount"])
		{
			auto pos = str.find(',');
			if(pos == std::string::npos)
				out_config.mount_points.push_back({str,"/"});
			else
				out_config.mount_points.push_back({str.substr(0,pos),str.substr(pos+1)});
		}
	}

	//-------------------------------------------------------------------------
	//	Process the args.
	//-------------------------------------------------------------------------
	if(values.args.size() > 0)
		out_config.input_file = values.args.front();

	return 0;
}

} // namespace gap
