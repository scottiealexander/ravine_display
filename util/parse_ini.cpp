
#include <cctype>
#include <cstdlib>

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

    return gs;
}

RunSpec::RunSpec(const char* filename) : g_param(default_grating()), ptr(0)
{
    bool success = parse_ini(filename, g_param, varying, values);
}

bool RunSpec::get(const std::string& key, float& out)
{
    bool ret = true;
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

    return ret;
}

int RunSpec::length()
{
    return valuse.size() * (int)g_param["repeats"];
}

void parse_array(const std::string& str, std::vector<float>& values)
{
    int k = 0;
    while (k < str.length() && (isspace(str[k]) || str[k] == '[')) { ++k; }

    int ks = k;
    int ke = ks + 1;
    k = ke;
    while (k < str.length())
    {
        if (str[k] == ',' || str[k] == ']')
        {
            values.push_back(std::atof(str.substr(ks, ke - ks).c_str()));
            ++k;
            while (k < str.length() && isspace(str[k])) { ++k; }
            ks = k;
            ke = k + 1;
        }
        else if (!isspace(str[k]))
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

    std::string buf;

    while (std::getline(is, buf))
    {
        bool valid;

        int ks = 0;
        while (ks < buf.length() && isspace(buf[ks])) { ++ks; }

        int ke = ks + 1;
        int k = ke;
        while (k < buf.length())
        {
            if (isspace(buf[k]))
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
            while (vs < buf.length() && isspace(buf[vs])) { ++vs; }

            int ve = vs + 1;
            while (ve < buf.length() && !isspace(buf[ve])) { ++ve; }

            if (ve < buf.length() + 1)
            {
                std::string val = buf.substr(vs, ve - vs);

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
    return success;
}
