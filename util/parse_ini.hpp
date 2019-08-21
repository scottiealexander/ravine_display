#ifndef PARSE_INI_HPP_
#define PARSE_INI_HPP_

#include <string>
#include <map>

class RunSpec
{
public:
    RunSpec(const char* filename);
    float get(const std::string& key);
    int length();
private:
    std::map<std::string, float> g_param;
    std::string varying;
    std::vector<float> values;
    int ptr;
};

#endif
