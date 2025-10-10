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

static constexpr uint8_t		FORMAT_S8		= 0;
static constexpr uint8_t		FORMAT_U8		= 1;
static constexpr uint8_t		FORMAT_S16	= 2;
static constexpr uint8_t		FORMAT_U16	= 3;

static constexpr uint8_t		COMPRESSION_NONE	= 0;


class SoundSample
{
private:
	std::string							m_name;
	uint16_t								m_sample_rate 	= 0;
	uint8_t									m_format 				= 0;
	uint8_t									m_compression 	= 0;
	std::vector<uint8_t>		m_data;

public:
	SoundSample() = default;
	~SoundSample() = default;
	SoundSample( 	std::string_view name,
								uint16_t								sample_rate,
								uint8_t									format,
								uint8_t									compression,
								std::vector<uint8_t>		data );

};

std::unique_ptr<SoundSample>			load_sound_sample(const std::filesystem::path & filename, gap::FileSystem & filesystem);

} // namespace gap::sound

#endif // ! defined GUARD_ADE_GAME_ASSET_PACKER_SOUND_SAMPLE_H
