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
#ifndef GUARD_ADE_JG_TILEMAP_TEST_TILEMAP_H
#define GUARD_ADE_JG_TILEMAP_TEST_TILEMAP_H

#include <cstdint>
#include <array>
#include "utils/errors.h"
#include "utils/maths.h"

template<typename T, std::size_t Nblksize >
struct TilemapBlock
{
	static constexpr std::size_t 	block_size = Nblksize;
	
	T			data[Nblksize * Nblksize];
	T			at(uint32_t x, uint32_t y)						{return data[((y&(Nblksize-1))*Nblksize)+(x&(Nblksize-1))];}
	void	set(uint32_t x, uint32_t y, T value)	{data[((y&(Nblksize-1))*Nblksize)+(x&(Nblksize-1))]=value;}
};

template< typename 		T,
					std::size_t Nindexcapacity,
					std::size_t	Nblocks,
					std::size_t Nblksize >
class TileMap
{
public:
	static constexpr std::size_t		block_size 			= Nblksize;
	static constexpr std::size_t		block_capacity	= Nblksize;
	using 													tile_type 			= T;

private:
	static_assert(jg::maths::is_powerof2(Nblksize), "Block size must be a power of 2");
	static_assert(Nindexcapacity > 0, "Must not be 0");
	static_assert(Nblocks > 0, "Must not be 0");
	static_assert(Nblocks <= Nindexcapacity, "It doesn't make sence for the number of blocks to be larger than the number of indices!");

	static constexpr uint16_t 							INDEX_EMPTY 		= 0xFFFE;		// Block is empty. Setting a tile will allocate a new block.
	static constexpr uint16_t 							INDEX_UNLOADED 	= 0xFFFF;		// This block has been unloaded and will require loading to become active.


	struct BlockFlags
	{
		std::array<uint8_t,(Nblocks+7)/8>			bitset;	

		void				set(uint16_t index)			{assert(index<Nblocks); bitset[index/8] |= (1<<(index&0x07));}
		void				reset(uint16_t index)		{assert(index<Nblocks); bitset[index/8] &= (1<<(index&0x07))^0xFF;}
		void				clear()									{bitset.fill(0);}
		uint16_t		allocate()							{
																					for(uint16_t i=0;i<Nblocks;i+=8) 
																					{
																						if(bitset[i/8]!=0xFF) 
																						{
																							for(uint16_t j=0;j<8;++j)
																							{
																								if((bitset[i/8] & (1<<j))==0) 
																									{set(i+j); return i+j;}
																							}
																						}
																					}; 
																					return INDEX_EMPTY;
																				}
	};


	std::array<uint16_t, Nindexcapacity>						m_indices;
	std::array<TilemapBlock<T,Nblksize>, Nblocks>		m_blocks;
	std::span<const TilemapBlock<T,Nblksize>>				m_external_blocks;
	BlockFlags																			m_block_flags;
	uint32_t																				m_width 				= 0;
	uint32_t																				m_height 				= 0;
	uint32_t																				m_blocks_wide		= 0;
	uint32_t																				m_blocks_high		= 0;

public:
	TileMap() = default;
	~TileMap() = default;
	TileMap(const TileMap &) = delete;
	TileMap(TileMap &&) = delete;
	TileMap & operator=(const TileMap &) = delete;
	TileMap & operator=(TileMap &&) = delete;

	uint32_t					width() const 					{return m_width;}
	uint32_t					height() const 					{return m_height;}
	uint32_t 					blocks_wide() const 		{return m_blocks_wide;}
	uint32_t 					blocks_high() const 		{return m_blocks_high;}

	std::error_code		create(unsigned int width, unsigned int height)
										{
											if(	!jg::maths::is_multiple_of(width, Nblksize) ||
													!jg::maths::is_multiple_of(height, Nblksize) ||
													((m_width*m_height) > (Nindexcapacity * Nblksize * Nblksize)) )
												return game::error::invalid_argument;
											reset();	
											m_width 				= width;
											m_height 				= height;
											m_blocks_wide		= width / Nblksize;
											m_blocks_high		= height / Nblksize;
											return {};
										}

//	void							load( std::span<const uint16_t> indices, std::span<const Block
	void							reset()
										{
											m_width = m_height = 0;
											m_indices.fill(INDEX_EMPTY);
											m_block_flags.clear();
										}

	uint16_t					allocate_block()
										{
											auto index = m_block_flags.allocate();
											//TODO(Ade): Add algorithm to handle the case where a block may need to be freed to satisfy this request.
											return index;
										}

	void							set(uint32_t x, uint32_t y, T value)
										{
											assert(x<m_width);
											assert(y<m_height);
											const std::size_t iblk = ((y/Nblksize)*m_blocks_wide)+(x/Nblksize);
											assert(iblk < Nindexcapacity);
											auto index = m_indices[iblk];

											switch(index)
											{
												case INDEX_UNLOADED :
													//TODO(Ade): Add mechanism to re-load block.
													break;

												case INDEX_EMPTY :
													index = allocate_block();
													if(index == INDEX_EMPTY)
														break;
													m_indices[iblk] = index;
													[[fallthrough]];
													
												default :
													m_blocks[index].set(x,y,value);
													break;
											}
										}

	T									get(uint32_t x, uint32_t y)
										{
											assert(x<m_width);
											assert(y<m_height);
											const std::size_t iblk = ((y/Nblksize)*m_blocks_wide)+(x/Nblksize);
											assert(iblk < Nindexcapacity);
											auto index = m_indices[iblk];
											switch(index)
											{
												case INDEX_UNLOADED :
													//TODO(Ade): Add mechanism to re-load block.
													return static_cast<T>(0);
													break;

												case INDEX_EMPTY :
													return static_cast<T>(0);
											}
											return m_blocks[index].at(x,y);
										}

private:
};




#endif // ! defined GUARD_ADE_JG_TILEMAP_TEST_TILEMAP_H
