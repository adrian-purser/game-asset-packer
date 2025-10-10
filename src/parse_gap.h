//=============================================================================
//	FILE:						parse_gap.h
//	SYSTEM:				 	game asset packer
//	DESCRIPTION:
//-----------------------------------------------------------------------------
//	COPYRIGHT:			(C)Copyright 2019 Adrian Purser. All Rights Reserved.
//	LICENCE:				MIT
//	MAINTAINER:			AJP - Adrian Purser <ade@arcadestuff.com>
//	CREATED:				25-SEP-2019 Adrian Purser <ade@arcadestuff.com>
//=============================================================================
#ifndef GUARD_ADE_GAME_ASSET_PACKER_PARSE_GAP_H
#define GUARD_ADE_GAME_ASSET_PACKER_PARSE_GAP_H

#include <cstdint>
#include <vector>
#include <string>
#include <memory>
#include <mutex>
#include "assets.h"
#include "filesystem.h"
#include "export.h"
#include "source_tilemap.h"

namespace gap
{

struct CommandLine
{
	std::string 																	command;
	std::unordered_map<std::string,std::string>		args;
};

class ParserGAP
{
private:
	gap::FileSystem &																	m_filesystem;
	std::unique_ptr<gap::assets::Assets>							m_p_assets;
	std::mutex																				m_mutex;

	std::vector<gap::exporter::ExportInfo>						m_export_info;

	int																								m_current_source_image				= -1;
//	int 																							m_current_image_group					= 0;
	int																								m_current_tileset							= -1;
	int																								m_current_colourmap						= -1;
	std::unique_ptr<gap::tilemap::SourceTileMap>			m_p_current_tilemap;

public:
	ParserGAP(gap::FileSystem & filesystem);
	~ParserGAP() = default;
	ParserGAP(const ParserGAP &) = delete;
	ParserGAP & operator=(const ParserGAP &) = delete;

	std::unique_ptr<gap::assets::Assets>		parse(std::string_view source);

	void								enumerate_exports(std::function<bool(const gap::exporter::ExportInfo &)> callback);

private:
	int									parse_line(std::string_view line,int line_number);
	int									on_error(int line_number,const std::string & error_message) {std::cerr << "Line " << line_number << ": " << error_message << '\n';return -1;}

	int									command_colourmap(int line_number,const CommandLine & args);
	int 								command_loadimage(int line_number,const CommandLine & args);
	int 								command_imagegroup(int line_number,const CommandLine & args);
	int 								command_image(int line_number,const CommandLine & args);
	int 								command_imagearray(int line_number,const CommandLine & args);
	int 								command_imagesequence(int line_number,const CommandLine & args);
	int 								command_imageframe(int line_number,const CommandLine & args);
	int 								command_export(int line_number,const CommandLine & args);
	int 								command_file(int line_number,const CommandLine & args);
	int 								command_tileset(int line_number,const CommandLine & args);
	int 								command_tile(int line_number,const CommandLine & args);
	int 								command_tilearray(int line_number,const CommandLine & args);
	int 								command_tilemap(int line_number,const CommandLine & args);
	int									command_loadtilemap(int line_number,const CommandLine & args);
	int									command_soundsample(int line_number,const CommandLine & args);
};


} // namespace gap

#endif // ! defined GUARD_ADE_GAME_ASSET_PACKER_PARSE_GAP_H
