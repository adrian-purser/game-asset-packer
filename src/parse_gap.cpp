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
#include <format>
#include <print>
#include "parse_gap.h"
#include "parse_colour_map.h"
#include "utility/hash.h"

#define GAPCMD_LOADIMAGE				"loadimage"
#define GAPCMD_IMAGE						"image"
#define GAPCMD_IMAGEGROUP				"imagegroup"
#define GAPCMD_IMAGEARRAY				"imagearray"
#define GAPCMD_IMAGESEQUENCE		"imagesequence"
#define GAPCMD_IMAGESEQ					"imageseq"
#define GAPCMD_IMAGEFRAME				"imageframe"
#define GAPCMD_TILESET					"tileset"
#define GAPCMD_TILE							"tile"
#define GAPCMD_TILEARRAY				"tilearray"
#define GAPCMD_TILEMAP					"tilemap"
#define GAPCMD_LOADTILEMAP			"loadtilemap"
#define GAPCMD_EXPORT						"export"
#define GAPCMD_FILE							"file"
#define GAPCMD_COLOURMAP				"colourmap"
#define GAPCMD_SOUNDSAMPLE			"soundsample"

using namespace std::literals::string_literals;

#define HASH(s)	ade::hash::hash_ascii_string_as_lower(s)
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
		case ade::hash::hash_ascii_string_as_lower(GAPCMD_LOADIMAGE) :			result = command_loadimage(line_number,cmd); 			break;
		case ade::hash::hash_ascii_string_as_lower(GAPCMD_IMAGEGROUP) :			result = command_imagegroup(line_number,cmd);		 	break;
		case ade::hash::hash_ascii_string_as_lower(GAPCMD_IMAGE) :					result = command_image(line_number,cmd); 					break;
		case ade::hash::hash_ascii_string_as_lower(GAPCMD_IMAGEARRAY) :			result = command_imagearray(line_number,cmd); 		break;
		case ade::hash::hash_ascii_string_as_lower(GAPCMD_IMAGESEQUENCE) :	[[fallthrough]];
		case ade::hash::hash_ascii_string_as_lower(GAPCMD_IMAGESEQ) :				result = command_imagesequence(line_number,cmd); 	break;
		case ade::hash::hash_ascii_string_as_lower(GAPCMD_IMAGEFRAME) :			result = command_imageframe(line_number,cmd); 		break;
		case ade::hash::hash_ascii_string_as_lower(GAPCMD_TILESET) :				result = command_tileset(line_number,cmd); 				break;
		case ade::hash::hash_ascii_string_as_lower(GAPCMD_TILE) :						result = command_tile(line_number,cmd); 					break;
		case ade::hash::hash_ascii_string_as_lower(GAPCMD_TILEARRAY) :			result = command_tilearray(line_number,cmd); 			break;
		case ade::hash::hash_ascii_string_as_lower(GAPCMD_TILEMAP) :				result = command_tilemap(line_number,cmd); 				break;
		case ade::hash::hash_ascii_string_as_lower(GAPCMD_LOADTILEMAP) :		result = command_loadtilemap(line_number,cmd); 		break;
		case ade::hash::hash_ascii_string_as_lower(GAPCMD_EXPORT) :					result = command_export(line_number,cmd); 				break;
		case ade::hash::hash_ascii_string_as_lower(GAPCMD_FILE) :						result = command_file(line_number,cmd); 					break;
		case ade::hash::hash_ascii_string_as_lower(GAPCMD_COLOURMAP) :			result = command_colourmap(line_number,cmd); 			break;
		case ade::hash::hash_ascii_string_as_lower(GAPCMD_SOUNDSAMPLE) :		result = command_soundsample(line_number,cmd); 		break;

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

//	std::cout << "  Image added into slot " << m_current_source_image << std::endl;

	return 0;
}

