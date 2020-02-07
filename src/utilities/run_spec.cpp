#include <iostream>
#include <vector>
#include <string>

#include "parse_ini.hpp"
#include "run_spec.hpp"

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
    gs["host-port"] = -1.0f;

    return gs;
}

RunSpec::RunSpec(const char* filename) : g_param(default_grating()), ptr(0)
{
    valid = parse_ini(filename, g_param, varying, values, host_ip, triggers);
    if (values.size() != triggers.size())
    {
        valid = false;
    }
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
    return values.size() * static_cast<int>(g_param["repeats"]);
}

void RunSpec::show()
{
    std::cout << "Ori:   " << g_param["orientation"] << std::endl;
    std::cout << "Sf:    " << g_param["spatial_frequency"] << std::endl;
    std::cout << "Tf:    " << g_param["temporal_frequency"] << std::endl;
    std::cout << "Con:   " << g_param["contrast"] << std::endl;
    std::cout << "Rad:   " << g_param["radius"] << std::endl;
    std::cout << "Dur:   " << g_param["duration"] << std::endl;
    std::cout << "Blank: " << g_param["blank_duration"] << std::endl;
    std::cout << "Rep:   " << g_param["repeats"] << std::endl;
}
