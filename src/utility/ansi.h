//=============================================================================
//	FILE:					ansi.h
//	SYSTEM:	
//	DESCRIPTION:	
//-----------------------------------------------------------------------------
//  COPYRIGHT:		(C) Copyright 2019 Adrian Purser. All Rights Reserved.
//	LICENCE:			MIT - See LICENSE file for details
//	MAINTAINER:		Adrian Purser <ade@adrianpurser.co.uk>
//	CREATED:			13-JUN-2019 Adrian Purser <ade@adrianpurser.co.uk>
//=============================================================================
#ifndef GUARD_ADE_ANSI_H
#define GUARD_ADE_ANSI_H

//#include <iostream>
#include <cstdint>
#include <cstdio>
#include <string>
#include <string_view>
#include <functional>
#include <vector>
#include <array>
#include <span>
#include <optional>

// http://ascii-table.com/ansi-escape-sequences.php
// http://ascii-table.com/ansi-escape-sequences-vt-100.php

namespace ansi
{
	enum
	{
		INSERT_BLANK_CHARACTERS						= '@',
		CURSOR_UP													= 'A',	// [CSI]Ps[A]		Cursor Up Ps Times (default = 1)
		CURSOR_DOWN												= 'B',	// [CSI]Ps[B]		Cursor Down Ps Times (default = 1)
		CURSOR_FORWARD										= 'C',	// [CSI]Ps[C]		Cursor Forward Ps Times (default = 1)
		CURSOR_BACKWARD										= 'D',	// [CSI]Ps[D]		Cursor Backward Ps Times (default = 1)
		CURSOR_NEXT_LINE									= 'E',
		CURSOR_PREVIOUS_LINE							= 'F',
		CURSOR_HORIZONTAL_ABSOLUTE				= 'G',
		CURSOR_POSITION										=	'H',
		CURSOR_HORIZONTAL_TAB							= 'I',
		ERASE_IN_DISPLAY									= 'J',
		ERASE_IN_LINE											= 'K',
		INSERT_LINES											= 'L',
		DELETE_LINES											= 'M',
		DELETE_CHARACTERS									= 'P',
		SCROLL_UP													= 'S',
		SCROLL_DOWN												= 'T',
		ERASE_CHARACTERS									= 'X',
		CURSOR_BACKWARDS_TAB							= 'Z',
		SAVE_CURSOR_POSITION							= 's',
		RESTORE_CURSOR_POSITION						= 'u',
		ERASE_LINE												= 'K',
		ERASE_SCREEN											= 'K',
		DELETE_COLUMN											= '~',
		SET_GRAPHIS_RENDITION							= 'm',
		SET_MODE													= 'h',
		RESET_MODE												= 'l',
		SET_KEYBOARD_STRINGS							= 'p'
	};

	enum
	{
		BLACK	= 30,
		RED,
		GREEN,
		YELLOW,
		BLUE,
		MAGENTA,
		CYAN,
		LIGHT_GREY,
		DARK_GREY = 90,
		LIGHT_RED,
		LIGHT_GREEN,
		LIGHT_YELLOW,
		LIGHT_BLUE,
		LIGHT_MAGENTA,
		LIGHT_CYAN,
		WHITE
	};

