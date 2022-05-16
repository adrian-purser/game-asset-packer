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


namespace gap::assets::sound
{

#define HASH(s) ade::hash::hash_ascii_string_as_lower(s)

modaudio::ModuleType 
get_module_type( const std::string & name )
{
	switch(ade::hash::hash_ascii_string_as_lower(name.c_str()))
	{
		case HASH("OSCILLATOR_SIN") :									return modaudio::ModuleType::OSCILLATOR_SINE;
		case HASH("OSCILLATOR_TRIANGLE") :						return modaudio::ModuleType::OSCILLATOR_TRIANGLE;
		case HASH("OSCILLATOR") :											return modaudio::ModuleType::OSCILLATOR;
		case HASH("NOISE") :													return modaudio::ModuleType::NOISE;
		case HASH("AMPLIFIER") :											return modaudio::ModuleType::AMPLIFIER;
		case HASH("SINGLE_POLE_LOW_PASS_FILTER") :		return modaudio::ModuleType::SINGLE_POLE_LOW_PASS_FILTER;
		case HASH("ENVELOPE_ADSR") :									return modaudio::ModuleType::ENVELOPE_ADSR;
		case HASH("CONSTANT") :												return modaudio::ModuleType::CONSTANT;
		case HASH("MIXER2") :													return modaudio::ModuleType::MIXER2;
		case HASH("CLOCKED_RANDOM") :									return modaudio::ModuleType::CLOCKED_RANDOM;
		case HASH("WAVEFORM") :												return modaudio::ModuleType::WAVEFORM;
	}
	return modaudio::ModuleType::INVALID;
}


int
get_module_param_type(modaudio::ModuleType module, const std::string & param_name)
{
	auto param = ade::hash::hash_ascii_string_as_lower(param_name.c_str());

	switch(module)
	{
		case modaudio::ModuleType::AMPLIFIER :
			switch(param)
			{
				case HASH("BIAS") : return modaudio::ModuleAmplifier::PARAM_BIAS;
				case HASH("GAIN") : return modaudio::ModuleAmplifier::PARAM_GAIN;
			}
			break;

		case modaudio::ModuleType::ENVELOPE_ADSR :
			switch(param)
			{
				case HASH("ATTACK") : 					return modaudio::ModuleEnvelopeADSR::PARAM_ATTACK;
				case HASH("DECAY") : 						return modaudio::ModuleEnvelopeADSR::PARAM_DECAY;
				case HASH("SUSTAIN") : 					return modaudio::ModuleEnvelopeADSR::PARAM_SUSTAIN;
				case HASH("RELEASE") : 					return modaudio::ModuleEnvelopeADSR::PARAM_RELEASE;
				case HASH("GATE_RELEASE") : 		return modaudio::ModuleEnvelopeADSR::PARAM_GATE_RELEASE;
				case HASH("GATE") : 						return modaudio::ModuleEnvelopeADSR::PARAM_GATE;
				case HASH("GATE_THRESHOLD") : 	return modaudio::ModuleEnvelopeADSR::PARAM_GATE_THRESHOLD;
			}
			break;

		case modaudio::ModuleType::NOISE :
			switch(param)
			{
				case HASH("FREQUENCY") : 				return modaudio::ModuleNoise::PARAM_CARRIER_FREQUENCY;
			}
			break;

		case modaudio::ModuleType::OSCILLATOR :
			switch(param)
			{
				case HASH("FREQUENCY") :				return modaudio::ModuleOscillator::PARAM_CARRIER_FREQUENCY;
				case HASH("CV2") :							return modaudio::ModuleOscillator::PARAM_CV2;
			}

		case modaudio::ModuleType::OSCILLATOR_SINE :
			switch(param)
			{
				case HASH("FREQUENCY") :				return modaudio::ModuleOscillatorSine::PARAM_CARRIER_FREQUENCY;
			}

		case modaudio::ModuleType::OSCILLATOR_TRIANGLE :
			switch(param)
			{
				case HASH("FREQUENCY") :				return modaudio::ModuleOscillatorTriangle::PARAM_CARRIER_FREQUENCY;
			}

		case modaudio::ModuleType::SINGLE_POLE_LOW_PASS_FILTER :
			switch(param)
			{
				case HASH("FILTER") :						return modaudio::ModuleSinglePoleLPF::PARAM_FILTER;
			}
	}

	return -1;
}

int
get_module_input_type(modaudio::ModuleType module, const std::string & input_name)
{
	auto input = ade::hash::hash_ascii_string_as_lower(input_name.c_str());

	switch(module)
	{
		case modaudio::ModuleType::AMPLIFIER :
			switch(input)
			{
				case HASH("BIAS") : 						return modaudio::ModuleAmplifier::INPUT_BIAS; 
				case HASH("CV") : 							return modaudio::ModuleAmplifier::INPUT_CV; 
				case HASH("IN") : 							return modaudio::ModuleAmplifier::INPUT_IN; 
			}
			break;

		case modaudio::ModuleType::ENVELOPE_ADSR :
			switch(input)
			{
				case HASH("GATE") : 								return modaudio::ModuleEnvelopeADSR::INPUT_GATE; 
			}
			break;

		case modaudio::ModuleType::MIXER2 :
			switch(input)
			{
				case HASH("IN1") : 								return modaudio::ModuleMixer2::INPUT_IN1; 
				case HASH("IN2") : 								return modaudio::ModuleMixer2::INPUT_IN2; 
			}
			break;

		case modaudio::ModuleType::NOISE :
			switch(input)
			{
				case HASH("CV") : 								return modaudio::ModuleNoise::INPUT_CV; 
			}
			break;

		case modaudio::ModuleType::OSCILLATOR :
			switch(input)
			{
				case HASH("CV1") : 								return modaudio::ModuleOscillator::INPUT_CV1;
				case HASH("CV2") : 								return modaudio::ModuleOscillator::INPUT_CV2;
				case HASH("PULSE_WIDTH") : 				return modaudio::ModuleOscillator::INPUT_PULSE_WIDTH;
			}
			break;

		case modaudio::ModuleType::SINGLE_POLE_LOW_PASS_FILTER :
			switch(input)
			{
				case HASH("FILTER") : 						return modaudio::ModuleSinglePoleLPF::INPUT_FILTER;
				case HASH("IN") : 								return modaudio::ModuleSinglePoleLPF::INPUT_IN;
			}
			break;
	}

	return -1;
}


} // namespace gap::assets::sound
