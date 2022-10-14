//=============================================================================
//	FILE:						parse_gap.cpp
//	SYSTEM:				 	game asset packer
//	DESCRIPTION:
//-----------------------------------------------------------------------------
//	COPYRIGHT:			(C)Copyright 2019 Adrian Purser. All Rights Reserved.
//	LICENCE:				MIT
//	MAINTAINER:			AJP - Adrian Purser <ade@arcadestuff.com>
//	CREATED:				25-SEP-2019 Adrian Purser <ade@arcadestuff.com>
//=============================================================================

#include <iostream>
#include <regex>
#include <string>
#include <string_view>
#include <algorithm>
#include <fmt/format.h>
#include "parse_gap.h"
#include "parse_colour_map.h"
#include "utility/hash.h"

#define GAPCMD_LOADIMAGE				"loadimage"
#define GAPCMD_IMAGE						"image"
#define GAPCMD_IMAGEGROUP				"imagegroup"
#define GAPCMD_IMAGEARRAY				"imagearray"
#define GAPCMD_TILESET					"tileset"
#define GAPCMD_TILE							"tile"
#define GAPCMD_TILEARRAY				"tilearray"
#define GAPCMD_EXPORT						"export"
#define GAPCMD_FILE							"file"
#define GAPCMD_COLOURMAP				"colourmap"

using namespace std::literals::string_literals;

