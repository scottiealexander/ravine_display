// #include <cstdlib>
#include <cctype>
#include <vector>
#include <string>
#include <fstream>
#include <cinttypes>

#include "parse_ini.hpp"

bool parse_ini(const char* filename, PMap& gs,
    std::string& varying, std::vector<float>& values,
    std::string& host_ip, std::vector<uint8_t>& triggers
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
                else if (key == "host-ip")
                {
                    host_ip = val;
                }
                else if (key == "values")
                {
                    // parse array of numbers
                    parse_array<float>(val, values);
                }
                else if (key == "triggers")
                {
                    // annoyingly, uint8_t are chars, so when we ask parse_array
                    // to use uint8_t elements, is uses char's and we get the
                    // ascii code (e.g. 49 for 1), instead of the number...
                    std::vector<int> tmp;

                    parse_array<int>(val, tmp);

                    // finally, cast to uint8_t as we copy...
                    triggers.resize(tmp.size());
                    for (int k = 0; k < tmp.size(); ++k)
                    {
                        triggers[k] = (uint8_t)tmp[k];
                    }
                }
            }
        }
    }

    is.close();

    return success;
}
