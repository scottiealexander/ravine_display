// #include <cstdlib>
#include <cctype>
#include <vector>
#include <string>
#include <fstream>

#include "parse_ini.hpp"

void parse_array(const std::string& str, std::vector<float>& values)
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
            values.push_back(std::atof(str.substr(ks, ke - ks).c_str()));
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

bool parse_ini(
    const char* filename,
    PMap& gs,
    std::string& varying,
    std::vector<float>& values
)
{
    bool success = true;
    std::ifstream is;
    is.open(filename, std::ifstream::in);

    if (!is.good())
    {
        return false;
    }

    std::string buf;

    while (std::getline(is, buf, '\n'))
    {
        bool valid;

        int ks = 0;
        while (ks < buf.length() && std::isspace(buf[ks])) { ++ks; }

        int ke = ks + 1;
        int k = ke;
        while (k < buf.length())
        {
            if (std::isspace(buf[k]))
            {
                ++k;
            }
            else if (buf[k] == '=')
            {
                break;
            }
            else
            {
                ++ke;
                ++k;
            }
        }

        if (k < buf.length())
        {
            std::string key = buf.substr(ks, ke - ks);

            int vs = k + 1;
            while (vs < buf.length() && std::isspace(buf[vs])) { ++vs; }

            int ve = buf.length() - 1;
            while (ve > vs && std::isspace(buf[ve])) { --ve; }

            if (ve < buf.length() + 1)
            {
                std::string val = buf.substr(vs, ve - vs + 1);

                // if key is a default param, add to grating spec
                PMap::iterator it = gs.find(key);
                if (it != gs.end())
                {
                    gs[key] = std::atof(val.c_str());
                }
                else if (key == "varying")
                {
                    varying = val;
                }
                else if (key == "values")
                {
                    // parse array of numbers
                    parse_array(val, values);
                }
            }
        }
    }

    is.close();

    return success;
}
