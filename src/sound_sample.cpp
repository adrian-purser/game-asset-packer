//=============================================================================
//	FILE:						sound_sample.cpp
//	SYSTEM:				 	game asset packer
//	DESCRIPTION:
//-----------------------------------------------------------------------------
//	COPYRIGHT:			(C)Copyright 2025 Adrian Purser. All Rights Reserved.
//	LICENCE:				MIT
//	MAINTAINER:			AJP - Adrian Purser <ade@arcadestuff.com>
//	CREATED:				10-OCT-2025 Adrian Purser <ade@arcadestuff.com>
//=============================================================================
#include <iostream>
#include "sound_sample.h"
#include "miniaudio.h"

namespace gap::sound
{

SoundSample::SoundSample( std::string_view 				name,
													uint16_t								sample_rate,
													uint8_t									format,
													uint8_t									compression,
													std::vector<uint8_t>		data )
	: m_name(name)
	, m_sample_rate(sample_rate)
	, m_format(format)
	, m_compression(compression)
	, m_data(data)
{

}

std::unique_ptr<SoundSample>
load(const std::filesystem::path & path, gap::FileSystem & filesystem)
{
	auto file = filesystem.load(path);
	if(file.empty())
	{
		std::cerr << "LOAD_SOUNDSAMPLE: Failed to load file '" << path << "'\n";
		return nullptr;
	}



	return nullptr;
}

} // namespace gap::image

