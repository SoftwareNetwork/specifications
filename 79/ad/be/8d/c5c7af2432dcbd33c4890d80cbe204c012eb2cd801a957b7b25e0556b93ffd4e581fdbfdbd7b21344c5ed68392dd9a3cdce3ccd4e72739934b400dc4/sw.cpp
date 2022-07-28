void build(Solution &s)
{
    auto &googletest = s.addProject("google.googletest", "1.12.1");
    googletest += Git("https://github.com/google/googletest", "release-{v}");

    const auto api = "GTEST_API_"s;
    const auto api2 = "GTEST_MAIN_API_"s;

    auto add = [&googletest, &api, &api2](const String &name) -> decltype(auto)
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

        auto &m = t.addLibrary("main");
        {
            m += cpp11;
            m.ApiName = api2;
            auto f = path(dirname) / "src" / (tname + "_main.cc");
            m += f;
            m.Public += t;
            m.patch(f, api, api2);
        }

        return t;
    };

    auto &t = add("test");
    auto &m = add("mock");
    m.Public += t;

    t.patch("googletest/include/gtest/internal/gtest-port.h", "bool ParseInt32", api + "\nbool  ParseInt32");
    // see https://github.com/google/googletest/issues/3196
    /*t.patch("googletest/src/gtest-death-test.cc",
        "if (fields.size() != 6",
        R"(
for (auto i : {3,4,5}) {
    for (auto &c : fields[i]) if (c < 0) c = ' ';
    fields[i].erase(std::remove_if(fields[i].begin(), fields[i].end(), isspace), fields[i].end());
}

if (fields.size() != 6
)");*/

}
