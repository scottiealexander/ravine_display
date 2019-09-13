#ifndef RUN_SPEC_HPP_
#define RUN_SPEC_HPP_

#include <cinttypes>
#include <string>
#include <vector>
#include <map>

#include "parse_ini.hpp"

class RunSpec
{
public:
    RunSpec(const char* filename);
    float get(const std::string& key);

    inline uint8_t next_trigger() const
    {
        return triggers[ptr % triggers.size()];
    }

    int length();
    const std::string& get_varying() { return varying; };
    void show();
    bool isvalid() const { return valid; }
private:
    bool valid;
    PMap g_param;
    std::string varying;
    std::vector<float> values;
    std::vector<uint8_t> triggers;
    int ptr;
};

#endif
