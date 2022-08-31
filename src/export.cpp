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
#include "encode_definitions.h"
#include "encode_gbin.h"
#include "export.h"
#include <utility/hexdump.h>
#include <filesystem>
#include <fmt/format.h>


namespace gap::exporter
{

int		
export_assets(gap::assets::Assets & assets,const gap::exporter::ExportInfo & exportinfo,const gap::Configuration & config)
{

	std::vector<std::uint8_t>	blob;
	
	switch(exportinfo.type)
	{
		case gap::exporter::TYPE_GBIN :						blob = gap::encode_gbin(exportinfo.name,assets,config); break;
		case gap::exporter::TYPE_DEFINITIONS :		blob = gap::encode_definitions(exportinfo,assets,config); break;
		default :
			std::cerr << "Unknown or unsupported export type! (" << exportinfo.filename << ')' << std::endl;
			return -1;
	} 

	std::cout << "=============================================================================\n\n"; 
	// std::cout << ade::hexdump(blob.data(),blob.size());


	switch(exportinfo.format)
	{
		case gap::exporter::FORMAT_BINARY :
			{
				std::ofstream outfile(config.output_prefix + exportinfo.filename,std::ios_base::binary | std::ios_base::out);
				if(outfile.fail())
				{
					std::cerr << "Failed to create output file " << exportinfo.filename << std::endl;
					return -1;
				}
				outfile.write((const char *)blob.data(),blob.size());
			}
			break;

		case gap::exporter::FORMAT_C_ARRAY :
			{
				std::ofstream outfile(config.output_prefix + exportinfo.filename,std::ios_base::binary | std::ios_base::out);
				if(outfile.fail())
				{
					std::cerr << "Failed to create output file " << (config.output_prefix + exportinfo.filename) << std::endl;
					return -1;
				}

				std::string name(exportinfo.name);
				std::transform(begin(name),end(name),begin(name),::toupper);
				std::string guard_name = fmt::format("GUARD_GAP_AUTOGEN_{}_H",name);

				outfile << "//=============================================================================\n";
				outfile << "//\tFILE:\t\t" << std::filesystem::path(exportinfo.filename).filename().string() << "\n";
				outfile << "//\n//\tAutogenerated by the GAP program.\n";
				outfile << "//=============================================================================\n\n";
				outfile << fmt::format("#ifndef {}\n#define {}\n\n",guard_name,guard_name);

				outfile << fmt::format("static uint8_t sg_gbin_{}[] =\n{{\n",exportinfo.name);

				for(size_t i=0,size=blob.size(); i<size; ++i)
				{
					if( (i&0x0F) == 0)
						outfile << "\n\t";
					outfile << fmt::format("0x{:02X},",(unsigned int)blob[i]);
				}

				outfile << "\n\t0";
				outfile << "\n};\n";
				outfile << fmt::format("#endif // ! defined {}\n",guard_name);
			}
			break;

		case gap::exporter::FORMAT_C_HEADER :
			{
				std::ofstream outfile(config.output_prefix + exportinfo.filename,std::ios_base::binary | std::ios_base::out);
				if(outfile.fail())
				{
					std::cerr << "Failed to create output file " << exportinfo.filename << std::endl;
					return -1;
				}
				outfile << "//=============================================================================\n";
				outfile << "//\tFILE:\t\t" << std::filesystem::path(exportinfo.filename).filename().string() << "\n";
				outfile << "//\n//\tAutogenerated by the GAP program.\n";
				outfile << "//=============================================================================\n\n";

//	FILE:					encode_definitions.cpp

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

