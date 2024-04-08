void build(Solution &s)
{
    auto &stb = s.addProject("stb", "master");
    stb += Git("https://github.com/nothings/stb", "", "master");

    auto &all = stb.addTarget<StaticLibraryTarget>("all");
    all +=
        "[^/]*\\.c"_r,
        "[^/]*\\.h"_r;
    if (!all.DryRun)
    {
        auto dummy = all.BinaryDir / "private/sw_copy_headers.txt";
        if (!fs::exists(dummy))
        {
            auto dir1 = all.BinaryDir / "stb";
            fs::create_directories(dir1);
            for (auto &f : fs::directory_iterator(all.SourceDir))
            {
                auto fn = f.path().filename().string();
                if (fn.starts_with("stb_") && fn.ends_with(".h"))
                    fs::copy_file(f, dir1 / fn, fs::copy_options::overwrite_existing);
            }
            write_file(dummy, "");
        }
    }

    auto add_file = [&stb](const String &name) -> decltype(auto)
    {
        auto &t = stb.addTarget<StaticLibraryTarget>(name);
        t += path("stb_" + name + ".h");
        return t;
    };

    add_file("image");
    add_file("image_write");
}
