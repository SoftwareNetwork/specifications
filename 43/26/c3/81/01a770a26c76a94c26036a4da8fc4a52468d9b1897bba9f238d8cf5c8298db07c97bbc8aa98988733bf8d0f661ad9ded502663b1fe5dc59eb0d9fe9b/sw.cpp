void build(Solution &s)
{
    auto &libogg = s.addTarget<LibraryTarget>("xiph.libogg", "1.3.4");
    libogg += RemoteFile("https://downloads.xiph.org/releases/ogg/libogg-{v}.tar.gz");

    libogg +=
        "include/.*\\.h"_rr,
        "include/.*\\.in"_rr,
        "src/.*\\.[hc]"_rr,
        "win32/ogg.def";

    libogg.Private +=
        "src"_id;
    libogg.Public +=
        "include"_id;

    libogg.writeFileOnce("ogg/config_types.h", R"(
        #pragma once

        #  include <stdint.h>

        typedef int16_t ogg_int16_t;
        typedef uint16_t ogg_uint16_t;
        typedef int32_t ogg_int32_t;
        typedef uint32_t ogg_uint32_t;
        typedef int64_t ogg_int64_t;
        )"
    );

    libogg.patch("win32/ogg.def", "LIBRARY ogg", "LIBRARY");
}
