void build(Solution &s)
{
    auto &t = s.addTarget<StaticLibrary>("webmproject.webm", "1.0.0.27");
    t += Git("https://github.com/webmproject/libwebm", "libwebm-{v}");

    t +=
        ".*\\.hp?p?"_rr,
        ".*\\.cc"_rr,
        ".*\\.cpp"_rr
        ;
    if (fs::exists(t.SourceDir / "sw.cpp"))
        t -= "sw.cpp";

    // install
    if (!t.DryRun)
    {
        auto d = t.BinaryDir / "libwebm";
        fs::create_directories(d);
        for (auto &[p, sf] : t[".*\\.hp?p?"_rr])
        {
            auto f = p;
            t.check_absolute(f);
            fs::copy_file(f, d / f.filename(), fs::copy_options::update_existing);
        }
    }
}
