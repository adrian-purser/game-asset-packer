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

namespace gap::exporter
{

int		
export_assets(const gap::assets::Assets & assets,const gap::exporter::ExportInfo & exportinfo,const gap::Configuration & config)
{

	std::vector<std::uint8_t>	blob;
	
	switch(exportinfo.type)
	{
		case gap::exporter::TYPE_GBIN :		blob = gap::encode_gbin(assets,config); break;
		default :
			std::cerr << "Unknown or unsupported export type! (" << exportinfo.filename << ')' << std::endl;
			return -1;
	} 

	std::cout << "=============================================================================\n\n"; 
	std::cout << ade::hexdump(blob.data(),blob.size());


	switch(exportinfo.format)
	{
		case gap::exporter::FORMAT_BINARY :
			{
				std::ofstream outfile(exportinfo.filename,std::ios_base::binary | std::ios_base::out);
				if(outfile.fail())
				{
					std::cerr << "Failed to create output file " << exportinfo.filename << std::endl;
					return -1;
				}
				outfile.write((const char *)blob.data(),blob.size());
			}
			break;

		default :
			std::cerr << "Unknown or unsupported export format! (" << exportinfo.filename << ')' << std::endl;
			return -1;
	}

	
	return 0;
}

} // namespace gap::exporter

