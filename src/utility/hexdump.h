//=============================================================================
//	FILE:						hexdump.h
//	SYSTEM:				 	
//	DESCRIPTION:
//-----------------------------------------------------------------------------
//	COPYRIGHT:			(C)Copyright 2013 Adrian Purser. All Rights Reserved.
//	LICENCE:				MIT
//	MAINTAINER:			AJP - Adrian Purser <ade@arcadestuff.com>
//	CREATED:				11-DEC-2013 Adrian Purser <ade@arcadestuff.com>
//=============================================================================
#ifndef GUARD_ADE_HEXDUMP_H
#define GUARD_ADE_HEXDUMP_H

#include <iostream>
#include <iomanip>
#include <cstdint>
#include <string>
#include <functional>

namespace ade
{

class hexdump
{
private:
	const char *				m_p_data;
	int							m_size;
	int							m_columns;
	std::uint32_t				m_baseaddr;

	hexdump(void);
	
public:
	explicit hexdump(	const void *	p_data,
						int				size,
						int				columns = 16,
						std::uint32_t	baseaddr = 0 )
		: m_p_data((const char *)p_data)
		, m_size(size)
		, m_columns(columns)
		, m_baseaddr(baseaddr)
	{
	}

	inline char hex_char(int val) const {val &= 0x0F;return (val > 9 ? val + ('A'-10) : val + '0');}

	std::ostream & operator()(std::ostream & stream) const
	{
		if(m_p_data && (m_size>0))
		{
			int		centre		= (m_columns & 1 ? 0 : m_columns/2);
			auto	size		= m_size;
			auto	baseaddr	= m_baseaddr;
			auto	p_data		= m_p_data;

			while(size)
			{
				std::string line;
				line.reserve((m_columns * 4) + 32);

				for(int d=0;d<8;++d)
					line.push_back(hex_char((baseaddr >> ((7-d)*4))&0x0F));

				line.append(": ");

				int i=0;
				while((i<m_columns) && (i<size))
				{
					line.push_back(hex_char(p_data[i]>>4));
					line.push_back(hex_char(p_data[i]));
					line.push_back(' ');
					++i;
					if(i == centre)
						line.push_back(' ');
				}

				if(i < centre)
					line.push_back(' ');

				while((i++)<m_columns)
					line.append("   ");

				line.push_back(' ');
				i=0;
				while((i<m_columns) && (i<size))
				{
					line.push_back(p_data[i]<32 ? '.' : p_data[i]);
					++i;
				}

				line.append("\r\n");

				size = (size<m_columns ? 0:size-m_columns);
				baseaddr += m_columns;
				p_data += m_columns;

				stream << line;
			}
		}

		return stream;
	}

};

inline std::ostream & operator << (std::ostream & stream,hexdump hd) {return hd(stream);}


inline char hex_char(int val) {val &= 0x0F;return (val > 9 ? val + ('A'-10) : val + '0');}

inline
void hex_dump(const uint8_t *	p_data, int size, int columns, uint32_t	baseaddr, std::function<void(const std::string &)> callback)
{
	if(p_data && (size>0))
	{
		int		centre		= (columns & 1 ? 0 : columns/2);
		std::string line;

		while(size)
		{
			line.clear();
			line.reserve((columns * 4) + 32);

			for(int d=0;d<8;++d)
				line.push_back(hex_char((baseaddr >> ((7-d)*4))&0x0F));

			line.append(": ");

			int i=0;
			while((i<columns) && (i<size))
			{
				line.push_back(hex_char(p_data[i]>>4));
				line.push_back(hex_char(p_data[i]));
				line.push_back(' ');
				++i;
				if(i == centre)
					line.push_back(' ');
			}

			if(i < centre)
				line.push_back(' ');

			while((i++)<columns)
				line.append("   ");

			line.push_back(' ');
			i=0;
			while((i<columns) && (i<size))
			{
				line.push_back(((p_data[i]<32) || (p_data[i]>127)) ? '.' : p_data[i]);
				++i;
			}

			line.append("\r\n");

			size = (size<columns ? 0:size-columns);
			baseaddr += columns;
			p_data += columns;

			callback(line);
		}
	}
}


} // namespace ade

#endif // ! defined GUARD_ADE_HEXDUMP_H
