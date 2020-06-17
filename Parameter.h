#include "json.hpp"
#include <string>

using DT = nlohmann::json;

/**
 * `template<typename T> class Parameter` is not preferred, 
 * because mixed type container, i.e. `std::map<std::string, Parameter>` is desired. 
 * Meanwhile, some C++ object can hold value of dynamic types with type info accessible, 
 * such as `Poco::Dynamic::Var` or `std::any`. Here nlohmann::json type are used
 * */
struct Parameter
{
    //  as dict key
    std::string name; // short name, potential can be used to gen code,
    std::string desc; // description/ doc string
    // if std::any is used, then data type enum/string is needed,
    // or detect from json, while advanced data type std::vector<ET>
    DT value;         // T any type supported by json, can be std::vector<ET>
    std::string unit; // empty if it is a non-unit
};

// parameter table, group, should have meta data
class ParameterTable
{
    std::map<std::string, Parameter> m_parameters;

public:
    ParameterTable(){};
};