//=============================================================================
//	FILE:					assets.h
//	SYSTEM:				Game Asset Packer
//	DESCRIPTION:
//-----------------------------------------------------------------------------
//  COPYRIGHT:		(C)Copyright 2019 Adrian Purser. All Rights Reserved.
//	LICENCE:
//	MAINTAINER:		AJP - Adrian Purser <ade&arcadestuff.com>
//	CREATED:			25-SEP-2019 Adrian Purser <ade&arcadestuff.com>
//=============================================================================
#ifndef GUARD_ADE_GAMES_ASSET_PACKER_ASSETS_H
#define GUARD_ADE_GAMES_ASSET_PACKER_ASSETS_H

#include <vector>
#include "image.h"
#include "tileset.h"
#include "asset_sound.h"

namespace gap::assets
{

struct FileInfo
{
	std::string									source_path;
	std::string 								name;
	std::vector<std::uint8_t>		data;
	int													compression 	= 0;
	uint32_t										type 					= 0;						// Allow file type override.
};

struct ColourMap
{
	std::string							source;
	std::string							name;
	std::vector<uint32_t>		colourmap;

	bool		empty() const noexcept {return colourmap.empty();}

};

class Assets
{
private:
	struct ImageGroup
	{
		std::string												name;
		std::vector<gap::image::Image>		images;
	//	int																current_index = 0;
		uint16_t													base = 0;
	};

	static const int 																				m_max_image_groups = 16;
	std::vector<std::unique_ptr<gap::image::SourceImage>>		m_source_images;
	std::array<ImageGroup,m_max_image_groups>								m_image_groups;
	std::vector<gap::tileset::TileSet>											m_tilesets;
	std::vector<FileInfo>																		m_files;
	std::vector<ColourMap>																	m_colour_maps;
	std::vector<Sound>																			m_sounds;


public:
	Assets() = default;
	Assets(const Assets &) = delete;
	Assets & operator=(const Assets &) = delete;

	int										add_source_image(std::unique_ptr<gap::image::SourceImage> p_image);
	int										add_image(int group,gap::image::Image & image);
	int										add_file(FileInfo && file);
	int										add_sound(const gap::assets::Sound & sound);
	
	void									add_tileset(const gap::tileset::TileSet & tileset)	{m_tilesets.push_back(tileset);}
	void									add_tile(int tileset, const gap::tileset::Tile & tile);
	uint32_t 							tileset_width(int id)		{	if(auto p_tileset = get_tileset(id))	return p_tileset->tile_width; return 0; }
	uint32_t 							tileset_height(int id)	{	if(auto p_tileset = get_tileset(id))	return p_tileset->tile_width; return 0; }

	int										image_group_count() const noexcept;
	int										source_image_count() const noexcept			{return m_source_images.size();}
	void									enumerate_source_images(std::function<bool (int image_index,const gap::image::SourceImage &)> callback) const;
	void									enumerate_images(std::function<bool (int group,int image_index,const gap::image::Image &)> callback) const;
	void									enumerate_image_groups(std::function<bool(const std::string & name,uint32_t group_number,uint16_t base, uint16_t size)> callback) const;
	void 									enumerate_group_images(int group_number,std::function<bool(int image_index,const gap::image::Image & image)> callback) const;
	void									enumerate_tilesets(std::function<bool(const gap::tileset::TileSet & tileset)> callback) const;
	void									enumerate_files(std::function<bool(const gap::assets::FileInfo & fileinfo)> callback) const;
	void									enumerate_colourmaps(std::function<bool(const gap::assets::ColourMap &)> callback) const;
	void									enumerate_sounds(std::function<bool(const gap::assets::Sound & sound)> callback ) const;
	void 									set_group_base(int group,uint16_t base)								{if((group >= 0) && (group < m_max_image_groups)) m_image_groups[group].base = base;}
	void 									set_group_name(int group,const std::string & name)		{if((group >= 0) && (group < m_max_image_groups)) m_image_groups[group].name = name;}

	void									dump();

	uint32_t 							get_target_image_offset(int index, int x,int y) const;
	uint32_t 							get_target_line_stride(int index) const;
	uint8_t 							get_target_pixelformat(int index) const;

	int										source_image_width(int index) const;
	int										source_image_height(int index) const;

	std::vector<uint8_t>											get_target_subimage(int index, int x, int y, int width, int height, uint8_t pixel_format, bool big_endian) const;
	std::unique_ptr<gap::image::SourceImage>	get_source_subimage(int index, int x, int y, int width, int height) const;

	int										find_colour_map(const std::string & name );
	const ColourMap *			get_colour_map(int index);
	int										add_colour_map(const ColourMap & cmap);

private:
	gap::tileset::TileSet * 	get_tileset(int id);

};


} // namespace gap::assets

#endif // ! defined GUARD_ADE_GAMES_ASSET_PACKER_ASSETS_H

