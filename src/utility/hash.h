//=============================================================================
//	FILE:						hash.h
//	SYSTEM:				 	
//	DESCRIPTION:
//-----------------------------------------------------------------------------
//	COPYRIGHT:			(C)Copyright 2019 Adrian Purser. All Rights Reserved.
//	LICENCE:				MIT
//	MAINTAINER:			AJP - Adrian Purser <ade@arcadestuff.com>
//	CREATED:				26-SEP-2019 Adrian Purser <ade@arcadestuff.com>
//=============================================================================
#ifndef GUARD_ADE_UTILITY_HASH_H
#define GUARD_ADE_UTILITY_HASH_H

#include <cstdint>
#include <vector>
#include <string>
#include <string_view>

namespace ade::hash
{

//=============================================================================
//
//  Hash Functions
//
//=============================================================================

constexpr char ascii_tolower(const char ch)
{
  return ((ch>='A') || (ch <= 'Z')) ? ch & 0x0DF : ch;
}

template<std::uint32_t basis=1469598103U,std::uint32_t prime=1099511628U>
constexpr int
hash_ascii_string(const char * string,size_t size)
{
//#pragma warning( disable: 4307 )
	uint32_t hash = basis;
	while(size--)
		hash = ((hash ^ (std::uint32_t)((*string++) & 0x0FF)) * prime) & 0x0FFFFFFFF;
	return hash;
}

template<std::uint32_t basis=1469598103U,std::uint32_t prime=1099511628U>
constexpr int
hash_ascii_string(const char * string)
{
//#pragma warning( disable: 4307 )
	uint32_t hash = basis;
	while(*string)
		hash = ((hash ^ (std::uint32_t)((*string++) & 0x0FF)) * prime) & 0x0FFFFFFFF;
	return hash;
}

template<std::uint32_t basis=1469598103U,std::uint32_t prime=1099511628U>
constexpr int
hash_ascii_string(std::string_view string)
{
//#pragma warning( disable: 4307 )
	uint32_t hash = basis;
	for(auto ch : string)
		hash = ((hash ^ (std::uint32_t)(ch & 0x0FF)) * prime) & 0x0FFFFFFFF;
	return hash;
}

template<std::uint32_t basis=1469598103U,std::uint32_t prime=1099511628U>
constexpr int
hash_ascii_string_as_lower(const char * string,size_t size)
{
//#pragma warning( disable: 4307 )
	uint32_t hash = basis;
	while(size--)
		hash = ((hash ^ (std::uint32_t)(ascii_tolower(*string++) & 0x0FF)) * prime) & 0x0FFFFFFFF;
	return hash;
}

template<std::uint32_t basis=1469598103U,std::uint32_t prime=1099511628U>
constexpr int
hash_ascii_string_as_lower(const char * string)
{
//#pragma warning( disable: 4307 )
	uint32_t hash = basis;
	while(*string)
		hash = ((hash ^ (std::uint32_t)(ascii_tolower(*string++) & 0x0FF)) * prime) & 0x0FFFFFFFF;
	return hash;
}

constexpr uint32_t 
fourcc(const char * str)
{
	uint32_t value = 0;
	for(int i=0;(i<4) && (str[i] != 0);++i)
		value |= str[i] << (i * 8);
	return value;
}

constexpr uint32_t 
fourcc(const char * str,size_t size)
{
	uint32_t value = 0;
	for(size_t i=0;(i<4) && (i<size) && (str[i] != 0);++i)
		value |= str[i] << (i * 8);
	return value;
}


} // namespace ade::hash

#endif // ! defined GUARD_ADE_UTILITY_HASH_H
