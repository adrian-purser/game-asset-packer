//=============================================================================
//	FILE:					encode_definitions.cpp
//	SYSTEM:				Game Asset Packer
//	DESCRIPTION:
//-----------------------------------------------------------------------------
//  COPYRIGHT:		(C)Copyright 2022 Adrian Purser. All Rights Reserved.
//	LICENCE:
//	MAINTAINER:		AJP - Adrian Purser <ade&arcadestuff.com>
//	CREATED:			30-AUG-2022 Adrian Purser <ade&arcadestuff.com>
//=============================================================================
#include <map>
#include <cstdint>
#include <fmt/format.h>
#include "encode_definitions.h"

namespace gap
{

std::vector<std::uint8_t>		
encode_definitions(const gap::exporter::ExportInfo & exportinfo, const gap::assets::Assets & assets,const gap::Configuration & config)
{

	if(exportinfo.format != gap::exporter::FORMAT_C_HEADER)
		return {};

	std::string	definitions;

	definitions.append("namespace ");
	definitions.append(exportinfo.name);
	definitions.append("\n{\n\n");

	//===========================================================================
	//	IMAGE ASSETS
	//===========================================================================

	definitions.append("namespace image\n{\n\n");

	//---------------------------------------------------------------------------
	//	Image Groups
	//---------------------------------------------------------------------------
	std::map<uint32_t,std::string>	groups;

	assets.enumerate_image_groups([&](const std::string & name,uint32_t group_number,uint16_t base, uint16_t size)->bool
		{
			if(!name.empty())
				groups[group_number] = name;
			return true;
		});

	if(!groups.empty())
	{
		definitions.append("namespace group\n{\nenum\n{\n");
		for(auto [num,nam] : groups)
		{
			std::transform(begin(nam),end(nam),begin(nam),::toupper);
			definitions.append(fmt::format("\t{:24} = {},\n",nam,num));
		}
		definitions.append("};\n} // namespace group\n\n");
	}

	//---------------------------------------------------------------------------
	//	Images
	//---------------------------------------------------------------------------
	assets.enumerate_image_groups([&](const std::string & name,uint32_t group_number,uint16_t base, uint16_t size)->bool
		{
			std::map<uint32_t,std::string> images;
			std::string group_name = name.empty() ? fmt::format("GROUP{}",group_number) : name;
			std::transform(begin(group_name),end(group_name),begin(group_name),::toupper);

			assets.enumerate_group_images(group_number,[&](int image_index,const gap::image::Image & image)->bool
				{
					if(!image.name.empty())
						images[image_index] = image.name;
					return true;
				});

			if(!images.empty())
			{
				definitions.append(fmt::format("// GROUP: {}\n\n",group_name));
				definitions.append("enum\n{\n");
				for(auto [num,nam] : images)
				{
					std::transform(begin(nam),end(nam),begin(nam),::toupper);
					definitions.append(fmt::format("\t{:24} = {},\n",nam,num));
				}
				definitions.append("};\n\n");
			}
			return true;
		});


	definitions.append("} // namespace image\n\n");

	definitions.append("\n}\n");

	std::vector<std::uint8_t> def;
	def.assign(begin(definitions),end(definitions));
	return def;
}


} // namespace gap

