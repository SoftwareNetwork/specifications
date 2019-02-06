void build(Solution &s)
{
    auto &stb = s.addProject("stb", "master");
    stb += Git("https://github.com/nothings/stb", "", "master");

    auto &all = stb.addTarget<StaticLibraryTarget>("all");
    all +=
        "[^/]*\\.c"_r,
        "[^/]*\\.h"_r;

    auto add_file = [&stb](const String &name) -> decltype(auto)
    {
        auto &t = stb.addTarget<StaticLibraryTarget>(name);
        t += path("stb_" + name + ".h");
        return t;
    };

    add_file("image");
    add_file("image_write");
}
