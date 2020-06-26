#pragma once

#include <string>
#include <map>
#include <vector>
#include <sstream>
#include <iomanip>
#include <memory>

namespace data
{

    /// HDF5 C-API has DimensionScaled
    struct Dimension
    {
        std::string title;
        //std::vector<std::string> names;
        //std::vector<T> values;
        std::string unit;
    };

    /**  delay the design until it is actually needed
     * this class should holds metadata and a pointer to the data
    */

    /**  an tentative impl of Table, RowMajor table
     * this class should holds metadata and a pointer to the data
     * RowType can be any C++ data class
    */
    template <typename RowType>
    class Table
    {

    public:
        Table() : {};

        std::shared_ptr<std::vector<RowType>> rows;
        std::vector<std::string> colnames;
        std::vector<std::string> colunits;
    };

} // namespace data