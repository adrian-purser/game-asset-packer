//=============================================================================
//	FILE:					tilemap.cpp
//	SYSTEM:				SpaceGame
//	DESCRIPTION:
//-----------------------------------------------------------------------------
//  COPYRIGHT:		(C) Copyright 2025 Adrian Purser. All Rights Reserved.
//	LICENCE:			MIT - See LICENSE file for details
//	MAINTAINER:		Adrian Purser <ade@adrianpurser.co.uk>
//	CREATED:			12-AUG-2025 Adrian Purser <ade@adrianpurser.co.uk>
//=============================================================================
#include <format>
#include <print>
#include <string>
#include "tilemap.h"
#include "utility/ansi.h"

namespace gap::tilemap
{

uint16_t
TileMap::allocate_block()
{
	auto index = m_tilemap_blocks.allocate();
	//TODO(Ade): Add algorithm to handle the case where a block may need to be freed to satisfy this request.
	return index;
}

void
TileMap::set(uint32_t x, uint32_t y, uint64_t value)
{
	assert(x<m_width);
	assert(y<m_height);
	const std::size_t iblk = ((y/m_blocksize)*m_blocks_wide)+(x/m_blocksize);
	assert(iblk < m_indices.size());
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
			m_tilemap_blocks.set(index,x,y,value);
			break;
	}
}

uint64_t
TileMap::get(uint32_t x, uint32_t y)
{
	assert(x<m_width);
	assert(y<m_height);
	const std::size_t iblk = ((y/m_blocksize)*m_blocks_wide)+(x/m_blocksize);
	assert(iblk < m_indices.size());
	auto index = m_indices[iblk];
	switch(index)
	{
		case INDEX_UNLOADED :
			//TODO(Ade): Add mechanism to re-load block.
			return 0U;
			break;

		case INDEX_EMPTY :
			return 0U;
	}
	return m_tilemap_blocks.get(index,x,y);
}

void
TileMap::print() const
{
	for(int by=0;by<m_blocks_high;++by)
	{
		for(int row = -2;row <= (int)m_blocksize;++row)
		{
			for(int bx=0; bx<m_blocks_wide;++bx)
			{
				const std::size_t iblk = (by*m_blocks_wide)+bx;
				assert(iblk < m_indices.size());
				auto index = m_indices[iblk];

				if(row == -2)
				{
					auto istr = std::format("{:04X}",index);
					std::print(FOREGROUND_YELLOW"{:{}} ",istr,m_blocksize+3);
					ansi::stdio::reset_colour();
				}
				else if((row == -1) || (row==m_blocksize))
				{
					printf(FOREGROUND_LIGHT_BLUE "+");
					for(int i=0;i<m_blocksize;++i)
						putchar('-');
					printf("+ ");
					ansi::stdio::reset_colour();
				}
				else
				{
					printf(FOREGROUND_LIGHT_BLUE "|" FOREGROUND_WHITE);

					switch(index)
					{
						case INDEX_UNLOADED :
							for(int i=0;i<m_blocksize;++i)
								putchar('/');
							break;

						case INDEX_EMPTY :
							for(int i=0;i<m_blocksize;++i)
								putchar('.');
							break;

						default :
							for(int i=0;i<m_blocksize;++i)
							{
								auto tile = m_tilemap_blocks.get(index,i,row);
								char ch = tile > 9 ? '#' : '0' + (char)(tile&0x7F);
								ansi::stdio::background_colour(ansi::sgr::FG_BLACK + std::min<int>(tile,9));
								putchar(tile == 0 ? '.' : ch);
							}
							break;
					}
					ansi::stdio::reset_colour();
					printf(FOREGROUND_LIGHT_BLUE "| ");
					ansi::stdio::reset_colour();
				}
			}
			printf("\n");
		}
		printf("\n");
	}
}


} // namespace gap::tilemap


