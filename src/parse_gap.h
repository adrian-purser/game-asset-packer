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

namespace gap
{

class ParserGAP
{
private:
	gap::FileSystem &											m_filesystem;
	std::unique_ptr<gap::assets::Assets>	m_p_assets;
	std::mutex														m_mutex;

	int																		m_current_source_image				= -1;

public:
	ParserGAP(gap::FileSystem & filesystem);
	~ParserGAP() = default;
	ParserGAP(const ParserGAP &) = delete;
	ParserGAP & operator=(const ParserGAP &) = delete;

	std::unique_ptr<gap::assets::Assets>		parse(std::string_view source);

private:
	int									parse_line(std::string_view line,int line_number);
	int									on_error(int line_number,const std::string & error_message) {std::cerr << "Line " << line_number << ": " << error_message << '\n';return -1;}
	
	int 								command_loadimage(int line_number,const std::vector<std::string> & tokens);
	int 								command_imagegroup(int line_number,const std::vector<std::string> & tokens);
	int 								command_image(int line_number,const std::vector<std::string> & tokens);

};


} // namespace gap

#endif // ! defined GUARD_ADE_GAME_ASSET_PACKER_PARSE_GAP_H
