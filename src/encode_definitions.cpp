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

static
std::string
generate_header_guard_name(const gap::exporter::ExportInfo & exportinfo)
{
	std::string name {"GUARD_"};

	for(auto ch : exportinfo.name)
		name.push_back(isalnum(ch) ? toupper(ch) : '_');

	name.push_back('_');

	for(auto ch : exportinfo.filename)
		name.push_back(isalnum(ch) ? toupper(ch) : '_');

	return name;
}

std::vector<std::uint8_t>		
encode_definitions(const gap::exporter::ExportInfo & exportinfo, const gap::assets::Assets & assets,const gap::Configuration & config)
{
	(void)config;
	
	if(exportinfo.format != gap::exporter::FORMAT_C_HEADER)
		return {};

	std::string	definitions;

	//---------------------------------------------------------------------------
	// Add Header Guard
	//---------------------------------------------------------------------------
	auto header_guard = generate_header_guard_name(exportinfo);

	definitions.append("#ifndef ");
	definitions.append(header_guard);
	definitions.append("\n");

	definitions.append("#define ");
	definitions.append(header_guard);
	definitions.append("\n\n");

	//---------------------------------------------------------------------------
	//	Main Namespace
	//---------------------------------------------------------------------------

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
			(void)base;
			(void)size;

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
			(void)base;
			(void)size;

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

	//---------------------------------------------------------------------------
	//	TileSets
	//---------------------------------------------------------------------------
	definitions.append("namespace tileset\n{\n\nenum\n{\n");
	assets.enumerate_tilesets([&](const gap::tileset::TileSet & tileset)->bool
		{
			std::string nam = tileset.name;
			std::transform(begin(nam),end(nam),begin(nam),::toupper);
			std::replace(begin(nam),end(nam),'-','_');
			definitions.append(fmt::format("\t{:24} = {},\n",nam,tileset.id));
			return true;
		});
	definitions.append("};\n\n} // namespace tileset\n\n");

	definitions.append("} // namespace game\n\n");

	//---------------------------------------------------------------------------
	// Add Header Guard endif
	//---------------------------------------------------------------------------
	definitions.append("#endif // ! defined ");
	definitions.append(header_guard);
	definitions.append("\n");



	std::vector<std::uint8_t> def;
	def.assign(begin(definitions),end(definitions));
	return def;
}


} // namespace gap

