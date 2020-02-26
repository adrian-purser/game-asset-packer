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

namespace gap::assets
{

class Assets
{
private:
	struct ImageGroup
	{
		std::vector<gap::image::Image>		images;
		int																current_index = 0;
		uint16_t													base = 0;
	};

	static const int 																				m_max_image_groups = 16;
	std::vector<std::unique_ptr<gap::image::SourceImage>>		m_source_images;
	std::array<ImageGroup,m_max_image_groups>								m_image_groups;
	std::vector<gap::tileset::TileSet>											m_tilesets;

public:
	Assets() = default;
	Assets(const Assets &) = delete;
	Assets & operator=(const Assets &) = delete;

	int										add_source_image(std::unique_ptr<gap::image::SourceImage> p_image);
	int										add_image(int group,gap::image::Image & image);

	void									add_tileset(const gap::tileset::TileSet & tileset)	{m_tilesets.push_back(tileset);}
	void									add_tile(int tileset, const gap::tileset::Tile & tile);
	uint32_t 							tileset_width(int id)		{	if(auto p_tileset = get_tileset(id))	return p_tileset->tile_width; return 0; }
	uint32_t 							tileset_height(int id)	{	if(auto p_tileset = get_tileset(id))	return p_tileset->tile_width; return 0; }


	int										source_image_count() const noexcept			{return m_source_images.size();}
	void									enumerate_source_images(std::function<bool (int image_index,const gap::image::SourceImage &)> callback) const;
	void									enumerate_images(std::function<bool (int group,int image_index,const gap::image::Image &)> callback) const;
	void									enumerate_image_groups(std::function<bool(uint32_t group_number,uint16_t base)> callback) const;
	void 									enumerate_group_images(int group_number,std::function<bool(int image_index,const gap::image::Image & image)> callback) const;
	void									enumerate_tilesets(std::function<bool(const gap::tileset::TileSet & tileset)> callback) const;

	void 									set_group_base(int group,uint16_t base)		{if((group >= 0) && (group < m_max_image_groups)) m_image_groups[group].base = base;}

	void									dump();

	uint32_t 							get_target_image_offset(int index, int x,int y) const;
	uint32_t 							get_target_line_stride(int index) const;
	uint8_t 							get_target_pixelformat(int index) const;

	std::vector<uint8_t>											get_target_subimage(int index, int x, int y, int width, int height, uint8_t pixel_format, bool big_endian) const;
	std::unique_ptr<gap::image::SourceImage>	get_source_subimage(int index, int x, int y, int width, int height) const;

private:
	gap::tileset::TileSet * 	get_tileset(int id);

};


} // namespace gap::assets

#endif // ! defined GUARD_ADE_GAMES_ASSET_PACKER_ASSETS_H