	constexpr std::string_view cursor_save 											= "\033[s";
	constexpr std::string_view cursor_restore 									= "\033[u";
	constexpr std::string_view clear_line_from_cursor_right 		= "\033[K";
	constexpr std::string_view clear_line_from_cursor_left 			= "\033[1K";
	constexpr std::string_view clear_line 											= "\033[2K";
	constexpr std::string_view clear_display 										= "\033[2J";
	constexpr std::string_view reset_terminal										= "\033c";

// Select Graphics Rendition (SGR) parameters
namespace sgr
{
	static constexpr uint8_t	BOLD												= 1;
	static constexpr uint8_t	FAINT												= 2;
	static constexpr uint8_t	ITALIC											= 3;
	static constexpr uint8_t	UNDERLINE										= 4;
	static constexpr uint8_t	SLOW_BLINK									= 5;
	static constexpr uint8_t	RAPID_BLINK									= 6;
	static constexpr uint8_t	REVERSE											= 7;
	static constexpr uint8_t	CONCEAL											= 8;
	static constexpr uint8_t	STRIKE											= 9;
	static constexpr uint8_t	DOUBLE_UNDERLINE						= 21;
	static constexpr uint8_t	NORMAL_INTENSITY						= 22;
	static constexpr uint8_t	NOT_ITALIC									= 23;
	static constexpr uint8_t	NOT_UNDERLINED							= 24;
	static constexpr uint8_t	NOT_BLINKING								= 25;
	static constexpr uint8_t	NOT_REVERSED								= 27;
	static constexpr uint8_t	REVEAL											= 28;
	static constexpr uint8_t	NOT_CROSSED_OUT							= 29;

	static constexpr uint8_t	FG_BLACK										= 30;
	static constexpr uint8_t	FG_RED											= 31;
	static constexpr uint8_t	FG_GREEN										= 32;
	static constexpr uint8_t	FG_YELLOW										= 33;
	static constexpr uint8_t	FG_BLUE											= 34;
	static constexpr uint8_t	FG_MAGENTA									= 35;
	static constexpr uint8_t	FG_CYAN											= 36;
	static constexpr uint8_t	FG_WHITE										= 37;

	static constexpr uint8_t	SET_FOREGROUND_COLOUR				= 38;
	static constexpr uint8_t	DEFAULT_FOREGROUND_COLOUR		= 39;

	static constexpr uint8_t	BG_BLACK										= 40;
	static constexpr uint8_t	BG_RED											= 41;
	static constexpr uint8_t	BG_GREEN										= 42;
	static constexpr uint8_t	BG_YELLOW										= 43;
	static constexpr uint8_t	BG_BLUE											= 44;
	static constexpr uint8_t	BG_MAGENTA									= 45;
	static constexpr uint8_t	BG_CYAN											= 46;
	static constexpr uint8_t	BG_WHITE										= 47;

	static constexpr uint8_t	SET_BACKGROUND_COLOUR				= 48;
	static constexpr uint8_t	DEFAULT_BACKGROUND_COLOUR		= 49;

	static constexpr uint8_t	SET_UNDERLINE_COLOUR				= 58;
	static constexpr uint8_t	DEFAULT_UNDERLINE_COLOUR		= 59;

	static constexpr uint8_t	FG_LIGHT_BLACK							= 90;
	static constexpr uint8_t	FG_LIGHT_RED								= 91;
	static constexpr uint8_t	FG_LIGHT_GREEN							= 92;
	static constexpr uint8_t	FG_LIGHT_YELLOW							= 93;
	static constexpr uint8_t	FG_LIGHT_BLUE								= 94;
	static constexpr uint8_t	FG_LIGHT_MAGENTA						= 95;
	static constexpr uint8_t	FG_LIGHT_CYAN								= 96;
	static constexpr uint8_t	FG_LIGHT_WHITE							= 97;

	static constexpr uint8_t	BG_LIGHT_BLACK							= 100;
	static constexpr uint8_t	BG_LIGHT_RED								= 101;
	static constexpr uint8_t	BG_LIGHT_GREEN							= 102;
	static constexpr uint8_t	BG_LIGHT_YELLOW							= 103;
	static constexpr uint8_t	BG_LIGHT_BLUE								= 104;
	static constexpr uint8_t	BG_LIGHT_MAGENTA						= 105;
	static constexpr uint8_t	BG_LIGHT_CYAN								= 106;
	static constexpr uint8_t	BG_LIGHT_WHITE							= 107;

} // namespace sgr

} // namespace ansi


#ifndef ANSI_DISABLE_MACROS

