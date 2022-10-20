//=============================================================================
//  FILE:           program_options.h
//  SYSTEM:         
//  DESCRIPTION:
//-----------------------------------------------------------------------------
//  COPYRIGHT:      (C)Copyright 2017 Adrian Purser. All Rights Reserved.
//  LICENCE:        MIT
//  MAINTAINER:     AJP - Adrian Purser <ade@arcadestuff.com>
//  CREATED:        19-JUN-2009 Adrian Purser <ade@arcadestuff.com>
//=============================================================================
#ifndef GUARD_ADE_PROGRAM_OPTIONS_H
#define GUARD_ADE_PROGRAM_OPTIONS_H

#include <cstdlib>
#include <string>
#include <map>
#include <vector>
#include <cassert>
#include <iostream>
#include <istream>
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <memory>

namespace program_options  
{

//*****************************************************************************
//
//	OPTION
//
//*****************************************************************************
struct Option
{
	char					short_name;
	std::string		long_name;
	std::string		description;
	std::string		value_name;
	std::string		default_value;

	Option() : short_name(0){}
	Option(	char								short_name_,
					const std::string &	long_name_,
					const std::string & description_,
					const std::string 	value_name_="",
					const std::string 	default_value_="" )
		: short_name(short_name_)
		, long_name(long_name_)
		, description(description_)
		, value_name(value_name_)
		, default_value(default_value_) 
		{}

	Option(	const std::string &	name_,
					const std::string &	description_,
					const std::string	value_name_="",
					const std::string	default_value_="" )
		: short_name(0)
		, description(description_)
		, value_name(value_name_)
		, default_value(default_value_) 
		{
			std::string::size_type pos = name_.find_last_of(',');

			if(pos == std::string::npos)
				long_name = name_;
			else 
			{
				long_name = name_.substr(0,pos);
				if(pos != (name_.size()-1))
					short_name = name_.at(pos+1);
			}
		}

	std::string help_name() const
	{
		std::string name;
		
		//---------------------------------------------------------------------
		//	Short Name
		//---------------------------------------------------------------------
		if(short_name)
		{
			name.append("-");
			name.push_back(short_name);
			name.append(", ");
		}
		else
			name.append("    ");

		//---------------------------------------------------------------------
		//	Long Name
		//---------------------------------------------------------------------
		name.append("--");
		name.append(long_name);

		if(!value_name.empty())
		{
			if(default_value.empty())
			{
				name.append("=");
				name.append(value_name);
			}
			else
			{
				name.append("[=");
				name.append(value_name);
				name.append("]");
			}
		}

		return name;
	}

};

//*****************************************************************************
//
//	OPTION GROUP
//
//*****************************************************************************
class OptionGroup : public std::vector<Option>
{

public:
	OptionGroup(void){}
	~OptionGroup(){}

	void add_option(const std::string & name,const std::string & description,const std::string & value_name="",const std::string default_value="")
	{
		Option opt(name,description,value_name,default_value);
		push_back(opt);
	}
};


//*****************************************************************************
//
//	PARSER 
//
//*****************************************************************************
typedef std::string																OptionValue;
typedef std::vector<OptionValue>									ProgramOptionValues;
typedef std::map<std::string,ProgramOptionValues>	ProgramOptions;
typedef std::vector<OptionValue>									ProgramArgs;

struct ParserResults
{
	ProgramOptions	options;
	ProgramArgs		args;
};

class Parser 
{
private:
	typedef std::shared_ptr<Option>								option_ptr_t;
	typedef std::map<char,option_ptr_t>						short_lookup_table_t;
	typedef std::map<std::string,option_ptr_t>		long_lookup_table_t;
	typedef std::vector<option_ptr_t>							option_list_t;
	typedef std::map<std::string,option_list_t>		group_array_t;

	short_lookup_table_t			m_short_lookup;
	long_lookup_table_t				m_long_lookup;
	group_array_t							m_groups;
	option_list_t							m_non_grouped_options;
	std::string								m_error_string;
	bool											m_options_enabled		= true;	
	bool											m_b_fail						= false;
	
private:
	void	set_error(const std::string & err_str) {m_b_fail = true; m_error_string = err_str;}
	
