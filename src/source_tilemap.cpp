//=============================================================================
//	FILE:						source_tilemap.cpp
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
#include <print>
#include <format>
#include "source_tilemap.h"
#include "adexml/xml_parser.h"
#include "utility/unicode.h"
#include "utility/tokenize.h"
#include "utility/ansi.h"

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
SourceTileMapLayer::set_tile(std::size_t index, uint64_t value)
{
	if(index < m_data.size())
		m_data[index] = value;
}

void
SourceTileMapLayer::set_tile(uint32_t x, uint32_t y, uint64_t value)
{
	set_tile((y*m_width) + x, value);
}

uint64_t
SourceTileMapLayer::get_tile(uint32_t x, uint32_t y) const
{
	uint32_t index = (y*m_width) + x;
	if(index < m_data.size())
		return m_data[index];
	return 0U;
}

//=============================================================================
//
//	TILEMAP
//
//=============================================================================

void
SourceTileMap::enumerate_layers(std::function<bool(const SourceTileMapLayer &)> callback) const 
{
	for(const auto & p_layer : m_layers)
		if(p_layer != nullptr)
			if(!callback(*p_layer.get()))
				break;
}

const SourceTileMapLayer * 	
SourceTileMap::get_layer(uint32_t id)
{
	for(const auto & p_layer : m_layers)
		if(p_layer != nullptr)
			if(p_layer->m_id == id)
				return p_layer.get();
	return nullptr;
}

//=============================================================================
//
//	HELPER FUNCTIONS
//
//=============================================================================
/*
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
*/

//=============================================================================
//
//	TMX TILEMAP LOADING
//
//=============================================================================

static 
std::error_code
tmx_on_end_tag_map_layer_data( 	SourceTileMap & 												tilemap,
														const std::u8string &										path,
														const std::vector<adexml::Element> & 		stack )
{
	(void)path;

	auto tileset = tilemap.get_tileset();

	//---------------------------------------------------------------------------
	//	PARSE LAYER ELEMENT
	//---------------------------------------------------------------------------
	const auto & layer_element = stack[1];
	
	auto opt_id 			= layer_element.attribute(u8"id");
	auto opt_name 		= layer_element.attribute(u8"name");
	auto opt_width 		= layer_element.attribute(u8"width");
	auto opt_height 	= layer_element.attribute(u8"height");
	auto opt_x 				= layer_element.attribute(u8"x");
	auto opt_y 				= layer_element.attribute(u8"y");

	if(!opt_width || !opt_height)
		return std::make_error_code(std::errc::invalid_argument);

	uint32_t 			id 			= opt_id ? std::strtoul(ade::unicode::to_string(opt_id.value()).c_str(), nullptr, 10) : 0UL;
	std::string		name 		= opt_name ? ade::unicode::to_string(opt_name.value()) : "";
	uint32_t 			width		= std::strtoul(ade::unicode::to_string(opt_width.value()).c_str(), nullptr, 10);
	uint32_t 			height	= std::strtoul(ade::unicode::to_string(opt_height.value()).c_str(), nullptr, 10);
	uint32_t 			x				= opt_x ? std::strtoul(ade::unicode::to_string(opt_x.value()).c_str(), nullptr, 10) : 0UL;
	uint32_t 			y				= opt_y ? std::strtoul(ade::unicode::to_string(opt_y.value()).c_str(), nullptr, 10) : 0UL;


	auto p_layer = std::make_shared<SourceTileMapLayer>(id, name, width, height);
	auto & layer = *p_layer.get();

	layer.set_position(x,y);

	//---------------------------------------------------------------------------
	//	PARSE DATA ELEMENT
	//---------------------------------------------------------------------------
	const auto & data_element = stack[2];

	auto opt_encoding 		= data_element.attribute(u8"encoding");
	auto opt_compression	= data_element.attribute(u8"compression");

	std::string encoding("csv");
	std::string compression;

	if(opt_encoding)			encoding 		= ade::unicode::to_string(opt_encoding.value());
	if(opt_compression)		compression = ade::unicode::to_string(opt_compression.value());

	std::transform(begin(encoding), end(encoding), begin(encoding), ::tolower);
	std::transform(begin(compression), end(compression), begin(compression), ::tolower);

	if(encoding == "csv")
	{
		auto csv = ade::unicode::to_string(data_element.content);
		std::replace(begin(csv), end(csv), '\n', ' ');
		std::replace(begin(csv), end(csv), '\r', ' ');
		auto values = ade::tokenize(csv);
		if(values.size() != (width * height))
			return std::make_error_code(std::errc::invalid_argument);

		int i=0;
		for(auto val : values)
		{
			// Tile values are offset by the 'firstgid' attribute of the tileset
			// so we need to correct for that. 
			auto tile = strtoul(std::string(val).c_str(), nullptr, 10);
			layer.set_tile(i++,tile == 0 ? 0 : tile-tileset.first_tile_id);
		}
	}
	else
		return std::make_error_code(std::errc::protocol_not_supported);

	tilemap.add_layer(p_layer);

	return {};
}

