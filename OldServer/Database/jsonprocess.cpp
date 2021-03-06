#include "jsonprocess.h"

std::string JsonSpliced(std::initializer_list<std::string> stril)
{
    if (stril.size() % 2)
        return "error";
    std::string json = "{";
    for (auto i = stril.begin(); i != stril.end(); i += 2)
    {
        std::string key = *i;
        std::string value = *(i + 1);
        json += ("\"" + key + "\":");
        json += ("\"" + value + "\"");
        if (i + 2 == stril.end())
        {
            json += "}";
        }
        else
        {
            json += ",";
        }
        
    }
    return json;
}