#define FOREGROUND_BLACK						"\033[30m"
#define FOREGROUND_RED							"\033[31m"
#define FOREGROUND_GREEN						"\033[32m"
#define FOREGROUND_YELLOW						"\033[33m"
#define FOREGROUND_BLUE							"\033[34m"
#define FOREGROUND_MAGENTA					"\033[35m"
#define FOREGROUND_CYAN							"\033[36m"
#define FOREGROUND_LIGHT_GREY				"\033[37m"
#define FOREGROUND_DEFAULT					"\033[39m"
#define FOREGROUND_DARK_GREY				"\033[90m"
#define FOREGROUND_LIGHT_RED				"\033[91m"
#define FOREGROUND_LIGHT_GREEN			"\033[92m"
#define FOREGROUND_LIGHT_YELLOW			"\033[93m"
#define FOREGROUND_LIGHT_BLUE				"\033[94m"
#define FOREGROUND_LIGHT_MAGENTA		"\033[95m"
#define FOREGROUND_LIGHT_CYAN				"\033[96m"
#define FOREGROUND_WHITE						"\033[97m"

#define BACKGROUND_BLACK						"\033[40m"
#define BACKGROUND_RED							"\033[41m"
#define BACKGROUND_GREEN						"\033[42m"
#define BACKGROUND_YELLOW						"\033[43m"
#define BACKGROUND_BLUE							"\033[44m"
#define BACKGROUND_MAGENTA					"\033[45m"
#define BACKGROUND_CYAN							"\033[46m"
#define BACKGROUND_LIGHT_GREY				"\033[47m"
#define BACKGROUND_DEFAULT					"\033[49m"
#define BACKGROUND_DARK_GREY				"\033[100m"
#define BACKGROUND_LIGHT_RED				"\033[101m"
#define BACKGROUND_LIGHT_GREEN			"\033[102m"
#define BACKGROUND_LIGHT_YELLOW			"\033[103m"
#define BACKGROUND_LIGHT_BLUE				"\033[104m"
#define BACKGROUND_LIGHT_MAGENTA		"\033[105m"
#define BACKGROUND_LIGHT_CYAN				"\033[106m"
#define BACKGROUND_WHITE						"\033[107m"

#define SGR_BOLD										"\033[1m"
#define SGR_FAINT										"\033[2m"
#define SGR_ITALIC									"\033[3m"
#define SGR_UNDERLINE								"\033[4m"
#define SGR_SLOW_BLINK							"\033[5m"
#define SGR_RAPID_BLINK							"\033[6m"
#define SGR_REVERSE									"\033[7m"
#define SGR_CONCEAL									"\033[8m"
#define SGR_STRIKE									"\033[9m"

#define SGR_DOUBLE_UNDERLINE				"\033[21m"
#define SGR_NORMAL_INTENSITY				"\033[22m"
#define SGR_NOT_ITALIC							"\033[23m"
#define SGR_NOT_UNDERLINED					"\033[24m"
#define SGR_NOT_BLINKING						"\033[25m"
#define SGR_NOT_REVERSE							"\033[27m"
#define SGR_REVEAL									"\033[28m"
#define SGR_NOT_CROSSED_OUT					"\033[29m"

//#define FOREGROUND_ORANGE						"\033[38;5;208m"
#define FOREGROUND_ORANGE						"\033[38;2;240;120;0m"
#define FOREGROUND_GREY							"\033[38;5;242m"
//#define FOREGROUND_DARK_GREY				"\033[38;5;238m"
//#define FOREGROUND_LIGHT_GREY				"\033[38;5;248m"

#define RESET_COLOUR								"\033[0m"

#define CURSOR_COLUMN(_c_)	"\033[" #_c_ "G"

#endif