static 
std::error_code
tmx_on_start_tag_map( SourceTileMap & 											tilemap,
											const std::u8string &									path,
											const std::vector<adexml::Element> & 	stack )
{
	(void)path;
	const auto & element = stack[0];

	auto opt_orientation		= element.attribute(u8"orientation");
	auto opt_renderorder		= element.attribute(u8"renderorder");
	auto opt_width					= element.attribute(u8"width");
	auto opt_height					= element.attribute(u8"height");
	auto opt_tilewidth			= element.attribute(u8"tilewidth");
	auto opt_tileheight			= element.attribute(u8"tileheight");
	auto opt_infinite				= element.attribute(u8"infinite");

	if(opt_orientation && opt_orientation.value() != u8"orthogonal")
		return std::make_error_code(std::errc::protocol_not_supported);
	if(opt_renderorder && opt_renderorder.value() != u8"right-down")
		return std::make_error_code(std::errc::protocol_not_supported);

	uint32_t 	width 				= opt_width ? std::strtoul(ade::unicode::to_string(opt_width.value()).c_str(), nullptr, 10) : 0UL;
	uint32_t 	height 				= opt_height ? std::strtoul(ade::unicode::to_string(opt_height.value()).c_str(), nullptr, 10) : 0UL;
	uint32_t 	tile_width 		= opt_tilewidth ? std::strtoul(ade::unicode::to_string(opt_tilewidth.value()).c_str(), nullptr, 10) : 0UL;
	uint32_t 	tile_height 	= opt_tileheight ? std::strtoul(ade::unicode::to_string(opt_tileheight.value()).c_str(), nullptr, 10) : 0UL;
	uint32_t 	infinite			= opt_infinite ? std::strtoul(ade::unicode::to_string(opt_infinite.value()).c_str(), nullptr, 10) : 0UL;

	if(infinite > 0)
		return std::make_error_code(std::errc::protocol_not_supported);

	tilemap.set_dimensions(width, height);
	tilemap.set_tilesize(tile_width, tile_height);

	return {};
}

static 
std::error_code
tmx_on_start_tag( SourceTileMap & 											tilemap,
									const std::u8string &									path,
									const std::vector<adexml::Element> & 	stack )
{
	assert(!stack.empty());
	auto & element = stack.back();
	if(element.type != adexml::ElementType::ELEMENT)
		return {};

//	std::cout << "START: " << ade::unicode::to_string(path) << std::endl;

	if(path == u8"map")
		return tmx_on_start_tag_map(tilemap, path, stack);
	if(path == u8"map/tileset")
	{
		SourceTileSet tileset;
		auto opt_source		= element.attribute(u8"source");
		auto opt_firstgid	= element.attribute(u8"firstgid");
	
		tileset.sourcefile		= opt_source ? ade::unicode::to_string(opt_source.value()) : "";
		tileset.sourcetype		= "tiled:tsx";
		tileset.first_tile_id	= opt_firstgid ? std::strtoul(ade::unicode::to_string(opt_firstgid.value()).c_str(), nullptr, 10) : 0UL;
		tilemap.set_tileset(tileset);
	}

	return {};
}

