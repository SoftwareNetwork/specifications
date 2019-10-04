void build(Solution &s)
{
    auto &t = s.addStaticLibrary("xiph.opusfile", "0.11.0");
    t += RemoteFile("https://archive.mozilla.org/pub/opus/opusfile-{M}.{m}.tar.gz");

    t.Public += "org.sw.demo.xiph.opus"_dep;
    t.Public += "org.sw.demo.xiph.libogg"_dep;

    if (!t.DryRun)
    {
        auto dummy = t.BinaryDir / "private/sw_copy_headers.txt";
        if (!fs::exists(dummy))
        {
            auto dir2 = t.BinaryDir / "opus";
            fs::create_directories(dir2);
            for (auto &f : fs::recursive_directory_iterator(t.SourceDir / "include"))
                fs::copy_file(f, dir2 / f.path().filename(), fs::copy_options::overwrite_existing);
            write_file(dummy, "");
        }
    }
}
