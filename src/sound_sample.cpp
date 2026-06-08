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


std::unique_ptr<SoundSample>
load_raw_sound_sample(const std::filesystem::path & path, gap::FileSystem & filesystem)
{
	auto file = filesystem.load(path);
	if(file.empty())
	{
		std::cerr << "LOAD_SOUNDSAMPLE: Failed to load file '" << path << "'\n";
		return nullptr;
	}

	auto name = path.stem().string();
	std::transform(begin(name), end(name), begin(name), ::toupper);
	auto p_sample = std::make_unique<SoundSample>();

	p_sample->name = name;
	p_sample->data = std::move(file);

 	return p_sample;
}

std::unique_ptr<SoundSample>
load_wav_sound_sample(const std::filesystem::path & path, gap::FileSystem & filesystem)
{
	std::cerr << "LOAD_SOUNDSAMPLE: Failed to load file '" << path << "'. WAV Loading is not implemented\n";
	return nullptr;
}

std::unique_ptr<SoundSample>
load_sound_sample(const std::filesystem::path & path, gap::FileSystem & filesystem)
{
	auto ext = path.extension().string();
	std::transform(begin(ext), end(ext), begin(ext), ::tolower);
	if(ext == ".wav")
		return load_wav_sound_sample(path, filesystem);
	return load_raw_sound_sample(path, filesystem);
}

//=============================================================================
//
//	IMA ADPCM ENCODER
//
//=============================================================================
struct ima_state_t
{
	int predictor = 0;
	int step_index = 0;
};

static const int s_step_table[89] =
{
	7, 8, 9, 10, 11, 12, 13, 14, 16, 17,
	19, 21, 23, 25, 28, 31, 34, 37, 41, 45,
	50, 55, 60, 66, 73, 80, 88, 97, 107, 118,
	130, 143, 157, 173, 190, 209, 230, 253, 279, 307,
	337, 371, 408, 449, 494, 544, 598, 658, 724, 796,
	876, 963, 1060, 1166, 1282, 1411, 1552,
	1707, 1878, 2066, 2272, 2499, 2749,
	3024, 3327, 3660, 4026, 4428, 4871,
	5358, 5894, 6484, 7132, 7845, 8630,
	9493, 10442, 11487, 12635, 13899,
	15289, 16818, 18500, 20350, 22385,
	24623, 27086, 29794, 32767
};

static const int s_index_table[16] =
{
	-1, -1, -1, -1,
	 2,  4,  6,  8,
	-1, -1, -1, -1,
	 2,  4,  6,  8
};

static uint8_t encode_sample(	ima_state_t& state,	int16_t sample)
{
	int step = s_step_table[state.step_index];

	int diff = sample - state.predictor;

	uint8_t nibble = 0;

	if (diff < 0)
	{
		nibble |= 8;
		diff = -diff;
	}

	int predicted_diff = step >> 3;

	if (diff >= step)
	{
		nibble |= 4;
		diff -= step;
		predicted_diff += step;
	}

	if (diff >= (step >> 1))
	{
		nibble |= 2;
		diff -= (step >> 1);
		predicted_diff += (step >> 1);
	}

	if (diff >= (step >> 2))
	{
		nibble |= 1;
		predicted_diff += (step >> 2);
	}

	if (nibble & 8)
		state.predictor -= predicted_diff;
	else
		state.predictor += predicted_diff;

	state.predictor =
		std::clamp(state.predictor, -32768, 32767);

	state.step_index += s_index_table[nibble];
	state.step_index =
		std::clamp(state.step_index, 0, 88);

	return nibble;
}

void
encode_ima_adpcm( const std::vector<int16_t>& pcm_samples, std::vector<uint8_t> & output)
{
	ima_state_t state;

	output.clear();
	output.reserve((pcm_samples.size() + 1) / 2);

	for (size_t i = 0; i < pcm_samples.size(); i += 2)
	{
		uint8_t low_nibble =
			encode_sample(state, pcm_samples[i]);

		uint8_t high_nibble = 0;

		if ((i + 1) < pcm_samples.size())
			high_nibble =
				encode_sample(state, pcm_samples[i + 1]);

		output.push_back(
			low_nibble |
			(high_nibble << 4));
	}
}