namespace gap
{


ParserGAP::ParserGAP(gap::FileSystem & filesystem)
 : m_filesystem(filesystem)
 {
 }

std::unique_ptr<gap::assets::Assets>		
ParserGAP::parse(std::string_view source)
{
	m_p_assets = std::make_unique<gap::assets::Assets>();

	std::string_view::size_type linestart = 0;
	int linenumber = 1;
	bool b_error = false;

	while((linestart != std::string_view::npos) && !b_error)
	{
		auto linebreak = source.find('\n',linestart);
		auto line = (linebreak == std::string_view::npos) ? source.substr(linestart) : source.substr(linestart,linebreak-linestart);

		if(parse_line(line,linenumber))
		{
			b_error = true;
			continue;
		}

		if(linebreak == std::string_view::npos)
			break;

		linestart = linebreak+1;
		++linenumber;
	}

	if(b_error)
		return nullptr;

	return std::move(m_p_assets);
}


int
ParserGAP::parse_line(std::string_view line,int line_number)
{
	//---------------------------------------------------------------------------
	//	Tokenise the line string
	//---------------------------------------------------------------------------
	if(line.empty())
		return 0;

	gap::CommandLine	cmd;
	int 							iarg = 0;
	std::string 			token;
	bool							b_quotes = false;

	auto parse_arg = 	[&](int line_number,const std::string & token)->std::pair<std::string,std::string>
										{
											auto pos = token.find('=');
											if(pos == std::string::npos)
											{
												on_error(line_number,"Missing value for '" + token + "'!");
												return {};
											}
											auto key = token.substr(0,pos);
											auto val = token.substr(pos+1);
											if(key.empty() || val.empty())
											{
												on_error(line_number,"Syntax Error!");
												return {};
											}
											return {key,val};
										};

	for(auto ch : line)
	{
		bool b_finished = false;

		switch(ch)
		{
			case ' ' :
			case '\t' :
				if(!token.empty())
					token.push_back(ch);
				break;
				
			case '#' :
				b_finished = true;
				break;

			case ',' :
				if(!b_quotes)
				{
					if(iarg == 0)
					{
						if(token.empty())
							return on_error(line_number,"Missing command!");
						cmd.command = token;
					}
					else if(!token.empty())
					{
						auto arg = parse_arg(line_number,token);
						if(arg.first.empty())
							return -1;
						cmd.args[arg.first] = arg.second;
					}
					++iarg;
					token.clear();
				}
				else
					token.push_back(ch);
				break;

			case '"' :
				b_quotes = !b_quotes;
				break;

			case 8 :
			case 10 :
			case 13 :
				break;
			
			default :
				token.push_back(ch);
				break;
		}
	
		if(b_finished)
			break;
	}

		
	if(!token.empty())
	{
		if(iarg == 0)
		{
			if(token.empty())
				return on_error(line_number,"Missing command!");
			cmd.command = token;
		}
		else if(!token.empty())
		{
			auto arg = parse_arg(line_number,token);
			if(arg.first.empty())
				return -1;
			cmd.args[arg.first] = arg.second;
		}
	}

	if(cmd.command.empty())
		return 0;

	//---------------------------------------------------------------------------
	// Process the command
	//---------------------------------------------------------------------------
	
	int result = 0;

	auto hash = ade::hash::hash_ascii_string_as_lower(cmd.command.c_str(),cmd.command.size());

	switch(hash)
	{
		case ade::hash::hash_ascii_string_as_lower(GAPCMD_LOADIMAGE) :	result = command_loadimage(line_number,cmd); 		break;
		case ade::hash::hash_ascii_string_as_lower(GAPCMD_IMAGEGROUP) :	result = command_imagegroup(line_number,cmd); 	break;
		case ade::hash::hash_ascii_string_as_lower(GAPCMD_IMAGE) :			result = command_image(line_number,cmd); 				break;
		case ade::hash::hash_ascii_string_as_lower(GAPCMD_IMAGEARRAY) :	result = command_imagearray(line_number,cmd); 	break;
		case ade::hash::hash_ascii_string_as_lower(GAPCMD_TILESET) :		result = command_tileset(line_number,cmd); 			break;
		case ade::hash::hash_ascii_string_as_lower(GAPCMD_TILE) :				result = command_tile(line_number,cmd); 				break;
		case ade::hash::hash_ascii_string_as_lower(GAPCMD_TILEARRAY) :	result = command_tilearray(line_number,cmd); 		break;
		case ade::hash::hash_ascii_string_as_lower(GAPCMD_EXPORT) :			result = command_export(line_number,cmd); 			break;
		case ade::hash::hash_ascii_string_as_lower(GAPCMD_FILE) :				result = command_file(line_number,cmd); 				break;
		case ade::hash::hash_ascii_string_as_lower(GAPCMD_COLOURMAP) :	result = command_colourmap(line_number,cmd); 		break;

		default : 
			std::cerr << "GAP: Unknown command '" << cmd.command << "'\n";
			result = -1;
			break;
	}

	return result;
}

void
ParserGAP::enumerate_exports(std::function<bool(const gap::exporter::ExportInfo &)> callback)
{
	for(const auto & exportinfo : m_export_info)
		if(!callback(exportinfo))
			break;
}

int 
ParserGAP::command_loadimage(int line_number, const CommandLine & command)
{
	
	std::string src;
	std::string format;

	for(const auto & [key,value] : command.args)
	{
		auto hash = ade::hash::hash_ascii_string_as_lower(key.c_str(),key.size());
		switch(hash)
		{
			case ade::hash::hash_ascii_string_as_lower("src") 		:	src 		= value; break;
			case ade::hash::hash_ascii_string_as_lower("format") 	:	format 	= value; break;
			default : 
				// TODO: Warning - unknown arg
				break;
		}
	}

	if(src.empty())
		return on_error(line_number,"Missing image path!");

	auto p_image = gap::image::load(src,m_filesystem);
	if(p_image == nullptr)
		return on_error(line_number,std::string("Failed to load image! - ") + src);

	if(!format.empty())
	{
		auto pf = gap::image::parse_pixelformat_name(format);
		if(pf == 0)
			return on_error(line_number,std::string("Unknown Pixel Format! - ") + format);
		p_image->set_target_pixelformat(pf);
	}

	m_current_source_image = m_p_assets->add_source_image(std::move(p_image));

	std::cout << "  Image added into slot " << m_current_source_image << std::endl;
	
	return 0;
}

int 
ParserGAP::command_imagegroup(int /*line_number*/, const CommandLine & command)
{

	int 					group = m_current_image_group + 1;
	int 					base 	= 0;
	std::string		name;

	for(const auto & [key,value] : command.args)
	{
		auto hash = ade::hash::hash_ascii_string_as_lower(key.c_str(),key.size());
		
		switch(hash)
		{
			case ade::hash::hash_ascii_string_as_lower("group") 	:	group	= std::strtol(value.c_str(),nullptr,10); 	break;
			case ade::hash::hash_ascii_string_as_lower("base") 		:	base	= std::strtol(value.c_str(),nullptr,10); 	break;
			case ade::hash::hash_ascii_string_as_lower("name") 		:	name	= value; 	break;

			default : 
				// TODO: Warning - unknown arg
				break;
		}
	}
	
	if(group >= 0)
	{
		m_current_image_group = group;
		if(base > 0)
			m_p_assets->set_group_base(group,base);
		if(!name.empty())
			m_p_assets->set_group_name(group,name);
	}
	return 0;
}

int 
ParserGAP::command_image(int /*line_number*/, const CommandLine & command)
{
	//TODO: Add 'scale' parameters

	gap::image::Image	image;

	bool 		b_width 						= false;
	bool 		b_height						= false;
	bool		b_have_angle				= false;
	bool		b_have_angle_from		= false;
	bool		b_have_angle_to			= false;

	int 		count 							= 1;
	float		angle_step					= 0.0f;
	float		angle_from					= 0.0f;
	float		angle_to						= 0.0f;



	for(const auto & [key,value] : command.args)
	{
		auto hash = ade::hash::hash_ascii_string_as_lower(key.c_str(),key.size());
		switch(hash)
		{
			case ade::hash::hash_ascii_string_as_lower("x") 					:	image.x 				= std::strtol(value.c_str(),nullptr,10); 	break;
			case ade::hash::hash_ascii_string_as_lower("y") 					:	image.y 				= std::strtol(value.c_str(),nullptr,10); 	break;
			case ade::hash::hash_ascii_string_as_lower("w") 					:	
			case ade::hash::hash_ascii_string_as_lower("width") 			:	image.width			= std::strtol(value.c_str(),nullptr,10); 	b_width = true; break;
			case ade::hash::hash_ascii_string_as_lower("h") 					:	
			case ade::hash::hash_ascii_string_as_lower("height")			:	image.height		= std::strtol(value.c_str(),nullptr,10); 	b_height = true; break;
			case ade::hash::hash_ascii_string_as_lower("xo") 					:	
			case ade::hash::hash_ascii_string_as_lower("xorigin")			:	image.x_origin	= std::strtol(value.c_str(),nullptr,10); 	break;
			case ade::hash::hash_ascii_string_as_lower("yo") 					:	
			case ade::hash::hash_ascii_string_as_lower("yorigin")			:	image.y_origin	= std::strtol(value.c_str(),nullptr,10); 	break;

			case ade::hash::hash_ascii_string_as_lower("hf") 					:	
			case ade::hash::hash_ascii_string_as_lower("hflip")				:	image.b_hflip		= !!std::strtol(value.c_str(),nullptr,10);	break;

			case ade::hash::hash_ascii_string_as_lower("vf") 					:	
			case ade::hash::hash_ascii_string_as_lower("vflip")				:	image.b_vflip		= !!std::strtol(value.c_str(),nullptr,10);	break;

			case ade::hash::hash_ascii_string_as_lower("angle") 			:	
			case ade::hash::hash_ascii_string_as_lower("rotate")			:	image.angle			= std::strtof(value.c_str(),nullptr); b_have_angle = true;	break;

			case ade::hash::hash_ascii_string_as_lower("angle-step") 	:	
			case ade::hash::hash_ascii_string_as_lower("rotate-step")	:	angle_step 			= std::strtof(value.c_str(),nullptr); break;

			case ade::hash::hash_ascii_string_as_lower("angle-from") 	:	
			case ade::hash::hash_ascii_string_as_lower("rotate-from")	:	angle_from			= std::strtof(value.c_str(),nullptr); b_have_angle_from=true;	break;

			case ade::hash::hash_ascii_string_as_lower("angle-to") 		:	
			case ade::hash::hash_ascii_string_as_lower("rotate-to")		:	angle_to 				= std::strtof(value.c_str(),nullptr); b_have_angle_to=true;	break;

			case ade::hash::hash_ascii_string_as_lower("pf") 					:	
			case ade::hash::hash_ascii_string_as_lower("format")			:	image.pixel_format = gap::image::parse_pixelformat_name(value); break;
			case ade::hash::hash_ascii_string_as_lower("name") 				:	image.name 			= value; break;

			case ade::hash::hash_ascii_string_as_lower("count")				: count						=	std::strtol(value.c_str(),nullptr,10); 	break;

			default : 
				// TODO: Warning - unknown arg
				break;
		}
	}

	//---------------------------------------------------------------------------
	//	VALIDATE AND UPDATE PARAMETERS
	//---------------------------------------------------------------------------

	if(!b_width && (image.width == 0))
		image.width = m_p_assets->source_image_width(m_current_source_image) - image.x;

	if(!b_height && (image.height == 0))
		image.height = m_p_assets->source_image_height(m_current_source_image) - image.y;

	image.source_image	= m_current_source_image;

	if(image.pixel_format == 0)
		image.pixel_format = m_p_assets->get_target_pixelformat(m_current_source_image);
		
	if((image.width <= 0) || (image.height <= 0))
		return 0;
	
	if(!b_have_angle && b_have_angle_from)
			image.angle = angle_from;

	//---------------------------------------------------------------------------
	//	CREATE IMAGE/S
	//---------------------------------------------------------------------------
	if(count == 1)
		m_p_assets->add_image(m_current_image_group,image);
	else
	{
		std::string name = image.name;

		// Calculate angle step
		if(b_have_angle_from && b_have_angle_to)
		{
			image.angle = angle_from;
			angle_step	= (angle_to - angle_from) / (float)count;
		}

		for(int i=0;i<count;++i)
		{
			if(!name.empty())
				image.name = fmt::format("{}-{:03}",name,i);			
			m_p_assets->add_image(m_current_image_group,image);
			image.angle += angle_step;
		}
	}

	return 0;
}

int 
ParserGAP::command_imagearray(int line_number, const CommandLine & command)
{
	int 				x 			= 0;
	int 				y 			= 0;
	int 				width 	= 0;
	int 				height 	= 0;
	int 				xcount 	= 0;
	int 				ycount 	= 0;
	int 				xorigin	= 0;
	int 				yorigin	= 0;
	bool				hflip		= false;
	bool				vflip		= false;
	uint8_t 		format 	= 0;
	std::string	name;

	for(const auto & [key,value] : command.args)
	{
		auto hash = ade::hash::hash_ascii_string_as_lower(key.c_str(),key.size());
		switch(hash)
		{
			case ade::hash::hash_ascii_string_as_lower("x") 			:	x 				= std::strtol(value.c_str(),nullptr,10); 	break;
			case ade::hash::hash_ascii_string_as_lower("y") 			:	y 				= std::strtol(value.c_str(),nullptr,10); 	break;
			case ade::hash::hash_ascii_string_as_lower("w") 			:	
			case ade::hash::hash_ascii_string_as_lower("width") 	:	width			= std::strtol(value.c_str(),nullptr,10); 	break;
			case ade::hash::hash_ascii_string_as_lower("h") 			:	
			case ade::hash::hash_ascii_string_as_lower("height")	:	height		= std::strtol(value.c_str(),nullptr,10); 	break;
			case ade::hash::hash_ascii_string_as_lower("xc") 			:	
			case ade::hash::hash_ascii_string_as_lower("xcount")	:	xcount		= std::strtol(value.c_str(),nullptr,10); 	break;
			case ade::hash::hash_ascii_string_as_lower("yc") 			:	
			case ade::hash::hash_ascii_string_as_lower("ycount")	:	ycount		= std::strtol(value.c_str(),nullptr,10); 	break;
			case ade::hash::hash_ascii_string_as_lower("xo") 			:	
			case ade::hash::hash_ascii_string_as_lower("xorigin")	:	xorigin		= std::strtol(value.c_str(),nullptr,10); 	break;
			case ade::hash::hash_ascii_string_as_lower("yo") 			:	
			case ade::hash::hash_ascii_string_as_lower("yorigin")	:	yorigin		= std::strtol(value.c_str(),nullptr,10); 	break;
			case ade::hash::hash_ascii_string_as_lower("pf") 			:	
			case ade::hash::hash_ascii_string_as_lower("format")	:	format 		= gap::image::parse_pixelformat_name(value); break;
			case ade::hash::hash_ascii_string_as_lower("hf") 			:	
			case ade::hash::hash_ascii_string_as_lower("hmirror") :	
			case ade::hash::hash_ascii_string_as_lower("hflip")		:	hflip			= !!std::strtol(value.c_str(),nullptr,10);	break;
			case ade::hash::hash_ascii_string_as_lower("vf") 			:	
			case ade::hash::hash_ascii_string_as_lower("vmirror") :	
			case ade::hash::hash_ascii_string_as_lower("vflip")		:	vflip			= !!std::strtol(value.c_str(),nullptr,10);	break;
			case ade::hash::hash_ascii_string_as_lower("name") 		:	name 			= value; break;
			default : 
				// TODO: Warning - unknown arg
				break;
		}
	}

	if(xcount <= 0) 	return on_error(line_number,std::string("Invalid/Missing 'xcount' parameter!"));
	if(ycount <= 0) 	return on_error(line_number,std::string("Invalid/Missing 'ycount' parameter!"));
	if(width <= 0) 		return on_error(line_number,std::string("Invalid/Missing 'width' parameter!"));
	if(height <= 0) 	return on_error(line_number,std::string("Invalid/Missing 'height' parameter!"));

	for(int yi = 0;yi < ycount;++yi)
	{
		for(int xi = 0;xi < xcount;++xi)
		{
			gap::image::Image	image;
			image.x 						= x + (xi * width);
			image.y 						= y + (yi * height);
			image.x_origin			= xorigin;
			image.y_origin			= yorigin;
			image.width					= width;
			image.height 				= height;
			image.pixel_format	= format;
			image.b_hflip				= hflip;
			image.b_vflip				= vflip;
			image.source_image	= m_current_source_image;
			if(!name.empty())
				image.name = name + '_' + std::to_string(xi) + '_' + std::to_string(yi);
			m_p_assets->add_image(m_current_image_group,image);
		}
	}

	return 0;
}

int 
ParserGAP::command_tileset(int line_number, const CommandLine & command)
{
	gap::tileset::TileSet	tileset;

	for(const auto & [key,value] : command.args)
	{
		auto hash = ade::hash::hash_ascii_string_as_lower(key.c_str(),key.size());
		switch(hash)
		{
			case ade::hash::hash_ascii_string_as_lower("w") 			:	
			case ade::hash::hash_ascii_string_as_lower("width") 	:	tileset.tile_width		= std::strtol(value.c_str(),nullptr,10); 	break;
			case ade::hash::hash_ascii_string_as_lower("h") 			:	
			case ade::hash::hash_ascii_string_as_lower("height")	:	tileset.tile_height		= std::strtol(value.c_str(),nullptr,10); 	break;
			case ade::hash::hash_ascii_string_as_lower("id")			:	tileset.id						= std::strtol(value.c_str(),nullptr,10); 	break;
			case ade::hash::hash_ascii_string_as_lower("pf") 			:	
			case ade::hash::hash_ascii_string_as_lower("format")	:	tileset.pixel_format 	= gap::image::parse_pixelformat_name(value); break;
			case ade::hash::hash_ascii_string_as_lower("name") 		:	tileset.name 					= value; break;
			default : 
				// TODO: Warning - unknown arg
				break;
		}
	}

	if(tileset.id < 0)	return on_error(line_number,std::string("Invalid/Missing 'id' parameter!"));

	// If the width and height are specified then add the tileset, otherwise just make the tileset current.
	if((tileset.tile_width > 0) || (tileset.tile_height > 0))
	{
		if(tileset.tile_width <= 0)			return on_error(line_number,std::string("Invalid/Missing 'width' parameter!"));
		if(tileset.tile_height <= 0)		return on_error(line_number,std::string("Invalid/Missing 'height' parameter!"));

		if(tileset.pixel_format == 0)
			tileset.pixel_format = m_p_assets->get_target_pixelformat(m_current_source_image);
		
		m_p_assets->add_tileset(tileset);
	}
	
	m_current_tileset = tileset.id;

	return 0;
}

int 
ParserGAP::command_tile(int line_number, const CommandLine & command)
{
	if(m_current_tileset < 0)	return on_error(line_number,std::string("No active tileset! The tile command requires an active tileset."));
	
	gap::tileset::Tile tile;

	tile.source_image = m_current_source_image;

	for(const auto & [key,value] : command.args)
	{
		auto hash = ade::hash::hash_ascii_string_as_lower(key.c_str(),key.size());
		switch(hash)
		{
			case ade::hash::hash_ascii_string_as_lower("x") 				:	tile.x		= std::strtol(value.c_str(),nullptr,10); 	break;
			case ade::hash::hash_ascii_string_as_lower("y")					:	tile.y		= std::strtol(value.c_str(),nullptr,10); 	break;
			case ade::hash::hash_ascii_string_as_lower("hflip")			:	tile.transform |= gap::tileset::FLIP_HORZ; 	break;
			case ade::hash::hash_ascii_string_as_lower("vflip")			:	tile.transform |= gap::tileset::FLIP_VERT; 	break;
			case ade::hash::hash_ascii_string_as_lower("rotate")		:
			case ade::hash::hash_ascii_string_as_lower("rotation")	:	
				{
					switch(std::strtol(value.c_str(),nullptr,10))
					{
						case 0 :		break;
						case 90 : 	tile.transform |= gap::tileset::ROTATE_90; break;	
						case 180 : 	tile.transform |= gap::tileset::ROTATE_180; break;	
						case 270 : 	tile.transform |= gap::tileset::ROTATE_270; break;	
						default : 	return on_error(line_number,std::string("Invalid 'rotation' parameter! Must be one of 0, 90, 180 or 270."));
					}
				}
				break;

			default : 
				// TODO: Warning - unknown arg
				break;
		}
	}

	m_p_assets->add_tile(m_current_tileset,tile);

	return 0;
}

int 
ParserGAP::command_tilearray(int line_number, const CommandLine & command)
{
	if(m_current_tileset < 0)	return on_error(line_number,std::string("No active tileset! The tilearray command requires an active tileset."));
	
	gap::tileset::Tile tile;

	//---------------------------------------------------------------------------
	//	Parse Parameters
	//---------------------------------------------------------------------------
	uint32_t x = 0;
	uint32_t y = 0;
	uint32_t tiles_wide = 0;
	uint32_t tiles_high = 0;
	uint32_t transform = 0;

	for(const auto & [key,value] : command.args)
	{
		auto hash = ade::hash::hash_ascii_string_as_lower(key.c_str(),key.size());
		switch(hash)
		{
			case ade::hash::hash_ascii_string_as_lower("x") 					:	x						= std::strtol(value.c_str(),nullptr,10); 	break;
			case ade::hash::hash_ascii_string_as_lower("y")						:	y						= std::strtol(value.c_str(),nullptr,10); 	break;
			case ade::hash::hash_ascii_string_as_lower("width")				:	
			case ade::hash::hash_ascii_string_as_lower("tw")					:	
			case ade::hash::hash_ascii_string_as_lower("tiles-wide")	:	tiles_wide 	= std::strtol(value.c_str(),nullptr,10); 	break;
			case ade::hash::hash_ascii_string_as_lower("height")			:	
			case ade::hash::hash_ascii_string_as_lower("th")					:	
			case ade::hash::hash_ascii_string_as_lower("tiles-high")	:	tiles_high 	= std::strtol(value.c_str(),nullptr,10); 	break;
			case ade::hash::hash_ascii_string_as_lower("hflip")				:	transform |= gap::tileset::FLIP_HORZ; 	break;
			case ade::hash::hash_ascii_string_as_lower("vflip")				:	transform |= gap::tileset::FLIP_VERT; 	break;
			case ade::hash::hash_ascii_string_as_lower("rotate")		:
			case ade::hash::hash_ascii_string_as_lower("rotation")	:	
				{
					switch(std::strtol(value.c_str(),nullptr,10))
					{
						case 0 :		break;
						case 90 : 	transform |= gap::tileset::ROTATE_90; break;	
						case 180 : 	transform |= gap::tileset::ROTATE_180; break;	
						case 270 : 	transform |= gap::tileset::ROTATE_270; break;	
						default : 	return on_error(line_number,std::string("Invalid 'rotation' parameter! Must be one of 0, 90, 180 or 270."));
					}
				}
				break;

			default : 
				// TODO: Warning - unknown arg
				break;
		}
	}

	//---------------------------------------------------------------------------
	//	Generate tiles.
	//---------------------------------------------------------------------------
	const uint32_t tw = m_p_assets->tileset_width(m_current_tileset);
	const uint32_t th = m_p_assets->tileset_height(m_current_tileset);

	if((tw == 0) || (th == 0))	return on_error(line_number,std::string("tilearray: Unable to retrieve the tileset width/height."));

	for(uint32_t v=0;v<tiles_high;++v)
	{
		for(uint32_t h=0;h<tiles_wide;++h)
		{
			gap::tileset::Tile tile;
			tile.x 						= x + (h*tw);
			tile.y 						= y + (v*th);
			tile.transform 		= transform;
			tile.source_image	= m_current_source_image;
			m_p_assets->add_tile(m_current_tileset,tile);
		}
	}
	return 0;
}

int 
ParserGAP::command_export(int line_number, const CommandLine & command)
{
	gap::exporter::ExportInfo exportinfo;

	for(const auto & [key,value] : command.args)
	{
		auto hash = ade::hash::hash_ascii_string_as_lower(key.c_str(),key.size());
		switch(hash)
		{
			case ade::hash::hash_ascii_string_as_lower("filename") :	
			case ade::hash::hash_ascii_string_as_lower("path") :		
				exportinfo.filename = value;	
				break;

			case ade::hash::hash_ascii_string_as_lower("name") :	
				exportinfo.name = value;	
				break;

			case ade::hash::hash_ascii_string_as_lower("type") 			:		
				{
					auto valhash = ade::hash::hash_ascii_string_as_lower(value.c_str(),value.size());
					switch(valhash)
					{
						case ade::hash::hash_ascii_string_as_lower("gbin") :				exportinfo.type = gap::exporter::TYPE_GBIN; break;
						case ade::hash::hash_ascii_string_as_lower("definitions") :	exportinfo.type = gap::exporter::TYPE_DEFINITIONS; break;
						default :																										return on_error(line_number,std::string("Unknown export type! - ") + value);
					}
				}
				break;

			case ade::hash::hash_ascii_string_as_lower("format") 			:		
				{
					auto valhash = ade::hash::hash_ascii_string_as_lower(value.c_str(),value.size());
					switch(valhash)
					{
						case ade::hash::hash_ascii_string_as_lower("binary") :				exportinfo.format = gap::exporter::FORMAT_BINARY; 			break;
						case ade::hash::hash_ascii_string_as_lower("cheader") :				exportinfo.format = gap::exporter::FORMAT_C_HEADER; 		break;
						case ade::hash::hash_ascii_string_as_lower("c_array") :				exportinfo.format = gap::exporter::FORMAT_C_ARRAY; 			break;
						case ade::hash::hash_ascii_string_as_lower("cpp_vector") :		exportinfo.format = gap::exporter::FORMAT_CPP_VECTOR; 	break;
						case ade::hash::hash_ascii_string_as_lower("cpp_stdarray") :	exportinfo.format = gap::exporter::FORMAT_CPP_STDARRAY; break;
						default :																											return on_error(line_number,std::string("Unknown export format! - ") + value);
					}
				}
				break;

			default : 
				// TODO: Warning - unknown arg
				break;
		}
	}

	m_export_info.push_back(exportinfo);

	return 0;
}

int 
ParserGAP::command_file(int line_number, const CommandLine & command)
{
	gap::assets::FileInfo fileinfo;

	for(const auto & [key,value] : command.args)
	{
		auto hash = ade::hash::hash_ascii_string_as_lower(key.c_str(),key.size());
		switch(hash)
		{
			case ade::hash::hash_ascii_string_as_lower("src") 	:	fileinfo.source_path 	= value; break;
			case ade::hash::hash_ascii_string_as_lower("name") 	:	fileinfo.name 				= value; break;
			case ade::hash::hash_ascii_string_as_lower("type") 	:	fileinfo.type 				= ade::hash::fourcc(value.c_str(),value.size()); break;
			default : 
				// TODO: Warning - unknown arg
				break;
		}
	}

	if(fileinfo.source_path.empty())
		return on_error(line_number,"Missing file path!");

	if(fileinfo.name.empty())
		fileinfo.name = fileinfo.source_path;

	fileinfo.data = m_filesystem.load(fileinfo.source_path);
	if(fileinfo.data.empty())
		return on_error(line_number,"Failed to load file '"s + fileinfo.source_path + "'!"s);

	//---------------------------------------------------------------------------
	//	Determine file type
	//---------------------------------------------------------------------------
	if(fileinfo.type == 0)
	{
		auto pos = fileinfo.source_path.find_last_of('.');
		if(pos != std::string::npos)
		{
			std::string ext(fileinfo.source_path.substr(pos+1));
			std::transform(begin(ext),end(ext),begin(ext),::toupper);
			if(!ext.empty() && (ext.size() <= 4))
				fileinfo.type = ade::hash::fourcc(ext.c_str(),ext.size());
		}
	}

	m_p_assets->add_file(std::move(fileinfo));
		
	return 0;
}


int 
ParserGAP::command_colourmap(int line_number, const CommandLine & command)
{

	std::string src;
	std::string name;

	for(const auto & [key,value] : command.args)
	{
		auto hash = ade::hash::hash_ascii_string_as_lower(key.c_str(),key.size());
		switch(hash)
		{
			case ade::hash::hash_ascii_string_as_lower("src") 	:	src 	= value; break;
			case ade::hash::hash_ascii_string_as_lower("name") 	:	name	= value; break;
			default : 
				// TODO: Warning - unknown arg
				break;
		}
	}

	if(name.empty())
		return on_error(line_number, "Name not specified for colour map!");

	int index = m_p_assets->find_colour_map(name);

	if(index >= 0)
	{
		if(src.empty())
			m_current_colourmap = index;
		else
			return on_error(line_number, "Colour map '" + name + "' already exists! Unable to load from file '" + src + "'!");
	}
	else
	{
		auto colour_map = load_colour_map(src, m_filesystem);
		if(colour_map.empty())
			return on_error(line_number,"Failed to load colour map file '" + src + "'!");
		colour_map.name = name;
		index = m_p_assets->add_colour_map(colour_map);
		m_current_colourmap = index;
	}

	return 0;
}

} // namespace gap

