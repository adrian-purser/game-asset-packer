//=============================================================================
//	FILE:					app.cpp
//	SYSTEM:				Game Asset Packer
//	DESCRIPTION:
//-----------------------------------------------------------------------------
//  COPYRIGHT:		(C)Copyright 2019 Adrian Purser. All Rights Reserved.
//	LICENCE:
//	MAINTAINER:		AJP - Adrian Purser <ade&arcadestuff.com>
//	CREATED:			24-SEP-2019 Adrian Purser <ade&arcadestuff.com>
//=============================================================================

#include <iostream>
#include "build.h"
#include "parse_gap.h"
#include "encode_gbin.h"
#include "export.h"
#include "utility/hexdump.h"

namespace gap
{


int
build(const gap::Configuration & config, gap::FileSystem & filesystem)
{
	std::cout << "=============================================================================\n\n"; 

	if(config.input_file.empty())
	{
		std::cerr << "No Input File!\n";
		return -1;
	}

	auto filedata = filesystem.load(config.input_file);
	if(filedata.empty())
	{
		std::cerr << "Failed to load the source file or it is empty!\n";
		return -1;
	}

	std::string_view source(reinterpret_cast<const char *>(filedata.data()),filedata.size());

	gap::ParserGAP parser(filesystem);
	auto p_assets = parser.parse(source);

	if(p_assets == nullptr)
		return -1;

//	std::cout << source << std::endl;
//	p_assets->dump();

	parser.enumerate_exports([&](const auto & exportinfo)->bool
		{
			std::cout << "EXPORT: " << exportinfo.filename << " type:" << exportinfo.type << " format:" << exportinfo.format << std::endl;
			export_assets(*(p_assets.get()),exportinfo,config);
			return true;
		});

	std::cout << "Finished\n";
	return 0;
}

} // namespace gap

