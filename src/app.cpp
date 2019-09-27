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
#include "app.h"
#include "parse_gap.h"

namespace gap
{

Application::Application(const gap::Configuration & config)
	: m_config(config)
{
}


int
Application::run()
{

	// Mount Packages
	for(const auto & mp : m_config.mount_points)
		m_filesystem.mount(mp.path,mp.mountpoint);

	std::cout << "=============================================================================\n\n"; 

	if(m_config.input_file.empty())
	{
		std::cerr << "No Input File!\n";
		return -1;
	}

	auto filedata = m_filesystem.load(m_config.input_file);
	if(filedata.empty())
	{
		std::cerr << "Failed to load the source file or it is empty!\n";
		return -1;
	}

	std::string_view source(reinterpret_cast<const char *>(filedata.data()),filedata.size());

	gap::ParserGAP parser(m_filesystem);
	auto p_assets = parser.parse(source);

//	std::cout << source << std::endl;




	return 0;
}



} // namespace gap

