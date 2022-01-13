void build(Solution &s)
{
    auto &p = s.addProject("webmproject", "1.0.0.28");
    p += Git("https://github.com/webmproject/libwebm", "libwebm-{v}");

    auto install = [](auto &t)
    {
        if (t.DryRun)
            return;
        auto d = t.BinaryDir / "libwebm";
        fs::create_directories(d);
        for (auto &[p, sf] : t[".*\\.hpp"_rr])
        {
            auto f = p;
            t.check_absolute(f);
            fs::copy_file(f, d / f.filename(), fs::copy_options::update_existing);
        }
    };

    auto &mkvparser = p.addStaticLibrary("mkvparser");
    mkvparser += "mkvparser/.*"_rr;
    mkvparser += "mkvparser.hpp";
    mkvparser += "mkvreader.hpp";
    mkvparser += "common/webmids.h";
    mkvparser += "webmids.hpp";
    install(mkvparser);

    auto &mkvmuxer = p.addStaticLibrary("mkvmuxer");
    mkvmuxer += "mkvmuxer/.*"_rr;
    mkvmuxer += "mkvmuxer.hpp";
    mkvmuxer += "mkvmuxertypes.hpp";
    mkvmuxer += "mkvmuxerutil.hpp";
    mkvmuxer += "mkvwriter.hpp";
    mkvmuxer.Public += mkvparser;
    install(mkvmuxer);

    auto &webm = p.addStaticLibrary("webm");
    //webm.ExportAllSymbols = true;
    webm += "common/file_util.cc";
    webm += "common/file_util.h";
    webm += "common/hdr_util.cc";
    webm += "common/hdr_util.h";
    webm.Public += mkvmuxer;
    install(webm);
}
