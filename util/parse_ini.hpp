#ifndef PARSE_INI_HPP_
#define PARSE_INI_HPP_

#include <sstream>
#include <string>
#include <vector>
#include <map>

/* -------------------------------------------------------------------------- */
using PMap = std::map<std::string, float>;
/* -------------------------------------------------------------------------- */
bool parse_ini(const char*, PMap&, std::string&, std::vector<float>&, std::vector<uint8_t>&);
/* -------------------------------------------------------------------------- */
// void parse_array(const std::string&, std::vector<float>&);
template <typename T>
void parse_array(const std::string& str, std::vector<T>& values)
{
    int k = 0;
    while (k < str.length() && (std::isspace(str[k]) || str[k] == '[')) { ++k; }

    int ks = k;
    int ke = ks + 1;
    k = ke;
    while (k < str.length())
    {
        if (str[k] == ',' || str[k] == ']')
        {
            T val;
            std::istringstream ss(str.substr(ks, ke - ks));
            ss >> val;
            values.push_back(val);

            ++k;
            while (k < str.length() && std::isspace(str[k])) { ++k; }
            ks = k;
            ke = k + 1;
        }
        else if (!std::isspace(str[k]))
        {
            ++ke;
        }

        ++k;
    }
}
/* -------------------------------------------------------------------------- */
#endif
