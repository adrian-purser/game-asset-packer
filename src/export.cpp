//=============================================================================
//	FILE:					export.cpp
//	SYSTEM:				Game Asset Packer
//	DESCRIPTION:
//-----------------------------------------------------------------------------
//  COPYRIGHT:		(C)Copyright 2019 Adrian Purser. All Rights Reserved.
//	LICENCE:
//	MAINTAINER:		AJP - Adrian Purser <ade&arcadestuff.com>
//	CREATED:			30-SEP-2019 Adrian Purser <ade&arcadestuff.com>
//=============================================================================
#include <iostream>
#include "encode_gbin.h"
#include "export.h"
#include <utility/hexdump.h>

namespace gap
{

int		
export_assets(const gap::assets::Assets & assets,const gap::Configuration & config)
{

	// TODO: Select encoder to match output format (when more outputs are available)
	std::vector<std::uint8_t>	blob = gap::encode_gbin(assets,config);

	// TODO: Select output type (eg, binary, C Source etc)

	std::cout << "=============================================================================\n\n"; 
	std::cout << ade::hexdump(blob.data(),blob.size());


	std::ofstream outfile(config.output_file,std::ios_base::binary | std::ios_base::out);
	if(outfile.fail())
	{
		std::cerr << "Failed to create output file " << config.output_file << std::endl;
		return -1;
	}

	outfile.write((const char *)blob.data(),blob.size());
	
	return 0;
}

} // namespace gap