	std::string::size_type
	parse_string(const std::string & in_string,std::string::size_type pos,std::string & out_string)
	{
		std::string::size_type len = in_string.length();
		bool quotes = false;
		bool escape = false;	
		bool finished = false;	

#ifdef PROGRAM_OPTIONS_ENABLE_LOGGING
		std::clog << in_string << std::endl;
#endif

		while(!finished && (pos<len))
		{
			unsigned char ch = in_string.at(pos++);

			if(escape)
			{
				out_string.push_back(ch);
				escape = false;
				continue;
			}

			switch(ch)
			{
				case	13 :
				case	10 :
					finished = true;
					break;

				case	32 :
				case	9 :
					if(quotes)
						out_string.push_back(ch);
					else
						finished = true;
					break;

				case	'"' :
					//out_string.push_back(ch);
					quotes = !quotes;
					break;

#ifndef PROGRAM_OPTIONS_NO_ESCAPE
				case	'\\' :
					escape = true;
					break;
#endif // ! defined PROGRAM_OPTIONS_NO_ESCAPE

				default :
					if((ch>32) && (ch<128))
						out_string.push_back(ch);
					break;
			}
		}
		return pos;
	}

public:
	Parser( const Parser & ) = delete;
	const Parser & operator=( const Parser & ) = delete;
	Parser(){}
	~Parser(){}

	const std::string & error_string() const noexcept {return m_error_string;}
	
	bool is_fail() const noexcept {return m_b_fail;}
	bool option_exists(char short_name) const {return !!m_short_lookup.count(short_name);}
	bool option_exists(const std::string & long_name) const {return !!m_long_lookup.count(long_name);}

	int	add_option(const std::string & name,const std::string & description,const std::string & value_name="",const std::string default_value="")
	{
		Option opt(name,description,value_name,default_value);
		return add_option(opt);
	}

	int add_option(const Option & opt)
	{
		if(option_exists(opt.short_name) || option_exists(opt.long_name))	
			return -1;

		option_ptr_t p_option(new Option(opt));

		if(opt.short_name)
			m_short_lookup[opt.short_name] = p_option;
		m_long_lookup[opt.long_name] = p_option;
		m_non_grouped_options.push_back(p_option);

		return 0;
	}

	int add_group(const std::string & group_name,OptionGroup & group)
	{
		if(group.empty())
			return 0;

		if(m_groups.count(group_name))
			return -1;

		m_groups[group_name] = option_list_t();
		auto igroup = m_groups.find(group_name);

		if(igroup == m_groups.end())
			return -1;

		bool err = false;

		for(auto & opt : group)
		{
			if(option_exists(opt.short_name) || option_exists(opt.long_name))	
				err = true;
			else
			{
				option_ptr_t p_option(new Option(opt));

				if(opt.short_name)
					m_short_lookup[opt.short_name] = p_option;
				m_long_lookup[opt.long_name] = p_option;
				igroup->second.push_back(p_option);
			}
		}

		return (err ? -1:0);
	}