//=============================================================================
//
//	FORMAT CONVERSION FUNCTIONS
//
//=============================================================================

void convert_to_s16(const SoundSample & sample, std::vector<int16_t> & output)
{
	output.clear();

	switch(sample.format)
	{
		case FORMAT_U16:
			output.reserve(sample.data.size() / 2);
			for(size_t i=0; i<sample.data.size(); i+=2)
			{
				int32_t s = static_cast<int32_t>(sample.data[i]) + (static_cast<int32_t>(sample.data[i+1]) * 256);
				s -= 32768;
				output.push_back(static_cast<int16_t>(s));
			}
			return;

		case FORMAT_S16:
			output.reserve(sample.data.size() / 2);
			for(size_t i=0; i<sample.data.size(); i+=2)
			{
				int16_t s = sample.data[i] | (sample.data[i+1] << 8);
				output.push_back(s);
			}
			return;

		case FORMAT_S8:
			output.reserve(sample.data.size());
			for(size_t i=0; i<sample.data.size(); ++i)
			{
				int16_t s = static_cast<int8_t>(sample.data[i]);
				s *= 256;
				output.push_back(s);
			}
			return;

		case FORMAT_U8:
			output.reserve(sample.data.size());
			for(size_t i=0; i<sample.data.size(); ++i)
			{
				int16_t s = static_cast<int16_t>(sample.data[i]) - 128;
				s *= 256;
				output.push_back(s);
			}
			return;

		default:
			std::cerr << "convert_to_s16: Unsupported source format: " << static_cast<int>(sample.format) << '\n';
			return;
	}
	std::cerr << "convert_to_s16: Unsupported source format: " << static_cast<int>(sample.format) << '\n';
}

void
convert_from_s16(const std::vector<int16_t> & input, std::vector<uint8_t> & output, uint8_t format)
{
	output.clear();

	switch(format)
	{
		case FORMAT_U16:
			output.reserve(input.size() * 2);
			for(auto s : input)
			{
				int32_t s32 = s;
				s32 += 32768;
				output.push_back(s32 & 0xFF);
				output.push_back((s32 >> 8) & 0xFF);
			}
			return;

		case FORMAT_S16:
			output.reserve(input.size() * 2);
			for(auto s : input)
			{
				output.push_back(s & 0xFF);
				output.push_back((s >> 8) & 0xFF);
			}
			return;

		case FORMAT_S8:
			output.reserve(input.size());
			for(auto s : input)
			{
				s /= 256;
				if(s < -128) s = -128;
				if(s > 127) s = 127;
				output.push_back(static_cast<uint8_t>(s));
			}
			return;

		case FORMAT_U8:
			output.reserve(input.size());
			for(auto s : input)
			{
				s /= 256;
				s += 128;
				if(s < 0) s = 0;
				if(s > 255) s = 255;
				output.push_back(static_cast<uint8_t>(s));
			}
			return;

		case FORMAT_IMA_ADPCM:
			encode_ima_adpcm(input, output);
			return;

		default:
			std::cerr << "convert_from_s16: Unsupported target format: " << static_cast<int>(format) << '\n';
			return;
	}
	std::cerr << "convert_from_s16: Unsupported target format: " << static_cast<int>(format) << '\n';
}

std::unique_ptr<SoundSample>
convert_sample(const SoundSample & sample, uint8_t format, uint16_t rate)
{
	auto p_sample = std::make_unique<SoundSample>();
	p_sample->name = sample.name;
	p_sample->format = format;
	p_sample->sample_rate = rate;

	std::vector<int16_t> intermediate;
	convert_to_s16(sample, intermediate);
	if(intermediate.empty())
		return nullptr;

	// TODO(Ade): Implement resampling if rate != sample.sample_rate

	convert_from_s16(intermediate, p_sample->data, format);

	return p_sample;
}


} // namespace gap::sound

