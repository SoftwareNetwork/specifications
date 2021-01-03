void build(Solution &s)
{
    auto &t = s.addTarget<StaticLibrary>("webmproject.webm", "master");
    t += Git("https://github.com/webmproject/libwebm");

    t +=
        "mkvparser/.*"_rr,
        "mkvmuxer/.*"_rr,
        "common/webmids.h"
        ;
}
