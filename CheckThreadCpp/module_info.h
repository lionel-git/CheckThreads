#pragma once
#include <string>

class module_info
{
public:
    std::string name;
    std::string executable_path;
    int process_id;
    unsigned long long base_address;
    unsigned long module_size;
};

