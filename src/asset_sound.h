//=============================================================================
//	FILE:					asset_sound.h
//	SYSTEM:				Game Asset Packer
//	DESCRIPTION:
//-----------------------------------------------------------------------------
//  COPYRIGHT:		(C)Copyright 2022 Adrian Purser. All Rights Reserved.
//	LICENCE:
//	MAINTAINER:		AJP - Adrian Purser <ade&arcadestuff.com>
//	CREATED:			15-MAY-2022 Adrian Purser <ade&arcadestuff.com>
//=============================================================================
#ifndef GUARD_ADE_GAMES_ASSET_PACKER_ASSET_SOUND_H
#define GUARD_ADE_GAMES_ASSET_PACKER_ASSET_SOUND_H

#include <vector>
#include <string>

namespace gap::assets
{


struct SoundParam
{
	int					index;
	float				value;
};

struct SoundModule
{
	int														type;
	std::string										name;
	std::vector<SoundParam>				parameters;
};

struct SoundConnection
{
	int														source_module;
	int														dest_module;
	int														output;
	int														input;
};

struct SoundControl
{
	int														control;
	int														module;
	int														param;
};

struct SoundOutput
{
	int														output;
	int														module;
	int														module_output;
};

struct Sound
{
	std::string										name;
	std::vector<SoundModule>			modules;
	std::vector<SoundConnection>	connections;
	std::vector<SoundControl>			controls;
	std::vector<SoundOutput>			outputs;
};


namespace sound
{

int 		get_module_type( const std::string & name );

} // namespace sound


} // namespace gap::assets

#endif // ! defined GUARD_ADE_GAMES_ASSET_PACKER_ASSET_SOUND_H

