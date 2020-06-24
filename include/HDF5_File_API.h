#pragma once

#include <vector>
#include "ResultTypes.h"

namespace data
{

    /** this will be used by Pipeline impl code, if file format is `h5`
     * */
    class H5File_API // must derived from File_API ?
    {
    public:
        static void read_table()
        {
        }

        /// Table class, std:map<std:string, std::vector<>>
        static void write_table()
        {
        }

        template <class RowType>
        static Table<RowType> read_table()
        {
        }

        /// may also add the TableSpec as input parameter, with extra info like units
        template <class RowType>
        static void write_table(Table<RowType>)
        {
        }
    };

} // namespace data