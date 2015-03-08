#ifndef INVENTORY_BACK_HPP
#define INVENTORY_BACK_HPP
#include <string>
#include <fstream>
#include <iomanip>
std::string decodeURI(std::string src)
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

std::string encodeURI(std::string value)
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

void uEnc(std::string& str) { str = encodeURI(str); }
void uDec(std::string& str) { str = decodeURI(str); }

std::string read_file(std::string location)
{
    std::ifstream fin(location.c_str());
    if(fin.is_open()) return dev::toString(fin.rdbuf());
    else return "Could not open file!";
}
void write_file(std::string location, std::string data)
{
    std::ofstream fout(location.c_str());
    if(fout.is_open()) fout << data;
    fout.close();
    return;
}
#endif // INVENTORY_BACK_HPP