int
ParserGAP::command_imagegroup(int line_number, const CommandLine & command)
{

	int 					base 	= 0;
	std::string		name;

	for(const auto & [key,value] : command.args)
	{
		auto hash = ade::hash::hash_ascii_string_as_lower(key.c_str(),key.size());

		switch(hash)
		{
			case ade::hash::hash_ascii_string_as_lower("base") 		:	base	= std::strtol(value.c_str(),nullptr,10); 	break;
			case ade::hash::hash_ascii_string_as_lower("name") 		:	name	= value; 	break;

			default :
				// TODO: Warning - unknown arg
				break;
		}
	}

	if(name.empty())
		return on_error(line_number, "Missing parameter 'name' in imagegroup!");

	auto group = m_p_assets->add_image_group(name, base);
	if(group < 0)
		return on_error(line_number, std::format("Failed to add imagegroup '{}'!",name));

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
		m_p_assets->add_image(image);
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
				image.name = std::format("{}-{:03}",name,i);
			m_p_assets->add_image(image);
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
				image.name = std::format("{}_{}_{}",name,xi,yi);
			m_p_assets->add_image(image);
		}
	}

	return 0;
}

int
ParserGAP::command_imagesequence(int line_number, const CommandLine & command)
{
	int					mode = gap::assets::ImageSequence::MODE_LOOP;
	std::string	name;

	for(const auto & [key,value] : command.args)
	{
		auto hash = ade::hash::hash_ascii_string_as_lower(key.c_str(),key.size());
		switch(hash)
		{
			case HASH("name") 		:	name 			= value; break;
			case HASH("mode") 		:	{
																auto modehash = ade::hash::hash_ascii_string_as_lower(value.c_str(),value.size());
																switch(modehash)
																{
																	case HASH("loop") : 	mode = gap::assets::ImageSequence::MODE_LOOP; break;
																	case HASH("bounce") : mode = gap::assets::ImageSequence::MODE_BOUNCE; break;
																	case HASH("once") : 	mode = gap::assets::ImageSequence::MODE_ONCE; break;
																	// TODO(Ade): Warn about unknown mode type
																}
															}
															break;
			default :
				// TODO(Ade): Warning - unknown arg
				break;
		}
	}

	if(name.empty())
		return on_error(line_number, "Missing parameter 'name' in image sequence!");

	if(m_p_assets->add_image_sequence(name, mode) < 0)
		return on_error(line_number, std::format("Failed to add image sequence '{}'!", name));

	return 0;
}