	std::string::size_type 
	parse_option(	const std::string &			args,
								std::string::size_type	pos,
								ParserResults &					out_results )
	{
		bool										finished = false;
		bool										firstch = true;
		std::string::size_type	len = args.length();

		while(!finished && (pos<len))
		{
			unsigned char	ch = args.at(pos++);
			option_ptr_t	p_option;			

			//-----------------------------------------------------------------
			//	If the character is a '-' and it's the first character then 
			//	the option name is a long name.
			//-----------------------------------------------------------------
			if(firstch && (ch=='-'))
			{
				std::string optname;
				bool fin=false;
			
				//-------------------------------------------------------------
				//	Extract the option name from the input string.
				//-------------------------------------------------------------
				while((pos<len) && !fin)
				{
					ch = args.at(pos++);

					if((ch<=32) || (ch=='='))
						fin = true;
					else
						optname.push_back(ch);
				}

				if(optname.empty())
				{
					//-------------------------------------------------------------
					// The option name is empty (i.e. '--'). This special option name 
					// is used to mark the end of options so we disable further option
					// parsing.
					//-------------------------------------------------------------
					m_options_enabled = false;
				}
				else
				{
					//-------------------------------------------------------------
					//	Search for the option name in the list of valid options.
					//-------------------------------------------------------------
					auto ilopt = m_long_lookup.find(optname);
					if(ilopt == m_long_lookup.end())
						set_error(std::string("Invalid Option : ")+optname);
					else
						p_option = ilopt->second;
				}
							
				finished = true;
			}
			//-----------------------------------------------------------------
			//	If the character is in the range 33-127 then is is a short name.
			//-----------------------------------------------------------------
			else if((ch>32) && (ch<128))
			{
				auto isopt = m_short_lookup.find(ch);
				if(isopt == m_short_lookup.end())
				{
					std::string errmsg("Invalid Option : ");
					errmsg.push_back(ch);
					set_error(errmsg);
				}
				else
					p_option = isopt->second;			
			}
			//-----------------------------------------------------------------
			//	Any other character terminates the option parsing.
			//-----------------------------------------------------------------
			else
				finished = true;
			firstch = false;

			//-----------------------------------------------------------------
			//	If we found a valid option then act upon it.
			//-----------------------------------------------------------------
			if(!is_fail() && p_option)
			{
				std::string value;

				//-------------------------------------------------------------
				//	If the option requires a value then try to read it. 
				//-------------------------------------------------------------
				if(!p_option->value_name.empty())
				{
					while((pos<len) && (args.at(pos)<=32))
						++pos;
					pos = parse_string(args,pos,value);
					finished = true;
				}
				out_results.options[p_option->long_name].push_back(value);
			}
		}

		return pos;
	}

	int parse(const std::string & args,ParserResults & out_results)
	{
		std::string::size_type	pos = 0;
		bool										finished = false;
		std::string::size_type	len = args.length();

		while(!finished && !is_fail() && (pos<len))
		{
			unsigned char ch = args.at(pos++);

			switch(ch)
			{
				//-------------------------------------------------------------
				case 32 :	// Ignore These Characters
				case 9 :	
				case 13 :
				case 10 :
				//-------------------------------------------------------------
					break;

				//-------------------------------------------------------------
				case '@' :	// Load options from file.
				//-------------------------------------------------------------
					{
						std::string filename;
						pos = parse_string(args,pos,filename);

						if(filename.empty())
							set_error("Missing Response File Name!");
						else
						{
							std::string infile;
							std::ifstream file(filename.c_str());
							
							//-----------------------------------------------------
							//	Read the config file line-by-line. 
							//	Lines starting with a '#' (ignoring whitespace) are 
							//	comments and are ignored.
							//-----------------------------------------------------
							while(file && !file.fail())
							{
								std::string line;
								std::getline(file,line);
								
								if(!line.empty())
								{
									std::string::size_type wspos = line.find_first_not_of(" \t");
									if(wspos != std::string::npos)
									{
										if(line[wspos] != '#')
										{
											infile.append(line);
											infile.push_back(' ');
										}
									}
								}
							}

							//-----------------------------------------------------
							//	If the config file wasn't empty then parse it.
							//-----------------------------------------------------
							if(!infile.empty())
								parse(infile,out_results);
						}
					}
					break;
					
				//-------------------------------------------------------------
				default :
				//-------------------------------------------------------------
					if(m_options_enabled && (ch == '-'))
					{
						if(pos<len)
							pos = parse_option(args,pos,out_results);
						else 
							set_error("Missing Option");
						break;						
					}
					else
					{
						std::string arg;
						pos = parse_string(args,pos-1,arg);
						out_results.args.push_back(arg);
					}
					break;
			}
		}

		if(!is_fail())
		{
			//---------------------------------------------------------------------
			//	If any options that have a default value have not been added to the 
			//	values object already then add them with the default value. 
			//---------------------------------------------------------------------
			for(auto & lookup : m_long_lookup)
				if(!(lookup.second->default_value.empty()) && !out_results.options.count(lookup.second->long_name))
					out_results.options[lookup.second->long_name].push_back(lookup.second->default_value);
		}
		
		return is_fail() ? -1 : 0;
	}
	
