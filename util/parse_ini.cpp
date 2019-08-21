#include <cstdlib>
#include <cctype>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>


#include "parse_ini.hpp"

using PMap = std::map<std::string, float>;

PMap default_grating()
{
    PMap gs;
    gs["orientation"] = 0.0f;
    gs["spatial_frequency"] = 0.1f;
    gs["temporal_frequency"] = 1.0f;
    gs["contrast"] = 1.0f;
    gs["radius"] = 200.0f;
    gs["duration"] = 2.0f;
    gs["blank_duration"] = 1.0f;
    gs["repeats"] = 1.0f;

    return gs;
}

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

RunSpec::RunSpec(const char* filename) : g_param(default_grating()), ptr(0)
{
    valid = parse_ini(filename, g_param, varying, values);
}

float RunSpec::get(const std::string& key)
{
    bool ret = true;
    float out;
    if (key == varying)
    {
        out = values[ptr % values.size()];
        ++ptr;
    }
    else
    {
        PMap::iterator it = g_param.find(key);
        if (it != g_param.end())
        {
            out = it->second;
        }
        else
        {
            ret = false;
        }
    }

    return out;
}

int RunSpec::length()
{
    return values.size() * (int)g_param["repeats"];
}

void RunSpec::show()
{
    std::cout << "Ori: " << g_param["orientation"] << std::endl;
    std::cout << "Sf: " << g_param["spatial_frequency"] << std::endl;
    std::cout << "Tf: " << g_param["temporal_frequency"] << std::endl;
    std::cout << "Con: " << g_param["contrast"] << std::endl;
    std::cout << "Rad: " << g_param["radius"] << std::endl;
    std::cout << "Dur: " << g_param["duration"] << std::endl;
    std::cout << "Blank: " << g_param["blank_duration"] << std::endl;
    std::cout << "Rep: " << g_param["repeats"] << std::endl;
}