int
ParserGAP::command_imageframe(int line_number, const CommandLine & command)
{
	std::string		group;
	std::string		image;
	int						time 	= 1;
	int						x 		= 0;
	int						y 		= 0;
	int						count = 1;

	for(const auto & [key,value] : command.args)
	{
		auto lcvalue = value;
		std::transform(begin(lcvalue), end(lcvalue), begin(lcvalue), ::tolower);

		auto hash = ade::hash::hash_ascii_string_as_lower(key.c_str(),key.size());
		switch(hash)
		{
			case HASH("group") 		:	group 	= value; break;
			case HASH("image") 		:	image 	= value; break;
			case HASH("time") 		: time 		= std::strtol(value.c_str(),nullptr,10); break;
			case HASH("x") 				: x 			= std::strtol(value.c_str(),nullptr,10); break;
			case HASH("y") 				: y 			= std::strtol(value.c_str(),nullptr,10); break;
			case HASH("count") 		: count 	= lcvalue == "all" ? -1 : std::strtol(value.c_str(),nullptr,10); break;

			default :
				// TODO(Ade): Warning - unknown arg
				break;
		}
	}

	if(count == 0)
		return on_error(line_number, "Invalid 'count' in image frame!");

	if(m_p_assets->add_image_frame( group, image, time, x, y, count) < 0)
		return on_error(line_number, std::format("Failed to add image frame - {}", m_p_assets->get_last_error()));

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

	// ----- If the tileset id was not specified then generate a unique id. -----
	if(tileset.id < 0)	tileset.id = m_p_assets->generate_tileset_id(m_current_tileset >= 0 ? m_current_tileset : 1);
	if(tileset.id < 0)	return on_error(line_number,std::string("Invalid/Missing 'id' parameter!"));

	// ----- If the width and height are specified then add the tileset, otherwise just make the tileset current. -----
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

			case ade::hash::hash_ascii_string_as_lower("section") :
				exportinfo.section = value;
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

//=============================================================================
//
//	COLOURMAP
//
//=============================================================================

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

//=============================================================================
//
//	LOADTILEMAP
//
//=============================================================================

int
ParserGAP::command_loadtilemap(int line_number, const CommandLine & command)
{

	std::string src;
	std::string type;

	for(const auto & [key,value] : command.args)
	{
		auto hash = ade::hash::hash_ascii_string_as_lower(key.c_str(),key.size());
		switch(hash)
		{
			case ade::hash::hash_ascii_string_as_lower("src") 		:	src 	= value; break;
			case ade::hash::hash_ascii_string_as_lower("type") 		:	type 	= value; break;
			default :
				// TODO: Warning - unknown arg
				break;
		}
	}

	if(src.empty())			return on_error(line_number,"Missing tilemap path!");
	if(type.empty())		return on_error(line_number,"Missing tilemap type!");

	auto p_tilemap = gap::tilemap::load(src,type,m_filesystem);
	if(p_tilemap == nullptr)
		return on_error(line_number,std::string("Failed to load tilemap! - ") + src);


	m_p_current_tilemap = std::move(p_tilemap);

	return 0;
}


//=============================================================================
//
//	TILEMAP
//
//=============================================================================

/*
ID                           Numerical identifier of the tileset.
NAME                         Name of the tilemap that can be used for searching.
W or WIDTH                   Width, in tiles. (If omitted, assumes dimension of loaded tilemap)
H or HEIGHT                  Height, in tiles. (If omitted, assumes dimension of loaded tilemap)
BLKSIZE or BLOCKSIZE         Block Size, in tiles. Default = 8. (This represents the width and height of the blocks)
TILESIZE                     Tile size, in bytes. Default = auto. (If not specified, will detect size based upon the data)
LAYER                        The layer to use as the source of tile data. Default = 0
*/

int
ParserGAP::command_tilemap(int line_number, const CommandLine & command)
{
	std::string 	name;
	uint32_t 			id					= 0;
	uint32_t 			x						= 0;
	uint32_t 			y						= 0;
	uint32_t 			width				= 0;
	uint32_t 			height			= 0;
	uint32_t 			blocksize		= 8;
	uint32_t 			tilesize		= 0;
	uint32_t 			layer_id		= 0;

	//---------------------------------------------------------------------------
	//	Parse Arguments
	//---------------------------------------------------------------------------
	for(const auto & [key,value] : command.args)
	{
		auto hash = ade::hash::hash_ascii_string_as_lower(key.c_str(),key.size());
		switch(hash)
		{
			case ade::hash::hash_ascii_string_as_lower("name") 				:	name 			= value; break;
			case ade::hash::hash_ascii_string_as_lower("id") 					:	id				= std::strtol(value.c_str(),nullptr,10); 	break;
			case ade::hash::hash_ascii_string_as_lower("x") 					:	x			 		= std::strtol(value.c_str(),nullptr,10); 	break;
			case ade::hash::hash_ascii_string_as_lower("y") 					:	y					= std::strtol(value.c_str(),nullptr,10); 	break;
			case ade::hash::hash_ascii_string_as_lower("width") 			:	width 		= std::strtol(value.c_str(),nullptr,10); 	break;
			case ade::hash::hash_ascii_string_as_lower("height") 			:	height		= std::strtol(value.c_str(),nullptr,10); 	break;
			case ade::hash::hash_ascii_string_as_lower("blksize") 		:	[[fallthrough]];
			case ade::hash::hash_ascii_string_as_lower("blocksize") 	:	blocksize = std::strtol(value.c_str(),nullptr,10); 	break;
			case ade::hash::hash_ascii_string_as_lower("tilesize") 		:	tilesize 	= std::strtol(value.c_str(),nullptr,10); 	break;
			case ade::hash::hash_ascii_string_as_lower("layer") 			:	layer_id 	= std::strtol(value.c_str(),nullptr,10); 	break;
			default :
				// TODO: Warning - unknown arg
				break;
		}
	}

	//---------------------------------------------------------------------------
	//	Validate and update parameters
	//---------------------------------------------------------------------------
	if(m_p_current_tilemap == nullptr)
		return on_error(line_number,std::string("No tilemap has been loaded!"));

	m_p_current_tilemap->enumerate_layers([&](const gap::tilemap::SourceTileMapLayer & layer)->bool
		{
			std::cout << std::format("LAYER: {:5} :{:24}: {}x{}\n", layer.m_id, layer.m_name, layer.m_width, layer.m_height);
			return true;
		});

	auto p_layer = m_p_current_tilemap->get_layer(layer_id);
	if(p_layer == nullptr)
		return on_error(line_number,std::format("Layer {} not found in tilemap!", layer_id));
	const auto & layer = *p_layer;

	if(blocksize == 0)
		return on_error(line_number,std::format("Invalid Block Size (0)!"));

	if(layer.m_data.empty())
		return on_error(line_number,std::format("Layer {} is empty!", layer_id));

	if((x>=layer.m_width) || (y>layer.m_height))
		return on_error(line_number,std::format("Specified coordinates ({},{}) are outside of the tilemap area!", x,y));

	if(width == 0)		width 		= layer.m_width - x;
	if(height == 0)		height 		= layer.m_height - y;
	if(name.empty())	name			= layer.m_name;

	if((width == 0) || (height == 0))
		return on_error(line_number,std::format("Specified dimensions ({},{}) are too small", width, height));

	if(tilesize == 0)
	{
		auto largest = *std::max_element(begin(layer.m_data), end(layer.m_data));
		uint32_t bits=0;
		for(int i=0;(i<32) && (largest!=0);++i, ++bits, largest >>= 1)
			;
		tilesize = (bits+7)/8;
	}

	std::println("TILEMAP: id:{} name:'{}' pos:{},{} size:{}x{} blksize:{} tilesize:{}", id, name, x, y, width, height, blocksize, tilesize);

	uint32_t blocks_wide = std::max<uint32_t>(1,(width+(blocksize-1))/blocksize);
	uint32_t blocks_high = std::max<uint32_t>(1,(height+(blocksize-1))/blocksize);

	std::println("TILEMAP: Blocks Wide: {} Blocks High: {}",blocks_wide, blocks_high);

	//---------------------------------------------------------------------------
	//	Create TileMap
	//---------------------------------------------------------------------------
	auto p_tilemap = std::make_unique<gap::tilemap::TileMap>(id, name, width, height, blocksize, tilesize);

	for(uint32_t iy=y, h=0; h<height; ++h, ++iy)
	{
		for(uint32_t ix=x, w=0; w<width; ++w, ++ix)
		{
			uint64_t 	tile = layer.get_tile(ix,iy);
			p_tilemap->set(ix,iy,tile);
		}
	}

	p_tilemap->print();

	m_p_assets->add_tilemap(std::move(p_tilemap));

	return 0;
}

//=============================================================================
//
//	SOUND SAMPLE
//
//=============================================================================
static uint8_t
decode_sample_format(std::string fmt)
{
	std::transform(begin(fmt), end(fmt), begin(fmt), ::toupper);
	if(fmt == "S8")		return gap::sound::FORMAT_S8;
	if(fmt == "U8")		return gap::sound::FORMAT_U8;
	if(fmt == "S16")	return gap::sound::FORMAT_S16;
	if(fmt == "U16")	return gap::sound::FORMAT_U16;

	return gap::sound::FORMAT_S8;
}

int
ParserGAP::command_soundsample(int line_number,const CommandLine & command)
{
	std::string 	name;
	std::string		source;
	uint8_t				format			= 0;	// Required format.
	uint8_t				srcformat		= 0;	// Source format hint, for RAW files.
	uint16_t			srcrate			= 0;	// Source rate hint, for RAW files.
	uint16_t			rate				= 0; 	// Desired rate

	//---------------------------------------------------------------------------
	//	Parse Arguments
	//---------------------------------------------------------------------------
	for(const auto & [key,value] : command.args)
	{
		auto hash = ade::hash::hash_ascii_string_as_lower(key.c_str(),key.size());
		switch(hash)
		{
			case ade::hash::hash_ascii_string_as_lower("name") 				:	name 			= value; break;
			case ade::hash::hash_ascii_string_as_lower("src") 				:	source		= value; break;
			case ade::hash::hash_ascii_string_as_lower("srcformat") 	:	srcformat	= decode_sample_format(value); break;
			case ade::hash::hash_ascii_string_as_lower("format") 			:	format		= decode_sample_format(value); break;

			case ade::hash::hash_ascii_string_as_lower("srcrate") 		:	srcrate		= std::strtol(value.c_str(),nullptr,10); 	break;
			case ade::hash::hash_ascii_string_as_lower("rate") 				:	rate			= std::strtol(value.c_str(),nullptr,10); 	break;
			default :
				// TODO: Warning - unknown arg
				break;
		}
	}

	std::print("SOUNDSAMPLE: name: {}, source: {}, srcfmt: {}, format: {}, srcrate: {}, rate: {}\n", name, source, format, srcformat, srcrate, rate);

	return 0;
}


} // namespace gap

