//=============================================================================
//	FILE:					app.h
//	SYSTEM:				Game Asset Packer
//	DESCRIPTION:
//-----------------------------------------------------------------------------
//  COPYRIGHT:		(C)Copyright 2019 Adrian Purser. All Rights Reserved.
//	LICENCE:
//	MAINTAINER:		AJP - Adrian Purser <ade&arcadestuff.com>
//	CREATED:			24-SEP-2019 Adrian Purser <ade&arcadestuff.com>
//=============================================================================
#ifndef GUARD_ADE_GAMES_ASSET_PACKER_APP_H
#define GUARD_ADE_GAMES_ASSET_PACKER_APP_H

#include "configuration.h"
#include "filesystem.h"
#include "assets.h"

namespace gap
{

class Application
{
private:
	const gap::Configuration & 		m_config;
	gap::FileSystem								m_filesystem;

public:
	Application() = default;
	Application(const gap::Configuration & config);
	~Application() = default;

	int			run();

};


} // namespace gap

#endif // ! defined GUARD_ADE_GAMES_ASSET_PACKER_APP_H