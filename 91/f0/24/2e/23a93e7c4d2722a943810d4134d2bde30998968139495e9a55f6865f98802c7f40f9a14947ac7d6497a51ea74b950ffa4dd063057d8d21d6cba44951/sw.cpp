void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("rapidxml", "1.13.0");
    t += RemoteFile("https://sourceforge.net/projects/rapidxml/files/rapidxml/rapidxml%20{M}.{m}/rapidxml-{M}.{m}.zip");

    t += ".*\\.hpp"_rr;

    if (!t.DryRun)
    {
        auto dummy = t.BinaryDir / "private/sw_copy_headers.txt";
        if (!fs::exists(dummy))
        {
            auto dir = t.BinaryDir / "rapidxml";
            fs::create_directories(dir);
            for (auto &f : fs::directory_iterator(t.SourceDir))
                fs::copy_file(f, dir / f.path().filename(), fs::copy_options::overwrite_existing);
            write_file(dummy, "");
        }
    }
}