static 
std::error_code
tmx_on_end_tag( SourceTileMap & 												tilemap,
								const std::u8string &										path,
								const std::vector<adexml::Element> & 		stack )
{
	(void)path;
	assert(!stack.empty());
	auto & element = stack.back();
	if(element.type != adexml::ElementType::ELEMENT)
		return {};

//	std::cout << "END:   " << ade::unicode::to_string(path) << std::endl;

	if((path == u8"map/layer/data") && (stack.size() >= 3))
		return tmx_on_end_tag_map_layer_data(tilemap, path, stack);

	return {};
}

std::unique_ptr<SourceTileMap>
load_tiled_tmx(const std::string & filename, gap::FileSystem & filesystem)
{
	auto file = filesystem.load(filename);
	if(file.empty())
	{
		std::cerr << "LOADTILEMAP: Failed to load tilemap '" << filename << "'\n";
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
																	case adexml::Parser::ACTION_START_ELEMENT :	return tmx_on_start_tag(tilemap, path, stack); break;
																	case adexml::Parser::ACTION_END_ELEMENT :		return tmx_on_end_tag(tilemap, path, stack); break;
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





//=============================================================================
//
//	TILEMAP LOADING
//
//=============================================================================

std::unique_ptr<SourceTileMap>
load(const std::string & filename, const std::string & type, gap::FileSystem & filesystem)
{
	std::unique_ptr<SourceTileMap> p_tilemap;

	auto ltype(type);
	std::transform(begin(ltype), end(ltype), begin(ltype), ::tolower);

	if(type == "tiled:tmx")			p_tilemap = load_tiled_tmx(filename, filesystem);

	if(p_tilemap)
	{
		print_tilemap(*p_tilemap);
	}
	return p_tilemap;
}


//=============================================================================
//
//	TILEMAP DEBUG
//
//=============================================================================

void
print_tilemap(const SourceTileMap & tilemap)
{
	printf("Tilemap\n------------------------------\n\n");
	
	printf(FOREGROUND_LIGHT_BLUE "Dimensions: " FOREGROUND_GREY "%d x %d\n", tilemap.width(), tilemap.height());
	printf(FOREGROUND_CYAN "\nLayers\n\n");

	tilemap.enumerate_layers([&](const SourceTileMapLayer & layer)->bool
		{
			std::print(FOREGROUND_LIGHT_BLUE "ID       : " FOREGROUND_GREY "{}\n", layer.m_id);
			std::print(FOREGROUND_LIGHT_BLUE "Name     : " FOREGROUND_GREY "{}\n", layer.m_name);
			std::print(FOREGROUND_LIGHT_BLUE "position : " FOREGROUND_GREY "x:{}, y:{}\n", layer.m_x, layer.m_y);
			std::print(FOREGROUND_LIGHT_BLUE "size     : " FOREGROUND_GREY "{} x {}\n", layer.m_width, layer.m_height);

			if((layer.m_x * layer.m_y) <= layer.m_data.size())
			{
				int i = 0;
				for(uint32_t y = 0; y < layer.m_height; ++y)
				{
					for(uint32_t x = 0; x < layer.m_width; ++x)
					{
						auto tile = layer.m_data[i++];
						ansi::stdio::background_colour(ansi::sgr::FG_BLACK + std::min<int>(tile,9));
						//printf("\033[%dm",ansi::sgr::BG_BLACK + std::min<int>(tile,9));
						printf("%c", tile == 0 ? '.' : (tile > 9 ? '#' : '0' + (char)(tile&0x7F)));
					}
					ansi::stdio::background_colour(ansi::sgr::FG_BLACK);
					printf("\n");
				}
			}
			printf("\n\n");
			return true;	
		});

	printf("\n------------------------------\n\n");

}



} // namespace gap::tilemap

