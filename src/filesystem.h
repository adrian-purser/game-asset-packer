//=============================================================================
//	FILE:					filesystem.h
//	SYSTEM:				Game Asset Packer
//	DESCRIPTION:
//-----------------------------------------------------------------------------
//  COPYRIGHT:		(C)Copyright 2019 Adrian Purser. All Rights Reserved.
//	LICENCE:
//	MAINTAINER:		AJP - Adrian Purser <ade&arcadestuff.com>
//	CREATED:			26-SEP-2019 Adrian Purser <ade&arcadestuff.com>
//=============================================================================
#ifndef GUARD_ADE_GAMES_ASSET_PACKER_FILESYSTEM_H
#define GUARD_ADE_GAMES_ASSET_PACKER_FILESYSTEM_H

#include "configuration.h"
#include "adefs/adefs.h"
#include "assets.h"

namespace gap
{

class FileSystem
{
private:
	adefs::AdeFS								m_adefs;

public:
	FileSystem() = default;
	~FileSystem() = default;

	int													mount(const std::string & package_name,const std::string & mountpoint = "/")	{return m_adefs.mount(package_name,mountpoint);}
	
	std::vector<std::uint8_t>		load( const std::string & filename )																					
															{
																// TODO: If the file can not be loaded from adefs then attempt to load it from the systems filesystem.
																return m_adefs.load(filename);
															}	

};


} // namespace gap

#endif // ! defined GUARD_ADE_GAMES_ASSET_PACKER_FILESYSTEM_H