//=============================================================================
//	FILE:					asset_sound.cpp
//	SYSTEM:				Game Asset Packer
//	DESCRIPTION:
//-----------------------------------------------------------------------------
//  COPYRIGHT:		(C)Copyright 2022 Adrian Purser. All Rights Reserved.
//	LICENCE:
//	MAINTAINER:		AJP - Adrian Purser <ade&arcadestuff.com>
//	CREATED:			15-MAY-2022 Adrian Purser <ade&arcadestuff.com>
//=============================================================================

#include "utility/hash.h"
#include "asset_sound.h"
#include "modaudio/modular_audio.h"

namespace gap::assets::sound
{

#define HASH(s) ade::hash::hash_ascii_string_as_lower(s)

int 
get_module_type( const std::string & name )
{
	switch(ade::hash::hash_ascii_string_as_lower(name.c_str()))
	{
		case HASH("OSCILLATOR_SIN") :									return (int) modaudio::ModuleType::OSCILLATOR_SINE;
		case HASH("OSCILLATOR_TRIANGLE") :						return (int) modaudio::ModuleType::OSCILLATOR_TRIANGLE;
		case HASH("OSCILLATOR") :											return (int) modaudio::ModuleType::OSCILLATOR;
		case HASH("NOISE") :													return (int) modaudio::ModuleType::NOISE;
		case HASH("AMPLIFIER") :											return (int) modaudio::ModuleType::AMPLIFIER;
		case HASH("SINGLE_POLE_LOW_PASS_FILTER") :		return (int) modaudio::ModuleType::SINGLE_POLE_LOW_PASS_FILTER;
		case HASH("ENVELOPE_ADSR") :									return (int) modaudio::ModuleType::ENVELOPE_ADSR;
		case HASH("CONSTANT") :												return (int) modaudio::ModuleType::CONSTANT;
		case HASH("MIXER2") :													return (int) modaudio::ModuleType::MIXER2;
		case HASH("CLOCKED_RANDOM") :									return (int) modaudio::ModuleType::CLOCKED_RANDOM;
		case HASH("WAVEFORM") :												return (int) modaudio::ModuleType::WAVEFORM;
	}
	return -1;
}






} // namespace gap::assets::sound
