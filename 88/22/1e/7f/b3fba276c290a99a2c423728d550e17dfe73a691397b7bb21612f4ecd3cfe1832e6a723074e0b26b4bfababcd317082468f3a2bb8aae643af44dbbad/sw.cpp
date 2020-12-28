void build(Solution &s)
{
    auto &googletest = s.addProject("google.googletest", "1.10.0");
    googletest += Git("https://github.com/google/googletest", "release-{v}");

    const auto api = "GTEST_API_"s;

    auto add = [&googletest, &api](const String &name) -> decltype(auto)
    {
        const auto dirname = "google" + name;
        const auto tname = "g" + name;

        auto &t = googletest.addLibrary(tname);
        {
            t += cpp11;
            t.ApiName = api;
            t -= FileRegex(dirname, "include/.*", true);
            t -= FileRegex(dirname, "src/.*", true);
            t += path(dirname) / "src" / (tname + "-all.cc");

            t += IncludeDirectory(dirname);
            t.Public += IncludeDirectory(dirname + "/include");

            if (t.getBuildSettings().TargetOS.Type != OSType::Windows)
                t += "pthread"_slib;
        }

        auto &m = t.addStaticLibrary("main");
        {
            m += cpp11;
            m.ApiName = api;
            m += path(dirname) / "src" / (tname + "_main.cc");
            m.Public += t;
        }

        return t;
    };

    auto &t = add("test");
    auto &m = add("mock");
    m.Public += t;

    t.patch("googletest/include/gtest/internal/gtest-port.h", "bool ParseInt32", api + "\nbool  ParseInt32");
}
