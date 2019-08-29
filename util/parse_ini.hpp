#ifndef PARSE_INI_HPP_
#define PARSE_INI_HPP_

#include <string>
#include <vector>
#include <map>

using PMap = std::map<std::string, float>;

void parse_array(const std::string&, std::vector<float>&);
bool parse_ini(const char*, PMap&, std::string&, std::vector<float>&);

#endif
