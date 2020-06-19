#pragma once

#include "toml11/toml.hpp"
#include "Parameter.h"

#include "EERA_types.h"

//////////////////////// example from toml doc //////////////////////
namespace ext
{
    struct foo
    {
        int a;
        double b;
        std::string c;
    };
} // namespace ext

namespace toml
{
    template <>
    struct from<ext::foo>
    {
        static ext::foo from_toml(const value &v)
        {
            ext::foo f;
            f.a = find<int>(v, "a");
            f.b = find<double>(v, "b");
            f.c = find<std::string>(v, "c");
            return f;
        }
    };

    template <>
    struct into<ext::foo>
    {
        static toml::table into_toml(const ext::foo &f)
        {
            return toml::table{{"a", f.a}, {"b", f.b}, {"c", f.c}};
        }
    };
} // namespace toml