namespace ansi::stdio
{
inline void reset_terminal() 												{printf("\033c");}

inline void foreground_colour(int colour)						{printf("\033[%dm",colour);}
inline void foreground_colour(int r, int g, int b)	{printf("\033[38;2;%d;%d;%dm",r,g,b);}
inline void background_colour(int colour)						{printf("\033[%dm",colour+10);}
inline void background_colour(int r, int g, int b)	{printf("\033[48;2;%d;%d;%dm",r,g,b);}
inline void bold(bool b_set)												{printf("\033[1%c",b_set ? 'm' : 'l');}
inline void underline(bool b_set)										{printf("\033[4%c",b_set ? 'm' : 'l');}
inline void blink(bool b_set)												{printf("\033[5%c",b_set ? 'm' : 'l');}
inline void reversed(bool b_set)										{printf("\033[6%c",b_set ? 'm' : 'l');}
inline void reset_colour()													{printf("\033[0m");}
inline void cursor_column(int column) 							{printf("\033[%dG",column);}
inline void cursor_position(int column,int row) 		{printf("\033[%d;%dG",row,column);}
inline void cursor_up(int count) 										{printf("\033[%dA",count);}
inline void cursor_down(int count) 									{printf("\033[%dB",count);}
inline void cursor_right(int count) 								{printf("\033[%dC",count);}
inline void cursor_left(int count) 									{printf("\033[%dD",count);}
inline void cursor_save() 													{printf("\033[s");}
inline void cursor_restore() 												{printf("\033[u");}
inline void clear_screen() 													{printf("\033[2J");}
inline void clear_line() 														{printf("\033[2K");}
inline void clear_to_eol() 													{printf("\033[K");}

} // namespace ansi::stdio

namespace ansi
{

inline void
foreground_colour(int r, int g, int b, std::function<void(char ch)> print_func)
{
	char buffer[64];
	snprintf(buffer,sizeof(buffer),"\033[38;2;%d;%d;%dm",r,g,b);
	for(size_t i=0;(i<sizeof(buffer)) && (buffer[i] != 0);++i)
		print_func(buffer[i]);
}

inline void
foreground_colour(uint32_t colour, std::function<void(char ch)> print_func)
{
	foreground_colour( (colour >> 16) & 0x0FF, (colour >> 8) & 0x0FF, colour & 0xFF, print_func);
}

inline uint32_t
ansi_colour_code_to_bgra(int code)
{
	switch(code)
	{
		case BLACK :					return 0x0FF000000;
		case RED :						return 0x0FFFF0000;
		case GREEN :					return 0x0FF00FF00;
		case YELLOW :					return 0x0FFFFFF00;
		case BLUE :						return 0x0FF0000FF;
		case MAGENTA :				return 0x0FFFF00FF;
		case CYAN :						return 0x0FF00FFFF;
		case LIGHT_GREY :			return 0x0FF808080;
		case DARK_GREY :			return 0x0FF404040;
		case LIGHT_RED :			return 0x0FFFF8080;
		case LIGHT_GREEN :		return 0x0FF80FF80;
		case LIGHT_YELLOW :		return 0x0FFFFFF80;
		case LIGHT_BLUE :			return 0x0FFFF8080;
		case LIGHT_MAGENTA :	return 0x0FFFF80FF;
		case LIGHT_CYAN :			return 0x0FF80FFFF;
		case WHITE :					return 0x0FFFFFFFF;
	}
	return 0x0FFFFFFFF;
}


} // namespace ansi

class ANSIDecode
{
private:
	enum
	{
		STATE_IDLE,
		STATE_ESCAPE,
		STATE_ARG
	};


	static const uint32_t 		MAX_ARGS 						= 16;
	int												m_args[MAX_ARGS]		= {0};
	uint32_t									m_argc							= 0;
	int												m_arg								= 0;
	int												m_state							= STATE_IDLE;

public:
	int												decode(int ch,std::function<int(int,std::span<int const>)> callback);
	std::optional<char32_t>		decode_u32(char32_t ch,std::function<int(int,std::span<int const>)> callback);

private:
	void				add_pending_arg(/*bool allow_empty = false*/); //FIXME
	void				set_state(int state);

};

inline void
ANSIDecode::set_state(int state)
{
	m_state = state;
	if(state == STATE_IDLE)
		m_argc = 0;
}

