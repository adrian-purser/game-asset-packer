//=============================================================================
//	FILE:					tilemap.h
//	SYSTEM:				SpaceGame
//	DESCRIPTION:
//-----------------------------------------------------------------------------
//  COPYRIGHT:		(C) Copyright 2025 Adrian Purser. All Rights Reserved.
//	LICENCE:			MIT - See LICENSE file for details
//	MAINTAINER:		Adrian Purser <ade@adrianpurser.co.uk>
//	CREATED:			10-MAR-2025 Adrian Purser <ade@adrianpurser.co.uk>
//=============================================================================
#ifndef GUARD_ADE_GAME_ASSET_PACKER_TILEMAP_H
#define GUARD_ADE_GAME_ASSET_PACKER_TILEMAP_H

#include <cstdint>
#include <cassert>
#include <array>
#include <vector>
#include <span>
#include <string>
#include "errors.h"
#include "utility/maths.h"

namespace gap::tilemap
{
static constexpr uint16_t		INDEX_EMPTY 		= 0xFFFE;		// Block is empty. Setting a tile will allocate a new block.
static constexpr uint16_t		INDEX_UNLOADED 	= 0xFFFF;		// This block has been unloaded and will require loading to become active.

class TilemapBlocks
{
private:
	std::vector<uint64_t>		m_data;
	uint32_t 								m_blocksize 					= 0;
	uint32_t								m_blockcount 					= 0;
	uint32_t 								m_active_block_count	= 0;

public:
	TilemapBlocks() = delete;
	TilemapBlocks(uint32_t blocksize, uint32_t blockcount)
		: m_blocksize(blocksize)
		, m_blockcount(blockcount)
		{
			m_data.resize(blockcount * blocksize * blocksize);
		}

	uint64_t			get(uint32_t block, uint32_t x, uint32_t y) const
								{
									auto index = calc_index(block,x,y);
									return index < m_data.size() ? m_data.at(index) : 0U;
								}

	void					set(uint32_t block, uint32_t x, uint32_t y, uint64_t value)
								{
									auto index = calc_index(block,x,y);
									if(index < m_data.size())
										m_data[index] = value;
								}

	uint16_t										allocate()									{return m_active_block_count++;}
	uint32_t										active_block_count() const	{return m_active_block_count;}
	std::span<const uint64_t>		block_data() const					{return std::span<const uint64_t>(m_data.data(), m_active_block_count * m_blocksize * m_blocksize);}

private:
	std::size_t 	calc_index(uint32_t block, uint32_t x, uint32_t y) const
								{
									return 	(block * m_blocksize * m_blocksize) +
													((y&(m_blocksize-1))*m_blocksize) +
													(x&(m_blocksize-1));
								}
};

class TileMap
{
private:
	std::string											m_name;
	TilemapBlocks										m_tilemap_blocks;
	std::vector<uint16_t>						m_indices;
	uint32_t 												m_id						= 0;
	uint32_t												m_width 				= 0;
	uint32_t												m_height 				= 0;
	uint32_t 												m_blocksize			= 0;
	uint32_t 												m_tilesize			= 0;
	uint32_t												m_blocks_wide		= 0;
	uint32_t												m_blocks_high		= 0;

public:
	TileMap() = delete;
	TileMap(uint32_t id, std::string_view name, uint32_t width, uint32_t height, uint32_t blocksize, uint32_t tilesize)
		: m_name(name)
		, m_tilemap_blocks(blocksize, width*height)
		, m_id(id)
		, m_width(width)
		, m_height(height)
		, m_blocksize(blocksize)
		, m_tilesize(tilesize)
		, m_blocks_wide((width+(blocksize-1))/blocksize)
		, m_blocks_high((height+(blocksize-1))/blocksize)
		{
			m_indices.resize(m_blocks_wide * m_blocks_high, INDEX_EMPTY);
		}

	~TileMap() = default;
	TileMap(const TileMap &) = delete;
	TileMap(TileMap &&) = delete;
	TileMap & operator=(const TileMap &) = delete;
	TileMap & operator=(TileMap &&) = delete;

	uint32_t										id() const									{return m_id;}
	uint32_t										width() const 							{return m_width;}
	uint32_t										height() const 							{return m_height;}
	uint32_t 										blocks_wide() const 				{return m_blocks_wide;}
	uint32_t 										blocks_high() const 				{return m_blocks_high;}
	uint32_t										block_size() const					{return m_blocksize;}
	uint32_t										tile_size() const						{return m_tilesize;}
	const std::string &					name() const								{return m_name;}
	std::size_t									active_block_count() const	{return m_tilemap_blocks.active_block_count();}
	std::span<const uint64_t>		block_data() const					{return m_tilemap_blocks.block_data();}
	std::span<const uint16_t>		indices() const							{return std::span<const uint16_t>(m_indices.data(), m_indices.size());}

	uint16_t							allocate_block();
	void									set(uint32_t x, uint32_t y, uint64_t value);
	uint64_t							get(uint32_t x, uint32_t y);
	void									print() const;
};

} // namespace gap::tilemap


#endif // ! defined GUARD_ADE_GAME_ASSET_PACKER_TILEMAP_H
