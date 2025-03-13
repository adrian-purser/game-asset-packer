//=============================================================================
//	FILE:						tilemap.h
//	SYSTEM:				 	game asset packer
//	DESCRIPTION:
//-----------------------------------------------------------------------------
//	COPYRIGHT:			(C)Copyright 2025 Adrian Purser. All Rights Reserved.
//	LICENCE:				MIT
//	MAINTAINER:			AJP - Adrian Purser <ade@arcadestuff.com>
//	CREATED:				12-MAR-2025 Adrian Purser <ade@arcadestuff.com>
//=============================================================================
#ifndef GUARD_ADE_GAME_ASSET_PACKER_TILEMAP_H
#define GUARD_ADE_GAME_ASSET_PACKER_TILEMAP_H

#include <cstdint>
#include <vector>
#include <string>
#include <cmath>
#include <utility>
#include "filesystem.h"
#include "utility/hash.h"

namespace gap::tilemap
{

struct SourceTileMapLayer
{
	uint32_t								m_id = 0;
	uint32_t								m_width = 0;
	uint32_t								m_height = 0;
	std::string							m_name;
	std::vector<uint32_t>		m_data;

	SourceTileMapLayer() = delete;
	SourceTileMapLayer(uint32_t id, std::string_view name, uint32_t width, uint32_t height);

	void				set_tile(std::size_t index, uint32_t value);
	void				set_tile(uint32_t x, uint32_t y, uint32_t value);
};


class SourceTileMap
{
private:
	std::vector<std::shared_ptr<SourceTileMapLayer>>	m_layers;

public:
	void		add_layer(std::shared_ptr<SourceTileMapLayer> p_layer)		{m_layers.push_back(p_layer);}
	void		enumerate_layers(std::function<bool(const SourceTileMapLayer &)> callback);

};

std::unique_ptr<SourceTileMap>		load(const std::string & filename, const std::string & type, gap::FileSystem & filesystem);

} // namespace gap::tilemap

#endif // ! defined GUARD_ADE_GAME_ASSET_PACKER_TILEMAP_H
