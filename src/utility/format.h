//=============================================================================
//	FILE:						format.h
//	SYSTEM:				 	
//	DESCRIPTION:
//-----------------------------------------------------------------------------
//	COPYRIGHT:			(C)Copyright 2019 Adrian Purser. All Rights Reserved.
//	LICENCE:				MIT
//	MAINTAINER:			AJP - Adrian Purser <ade@arcadestuff.com>
//	CREATED:				28-SEP-2019 Adrian Purser <ade@arcadestuff.com>
//=============================================================================
#ifndef GUARD_ADE_UTILITY_FORMAT_H
#define GUARD_ADE_UTILITY_FORMAT_H

#include <cstdint>
#include <string>

namespace ade::format
{
//=============================================================================
//
//  Char to Integer
//
//=============================================================================
constexpr int hex_digit(const char digit)
{
	if((digit >= '0') && (digit <= '9')) return digit - '0';
	if((digit >= 'A') && (digit <= 'F')) return digit - ('A' - 10);
	if((digit >= 'a') && (digit <= 'f')) return digit - ('a' - 10);
	return 0;
}

//=============================================================================
//
//  Integer to String
//
//=============================================================================

constexpr char hex_char(int value)
{
	const int v = value & 0x0F;
  return (v < 10 ? '0' + v : ('A'-10) + v);
}

inline std::string hex_string(int value,int digits)
{
	std::string str;
	str.reserve(digits);

	for(int i=0;i<digits;++i)
		str.push_back(hex_char(value >> (((digits-1)-i) * 4)));

	return str;
}

inline std::string to_hex_string(uint32_t value, int digits = -1)
{
	std::string str;
	if(digits < 0)
	{
		bool b_leading = true;
		for(int i=7;i>=0;--i)
		{
			uint32_t digit = (value >> (i*4)) & 0x0F;
			if((digit != 0) || !b_leading)
			{
				str.push_back(hex_char(digit));
				b_leading = false;
			}	
		}
	}
	else
	{
		for(int i=digits-1;i>=0;--i)
			str.push_back(hex_char(value >> (i * 4)));
	}
	return str;
}

//=============================================================================
//
//  String to Integer
//
//=============================================================================

inline int
hexstring_to_uint32(std::string_view arg, uint32_t & out_result)
{
	if(arg.empty())
		return -1;

	uint32_t value=0;
	for(auto ch : arg)
	{
		ch = toupper(ch);
		if((ch >= '0') && (ch<='9'))
			value = (value << 4) | (ch-'0');
		else if((ch >= 'A') && (ch<='F'))
			value = (value << 4) | (ch-('A'-10));
		else 
			return -1;
	}

	out_result = value;
	return 0;
}

inline uint32_t
hexstring_to_uint32_or(std::string_view arg, uint32_t default_value)
{
	if(arg.empty())
		return default_value;

	uint32_t value=0;
	for(auto ch : arg)
	{
		ch = toupper(ch);
		if((ch >= '0') && (ch<='9'))
			value = (value << 4) | (ch-'0');
		else if((ch >= 'A') && (ch<='F'))
			value = (value << 4) | (ch-('A'-10));
		else 
			return default_value;
	}

	return value;
}

inline int
binstring_to_uint32(std::string_view arg, uint32_t & out_result)
{
	if(arg.empty())
		return -1;

	uint32_t value=0;
	for(auto ch : arg)
	{
		if(ch == '0') 			value = (value << 1);
		else if(ch == '1')	value = (value << 1) | 1;
		else return {};
	}
	out_result = value;
	return 0;
}

inline int
string_to_uint32(std::string_view arg, uint32_t & out_result)
{
	const int size = arg.size();

	if(arg.empty())
		return -1;

	switch(arg[0])
	{
		case '$' :	return hexstring_to_uint32(arg.substr(1),out_result);
		case '0' :
			if(size > 1)
			{
				switch(tolower(arg[1]))
				{
					case 'x' :	return hexstring_to_uint32(arg.substr(2),out_result);
					case 'b' :	return binstring_to_uint32(arg.substr(2),out_result);
				}
			}
			[[fallthrough]];

		default :
			{
				uint32_t value = 0;
				for(auto ch : arg)
				{
					if((ch >= '0') && (ch<='9'))
						value = (value * 10) + (ch-'0');
					else 
						return -1;
				}
				out_result = value;
			}
			break;
	}

	return 0;
}

inline int
string_to_int(std::string_view arg, int & out_result)
{
	// TODO: This function is horrible! Fix it.

	const int size = arg.size();
	bool b_negative = false;

	if(arg.empty())
		return -1;

	uint32_t val = 0;

	switch(arg[0])
	{
		case '$' :	{int res=hexstring_to_uint32(arg.substr(1),val); out_result = static_cast<int>(val); return res;}
		case '-' :	b_negative = true; arg = arg.substr(1); [[fallthrough]];

		case '0' :
			if(size > 1)
			{
				switch(tolower(arg[1]))
				{
					case 'x' :	{int res = hexstring_to_uint32(arg.substr(2),val); out_result = static_cast<int>(val); return res; }
					case 'b' :	{int res = binstring_to_uint32(arg.substr(2),val); out_result = static_cast<int>(val); return res; }
				}
			}
			[[fallthrough]];

		default :
			{
				uint32_t value = 0;
				for(auto ch : arg)
				{
					if((ch >= '0') && (ch<='9'))
						value = (value * 10) + (ch-'0');
					else 
						return -1;
				}
				out_result = b_negative ? -value : value;
			}
			break;
	}

	return 0;
}

//=============================================================================
//
//  8CC
//
//=============================================================================

static constexpr uint64_t
string_to_8cc(const std::string_view & str)
{
	uint64_t m8cc = 0UL;
	for(size_t i=0;(i<str.size()) && (i<8);++i)
		m8cc |= ((uint64_t)str[i]) << (i*8);
	return m8cc;
}

inline std::string 
to_8cc_string(uint64_t value)
{
	std::string str;
	for(int i=0;i<8;++i)
	{
		auto ch = (value >> (i*8)) & 0x0FF;
		if(ch == 0)
			break;
		str.push_back(ch);
	}
	return str;
}

//=============================================================================
//
//  Colours
//
//=============================================================================
namespace colour
{

inline
uint32_t parse_colour_uint32( const std::string_view & str )
{
	uint32_t clr = 0U;

	if(!str.empty())
	{
		if(str[0] == '#')
		{
			if(str.size() == 9) 			for(int i=0;i<8;++i)	clr |= hex_digit(str[i+1]) << ((7-i)*4);
			else if(str.size() == 7)	for(int i=0;i<6;++i)	clr |= hex_digit(str[i+1]) << ((5-i)*4);
			else if(str.size() == 4)	for(int i=0;i<3;++i)	{uint32_t d = hex_digit(str[i+1]); clr |= (d << ((2-i)*8)) | (d << (((2-i)*8)+4) );}
		}
		else if(isdigit(str[0])) string_to_uint32(str,clr);
	}

	return clr;
}


} // namespace colour

//=============================================================================
//
//  ASCII
//
//=============================================================================

namespace ascii
{

inline
std::string
character_name(uint32_t ch)
{
	switch(ch)
	{
		case 0   :	return "NUL";
		case 1   : 	return "SOH";
		case 2   : 	return "STX";
		case 3   : 	return "ETX";
		case 4   : 	return "EOT";
		case 5   : 	return "ENQ";
		case 6   : 	return "ACK";
		case 7   : 	return "BEL";
		case 8   : 	return "BS";
		case 9   : 	return "HT";
		case 10  : 	return "LF";
		case 11  : 	return "VT";
		case 12  : 	return "FF";
		case 13  : 	return "CR";
		case 14  : 	return "SO";
		case 15  : 	return "SI";
		case 16  : 	return "DLE";
		case 17  : 	return "DC1:XON";
		case 18  : 	return "DC2";
		case 19  : 	return "DC3:XOFF";
		case 20  : 	return "DC4";
		case 21  : 	return "NAK";
		case 22  : 	return "SYN";
		case 23  : 	return "ETB";
		case 24  : 	return "CAN";
		case 25  : 	return "EM";
		case 26  : 	return "SUB";
		case 27  : 	return "ESC";
		case 28  : 	return "FS";
		case 29  : 	return "GS";
		case 30  : 	return "RS";
		case 31  : 	return "US";
		case 127 :	return "DEL";
	}

	if(ch < 256)
	{
		std::string str("'");
		str.push_back(ch & 0x0FF);
		str.push_back('\'');
		return str;
	}

	return std::string("U+")+to_hex_string(ch);
}

} // namespace ascii




} // namespace ade::format

#endif // ! defined GUARD_ADE_UTILITY_FORMAT_H
