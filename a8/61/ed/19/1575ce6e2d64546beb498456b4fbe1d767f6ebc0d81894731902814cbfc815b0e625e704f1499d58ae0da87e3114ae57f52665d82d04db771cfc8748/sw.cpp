void build(Solution &s) {
    auto &t = s.addStaticLibrary("fli2gif", "master");
    t += Git("https://github.com/bovine/fli2gif");

    t += "gif.*"_r;
    t += "fliplay.*"_r;
    t.patch("gif.cpp", "printf", ";//print f");

    auto &exe = t.addExecutable("exe");
    exe += "fli2gif.cpp";
    exe += t;
}