inline void
ANSIDecode::add_pending_arg(/*bool allow_empty*/)	//FIXME
{
	if(m_argc < MAX_ARGS)
		m_args[m_argc++] = m_arg;
	m_arg = 0;
}

inline int 
ANSIDecode::decode(int ch,std::function<int(int,std::span<int const>)> callback)
{
	if(ch < 0)
		return ch;

	switch(m_state)
	{
		case STATE_IDLE : 		if(ch == 27) 	{set_state(STATE_ESCAPE);ch = -1;}  	break;
		case STATE_ESCAPE :		if(ch == '[') {set_state(STATE_ARG);ch = -1;} 			break;
		case STATE_ARG :
			{
				int rch = -1;
				if(isdigit(ch))
					m_arg = (m_arg*10) + static_cast<int>(ch - '0');
				else if(ch == ';')
					add_pending_arg();
				else if(isalpha(ch))
				{
					add_pending_arg(); 
					rch = callback(ch,std::span<int const>(&m_args[0],m_argc));
					set_state(STATE_IDLE); 
				}
				else
					set_state(STATE_IDLE); 
				ch = rch;
			}
			break;

		default :	break;	
	}

	return ch;
}



//=============================================================================
//
//	ANSI DECODE U32
//
//=============================================================================

class ANSIDecodeU32
{
private:
	enum
	{
		STATE_IDLE,
		STATE_ESCAPE,
		STATE_ARG
	};

	static const uint32_t 		MAX_ARGS 						= 16;
	char32_t									m_arg_prefix				= 0;
	int												m_args[MAX_ARGS]		= {0};
	uint32_t									m_argc							= 0;
	int												m_arg								= 0;
	int												m_state							= STATE_IDLE;
	bool											m_b_have_arg				= false;

public:
	std::optional<char32_t>		decode(char32_t ch,std::function<void(char32_t,std::span<int const>, char32_t arg_prefix)> callback);

private:
	void				add_pending_arg(bool b_force = false);
	void				set_state(int state);

};

inline void
ANSIDecodeU32::set_state(int state)
{
	m_state = state;
	if(state == STATE_IDLE)
	{
		m_argc 				= 0;
		m_arg_prefix 	= 0;
		m_b_have_arg	= false;
	}
}

inline void
ANSIDecodeU32::add_pending_arg(bool b_force)	
{
	if(m_argc < MAX_ARGS)
	{
		if(m_b_have_arg)
			m_args[m_argc++] = m_arg;
		else if(b_force)
			m_args[m_argc++] = 0;
	}
	m_arg = 0;
	m_b_have_arg = false;
}

inline
std::optional<char32_t>
ANSIDecodeU32::decode(char32_t ch,std::function<void(char32_t,std::span<int const>, char32_t private_mode)> callback)
{
	switch(m_state)
	{
		case STATE_IDLE : 		if(ch == 27) 	{set_state(STATE_ESCAPE); return {};} break;
		case STATE_ESCAPE :		if(ch == '[') {set_state(STATE_ARG); return {};} 		break;
		case STATE_ARG :			if(isdigit(ch))				{ m_arg = (m_arg*10) + static_cast<int>(ch - '0'); m_b_have_arg=true; } 
													else if(isalpha(ch))	{add_pending_arg(); callback(ch,{&m_args[0],m_argc},m_arg_prefix);	set_state(STATE_IDLE); }
													else switch(ch)
													{
														case ';' :			add_pending_arg(true); m_b_have_arg=true; m_arg=0; break;
														case '?' : 			[[fallthrough]];
														case '>' :			[[fallthrough]];
														case '<' :			[[fallthrough]];
														case '#' :			[[fallthrough]];
														case '=' :			if(!m_b_have_arg && (m_argc==0)) m_arg_prefix = ch; break;
														default :				set_state(STATE_IDLE); break;
													}
													return {};
		default :							set_state(STATE_IDLE); break;
	}

	return ch;
}



#endif // ! defined GUARD_ADE_ANSI_H
