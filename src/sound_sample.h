//=============================================================================
//	FILE:						sound_sample.h
//	SYSTEM:				 	game asset packer
//	DESCRIPTION:
//-----------------------------------------------------------------------------
//	COPYRIGHT:			(C)Copyright 2019 Adrian Purser. All Rights Reserved.
//	LICENCE:				MIT
//	MAINTAINER:			AJP - Adrian Purser <ade@arcadestuff.com>
//	CREATED:				25-SEP-2019 Adrian Purser <ade@arcadestuff.com>
//=============================================================================
#ifndef GUARD_ADE_GAME_ASSET_PACKER_SOUND_SAMPLE_H
#define GUARD_ADE_GAME_ASSET_PACKER_SOUND_SAMPLE_H

#include <cstdint>
#include <vector>
#include <string>
#include <cmath>
#include <utility>
#include <filesystem>
#include "filesystem.h"
#include "utility/hash.h"

namespace gap::sound
{

static constexpr uint8_t		FORMAT_S8					= 0;
static constexpr uint8_t		FORMAT_U8					= 1;
static constexpr uint8_t		FORMAT_S16				= 2;
static constexpr uint8_t		FORMAT_U16				= 3;
static constexpr uint8_t		FORMAT_IMA_ADPCM	= 4;

static constexpr uint8_t		FORMAT_UNKNOWN 		= 0xFF;

struct SoundSample
{
	std::string							name;
	uint16_t								sample_rate 	= 0;
	uint8_t									format 				= FORMAT_UNKNOWN;
	std::vector<uint8_t>		data;
};

std::unique_ptr<SoundSample>			load_sound_sample(const std::filesystem::path & filename, gap::FileSystem & filesystem);
std::unique_ptr<SoundSample>			convert_sample(const SoundSample & sample, uint8_t format, uint16_t rate);

} // namespace gap::sound

#endif // ! defined GUARD_ADE_GAME_ASSET_PACKER_SOUND_SAMPLE_H