	int parse(int argc,const char ** argv,ParserResults & out_results)
	{
		std::string args;

		for(int i=1;i<argc;++i)
		{
			std::string arg(argv[i]);

			if( (arg.find_first_of(' ')!=std::string::npos)	|| (arg.find_first_of((char)9)!=std::string::npos) )
			{
				if(argv[i][0]=='@')
				{
					args.push_back('@');
					args.push_back('\"');
					args.append(std::string(&argv[i][1]));
					args.push_back('\"');
					args.push_back(' ');
				}
				else
					args = args + std::string("\"") + std::string(argv[i]) + std::string("\" ");
			}
			else
				args = args + std::string(argv[i]) + std::string(" ");
		}

#ifndef PROGRAM_OPTIONS_NO_ESCAPE
		std::replace(args.begin(),args.end(),'\\','/');
#endif

		return parse(args,out_results);
	}

	int parse(const ProgramArgs & args,ParserResults & out_results)
	{
		std::string argstr;
		
		for(const auto & a : args)
		{
			argstr.append(a);
			argstr.push_back(' ');
		}
		
		return parse(argstr,out_results);
	}
	
	friend std::ostream & operator<<(std::ostream & stream, Parser & parser);

};

//*****************************************************************************
//
//	OPERATOR << OVERLOADING
//
//*****************************************************************************

std::ostream & operator<<(std::ostream & stream, Parser & parser)
{
	//-------------------------------------------------------------------------
	//	Calculate the length of the longest name.
	//-------------------------------------------------------------------------
	int name_width = 0;
	for(auto & lookup_pair : parser.m_long_lookup)
	{
		int len = (int)(lookup_pair.second->help_name().length());
		if(len > name_width)
			name_width = len;
	}

	name_width += 4;

	//-------------------------------------------------------------------------
	//	Write the option groups to the stream.
	//-------------------------------------------------------------------------
	for(auto & group_pair : parser.m_groups)
	{
		stream << "\n" << group_pair.first << ":\n\n";

		for(auto & p_opt : group_pair.second)
		{
			const Option & opt = *p_opt;	
			stream << "  " << std::setw(name_width) << std::left << opt.help_name();

			//-----------------------------------------------------------------
			//	Write the description. If the description spans multiple lines
			//	then we need to pad the remaining lines to match the first line.
			//-----------------------------------------------------------------
			std::string padding;
			std::stringstream desc(opt.description);
			while(!desc.eof())
			{
				std::string dline;
				std::getline(desc,dline);
				stream << padding << dline << "\n";
				if(padding.empty())
					for(int i=name_width+2;i>0;--i)
						padding.push_back(' ');
			}

			//-----------------------------------------------------------------
			//	If there is a default value then write it to the output stream.
			//-----------------------------------------------------------------
			if(!opt.default_value.empty())
				stream << "  " << std::setw(name_width) << " " << "(default " << opt.default_value << ")\n";
		}
	}

	//-------------------------------------------------------------------------
	//	Write the ungrouped options to the stream.
	//-------------------------------------------------------------------------
	if(!parser.m_non_grouped_options.empty())
	{
		stream << "\nOther options:\n\n";

		for(auto & p_opt : parser.m_non_grouped_options)
		{
			const Option & opt = *p_opt;	
			stream << "  " << std::setw(name_width) << std::left << opt.help_name();
			
			//-----------------------------------------------------------------
			//	Write the description. If the description spans multiple lines
			//	then we need to pad the remaining lines to match the first line.
			//-----------------------------------------------------------------
			std::string padding;
			std::stringstream desc(opt.description);
			while(!desc.eof())
			{
				std::string dline;
				std::getline(desc,dline);
				stream << padding << dline << "\n";
				if(padding.empty())
					for(int i=name_width+2;i>0;--i)
						padding.push_back(' ');
			}

			//-----------------------------------------------------------------
			//	If there is a default value then write it to the output stream.
			//-----------------------------------------------------------------
			if(!opt.default_value.empty())
				stream << "  " << std::setw(name_width) << " " << "(default " << opt.default_value << ")\n";
		}
	}

	return stream;
}

} // namespace program_options

#endif // ! defined GUARD_ADELIBRARY_PROGRAM_OPTIONS_H


