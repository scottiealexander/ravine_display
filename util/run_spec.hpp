#ifndef RUN_SPEC_HPP_
#define RUN_SPEC_HPP_

#include <string>
#include <vector>
#include <map>

#include "parse_ini.hpp"

class RunSpec
{
public:
    RunSpec(const char* filename);
    float get(const std::string& key);
    int length();
    const std::string& get_varying() { return varying; };
    void show();
    bool isvalid() { return valid; }
private:
    bool valid;
    PMap g_param;
    std::string varying;
    std::vector<float> values;
    int ptr;
};

#endif
