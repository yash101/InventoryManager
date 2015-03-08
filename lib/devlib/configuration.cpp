#include "configuration.hpp"
#include "string.hpp"
#include <fstream>
#include <iostream>
#include <cstring>
#include "buildconf.h"
#include <iomanip>

std::string decodeURL(std::string src)
{
    std::stringstream o;
    for(size_t i = 0; i < src.size(); i++)
    {
        if(src[i] == '%')
        {
            int ii;
            sscanf(src.substr(i + 1, 2).c_str(), "%x", &ii);
            char ch = static_cast<char>(ii);
            i += 2;
            o << (char) ch;
        }
        else if(src[i] == '+') o << ' ';
        else o << src[i];
    }
    return o.str();
}

std::string encodeURL(std::string value)
{
    std::ostringstream escaped;
    escaped.fill('0');
    escaped << std::hex;
    for(size_t i = 0; i < value.size(); i++)
    {
        char c = value[i];
        if(isalnum(c) || c == '=' || c == '_' || c == '.' || c == '~') escaped << c;
        else escaped << '%' << std::setw(2) << int((unsigned char) c);
    }
    return escaped.str();
}

//%%%%%%%%%%%%
//  Constructor. Takes in configuration file location. If a file does not exist, it will
//      be created!
//%%%%%%%%%%%%
dev::ConfigurationParser::ConfigurationParser(std::string location) : _location(location)
{
    refresh();
}

void dev::ConfigurationParser::operator()(std::string location)
{
    _location = location;
    refresh();
}

//%%%%%%%%%%%%
//  Reads the configuration file into the map
//%%%%%%%%%%%%
void dev::ConfigurationParser::refresh()
{
    //Erase the contents of _cfdata
    _cfdata.clear();

    //Reads the configuration file
    std::ifstream fin(_location.c_str(), std::ios::in);

    //If the file does not exist, create a new file!
    if(!fin.is_open()) std::ofstream(_location.c_str(), std::ios::out).close();

    std::string buffer;
    //Extract each line!
    while(std::getline(fin, buffer, '\n'))
    {
        //Check if the index of the hash does exist! If it exists, get it out of the way!
        if(dev::charPos(buffer, '#') != -1) buffer = buffer.substr(0, dev::charPos(buffer, '#') - 1);
        dev::trim(buffer);
        //Check to make sure that it isn't a line with only comments!
        if(buffer.size() != 0)
        {
            std::string key, value;
            if(dev::charPos(buffer, '=') == -1) key = buffer;
            else
            {
                size_t eqloc = dev::charPos(buffer, '=');
                key = buffer.substr(0, eqloc);
                value = buffer.substr(eqloc + 1, buffer.size());
            }
            dev::trim(key);
            dev::trim(value);
            _cfdata[decodeURL(key)] = decodeURL(value);
        }
    }

    fin.close();
}

void dev::ConfigurationParser::flush()
{
    std::ofstream fout(_location.c_str(), std::ios::out);
    if(fout.is_open())
    {
        for(std::map<std::string, std::string>::const_iterator it = _cfdata.begin(); it != _cfdata.end(); ++it)
        {
            fout << encodeURL(it->first) << "=" << encodeURL(it->second) << std::endl;
        }
    }

    fout.close();
}

std::string dev::ConfigurationParser::get(std::string key)
{
    return _cfdata[key];
}
void dev::ConfigurationParser::add(std::string key, std::string value)
{
    _cfdata[key] = value;
}
void dev::ConfigurationParser::remove(std::string key)
{
    _cfdata.erase(key);
}
std::string& dev::ConfigurationParser::operator[](std::string key) { return _cfdata[key]; }

bool dev::ConfigurationParser::tryBool(std::string key)
{
    std::string b = dev::itrim(dev::tolower(_cfdata[key]));
    return b == "yes" || b == "1" || b == "true" || b == "on";
}

#ifndef CONFIGURATION_LOCATION
#define CONFIGURATION_LOCATION "app.cfg"
#endif

class dev::ConfigurationParser parser(CONFIGURATION_LOCATION);
class dev::ConfigurationParser& dev::getConfigurationParser() { return parser; }
