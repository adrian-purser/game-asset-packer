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
#include "tilemap.h"
#include "sound_sample.h"

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

struct ImageFrame
{
	int											group	= -1;
	int											image	= -1;
	uint8_t									time 	= 1;
	int8_t									x 		= 0;
	int8_t									y 		= 0;
};


struct ImageSequence
{
	enum {MODE_ONCE, MODE_LOOP, MODE_BOUNCE};

	std::string								name;
	int												mode = MODE_LOOP;

	std::vector<ImageFrame>		frames;
};


class Assets
{
private:
	struct ImageGroup
	{
		std::string												name;
	//	int																current_index = 0;
		uint16_t													base = 0;
		std::vector<gap::image::Image>		images;

//		ImageGroup( std::string_view _name, uint16_t _base = 0) : name(_name), base(_base) {}
	};

	std::vector<std::unique_ptr<gap::image::SourceImage>>		m_source_images;
	std::vector<ImageGroup>																	m_image_groups;
	std::vector<gap::tileset::TileSet>											m_tilesets;
	std::vector<FileInfo>																		m_files;
	std::vector<ColourMap>																	m_colour_maps;
	std::vector<ImageSequence>															m_image_sequences;
	std::vector<std::unique_ptr<gap::tilemap::TileMap>>			m_tilemaps;
	std::vector<std::unique_ptr<gap::sound::SoundSample>>		m_sound_samples;
	std::string 																						m_last_error;

	int			m_most_recent_tileset = -1;

public:
	Assets() = default;
	Assets(const Assets &) = delete;
	Assets & operator=(const Assets &) = delete;

	int										add_source_image(std::unique_ptr<gap::image::SourceImage> p_image);
	int										add_image(gap::image::Image & image);
	int										add_image_sequence( std::string_view name, int mode );
	int										add_image_frame( std::string_view group, std::string_view image, int time, int x=0, int y=0, int count=1);
	int										add_image_group( std::string_view name, int base = 0 );
	int										add_file(FileInfo && file);
	void									add_tileset(const gap::tileset::TileSet & tileset)	{m_tilesets.push_back(tileset); m_most_recent_tileset=tileset.id;}
	void									add_tile(int tileset, const gap::tileset::Tile & tile);
	void									add_tilemap(std::unique_ptr<gap::tilemap::TileMap> && p_tilemap);
	void									add_sound_sample(std::unique_ptr<gap::sound::SoundSample> && p_sound_sample);

	bool 									image_group_exists(std::string_view name)	{return !(find_group(name) < 0);}

	uint32_t 							tileset_width(int id)		{	if(auto p_tileset = get_tileset(id))	return p_tileset->tile_width; return 0; }
	uint32_t 							tileset_height(int id)	{	if(auto p_tileset = get_tileset(id))	return p_tileset->tile_width; return 0; }

	int										image_group_count() const noexcept;
	int										image_sequence_count() const noexcept		{return m_image_sequences.size();}
	int										source_image_count() const noexcept			{return m_source_images.size();}
	void									enumerate_source_images(std::function<bool (int image_index,const gap::image::SourceImage &)> callback) const;
	void									enumerate_images(std::function<bool (int group,int image_index,const gap::image::Image &)> callback) const;
	void									enumerate_image_groups(std::function<bool(const std::string & name,uint32_t group_number,uint16_t base, uint16_t size)> callback) const;
	void									enumerate_image_sequences(std::function<bool(uint32_t sequence_number, const gap::assets::ImageSequence &)> callback) const;
	void 									enumerate_group_images(int group_number,std::function<bool(int image_index,const gap::image::Image & image)> callback) const;
	void									enumerate_tilesets(std::function<bool(const gap::tileset::TileSet & tileset)> callback) const;
	void									enumerate_files(std::function<bool(const gap::assets::FileInfo & fileinfo)> callback) const;
	void									enumerate_colourmaps(std::function<bool(const gap::assets::ColourMap &)> callback) const;
	void 									enumerate_tilemaps(std::function<bool(const gap::tilemap::TileMap & tilemap)> callback) const;

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

	const std::string &		get_last_error() const noexcept		{return m_last_error;}

	int										generate_tileset_id(int mimimum_value=1);

private:
	gap::tileset::TileSet * 	get_tileset(int id);
	int												current_group() const noexcept {return m_image_groups.size()-1;}
	int												find_group( std::string_view name );
	int												find_image(int group, std::string_view image_name);
	int												find_image_sequence( std::string_view name );
	int												set_error(std::string_view error_msg)		{m_last_error = error_msg; return -1;}

};


} // namespace gap::assets

#endif // ! defined GUARD_ADE_GAMES_ASSET_PACKER_ASSETS_H

