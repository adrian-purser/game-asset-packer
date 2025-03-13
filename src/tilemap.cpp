//=============================================================================
//	FILE:						tilemap.cpp
//	SYSTEM:				 	game asset packer
//	DESCRIPTION:
//-----------------------------------------------------------------------------
//	COPYRIGHT:			(C)Copyright 2025 Adrian Purser. All Rights Reserved.
//	LICENCE:				MIT
//	MAINTAINER:			AJP - Adrian Purser <ade@arcadestuff.com>
//	CREATED:				12-MAR-2025 Adrian Purser <ade@arcadestuff.com>
//=============================================================================
#include <iostream>
#include <fstream>
#include <span>
#include <vector>
#include "tilemap.h"
#include "adexml/xml_parser.h"
#include "utility/unicode.h"

namespace gap::tilemap
{

#define TAG "TILEMAP: "

//=============================================================================
//
//	TILEMAP LAYER
//
//=============================================================================

/*
	uint32_t								m_id = 0;
	uint32_t								m_width = 0;
	uint32_t								m_height = 0;
	std::string							m_name;
	std::vector<uint32_t>		m_data;
*/

SourceTileMapLayer::SourceTileMapLayer(uint32_t id, std::string_view name, uint32_t width, uint32_t height)
	: m_id(id)
	, m_width(width)
	, m_height(height)
	, m_name(name)
{
	m_data.resize(width * height, 0);
}


void
SourceTileMapLayer::set_tile(std::size_t index, uint32_t value)
{
	if(index < m_data.size())
		m_data[index] = value;
}

void
SourceTileMapLayer::set_tile(uint32_t x, uint32_t y, uint32_t value)
{
	set_tile((y*m_width) + x, value);
}


//=============================================================================
//
//	TILEMAP
//
//=============================================================================

void
SourceTileMap::enumerate_layers(std::function<bool(const SourceTileMapLayer &)> callback)
{
	for(const auto & p_layer : m_layers)
		if(p_layer != nullptr)
			if(!callback(*p_layer.get()))
				break;
}

//=============================================================================
//
//	TILEMAP LOADING
//
//=============================================================================

static std::string
to_string(const std::u8string & u8str)
{
	std::string str;
	ade::unicode::U8Parser parser;

	for(auto ch : u8str)
	{
		auto optch = parser.put(ch);
		if(optch.has_value())
		{
			auto value = optch.value();
			str.push_back(value > 127 ? '?' : static_cast<char>(value & 0xFF));
		}
	}
	return str;
}

static 
std::error_code
on_start_tag( SourceTileMap & 											tilemap,
							const std::u8string &									path,
							const std::vector<adexml::Element> & 	stack )
{
	assert(!stack.empty());
	auto & element = stack.back();
	if(element.type != adexml::ElementType::ELEMENT)
		return {};

	std::cout << "END:   " << to_string(path) << std::endl;

	return {};
}

static 
std::error_code
on_end_tag( SourceTileMap & 												tilemap,
						const std::u8string &										path,
						const std::vector<adexml::Element> & 		stack )
{
	(void)path;
	assert(!stack.empty());
	auto & element = stack.back();
	if(element.type != adexml::ElementType::ELEMENT)
		return {};

	std::cout << "START: " << to_string(path) << std::endl;

	return {};
}

std::unique_ptr<SourceTileMap>
load_tiled_tmx(const std::string & filename, gap::FileSystem & filesystem)
{
	auto file = filesystem.load(filename);
	if(file.empty())
	{
		std::cerr << "LOADIMAGE: Failed to load image '" << filename << "'\n";
		return nullptr;
	}

	//---------------------------------------------------------------------------
	// XML Parser
	//---------------------------------------------------------------------------
	auto p_tilemap = std::make_unique<SourceTileMap>();
	auto & tilemap = *(p_tilemap.get());

	adexml::Parser parser([&](	adexml::Parser::Action 								action,
															const std::u8string &									path,
															const std::vector<adexml::Element> & 	stack ) -> std::error_code
															{
																switch(action)
																{
																	case adexml::Parser::ACTION_START_ELEMENT :	return on_start_tag(tilemap, path, stack); break;
																	case adexml::Parser::ACTION_END_ELEMENT :		return on_end_tag(tilemap, path, stack); break;
																	case adexml::Parser::ACTION_PI :						break; //std::cout << "PI:    "; print_element_path(std::cout, stack); std::cout.put('\n'); break;
																	default : 																	std::cout << "UNKNOWN ACTION!\n"; return std::make_error_code(std::errc::invalid_argument);
																};
																return {};
															});

	std::span<char8_t> filedata(reinterpret_cast<char8_t *>(file.data()), file.size());

	auto ec = parser.write(filedata);
	if(ec)
	{
		std::cerr << TAG "(xmlparser): " << ec.message() << '\n';
		return nullptr;
	}

	return p_tilemap;
}


std::unique_ptr<SourceTileMap>
load(const std::string & filename, const std::string & type, gap::FileSystem & filesystem)
{
	auto ltype(type);
	std::transform(begin(ltype), end(ltype), begin(ltype), ::tolower);

	if(type == "tiled:tmx")			return load_tiled_tmx(filename, filesystem);

	return nullptr;
}

} // namespace gap::tilemap

