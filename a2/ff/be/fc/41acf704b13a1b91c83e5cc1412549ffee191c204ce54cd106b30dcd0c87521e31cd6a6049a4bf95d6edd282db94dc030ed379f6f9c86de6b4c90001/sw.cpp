void build(Solution &s)
{
    auto &t = s.addTarget<StaticLibrary>("libxls", "1.5.1");
    t += Git("https://github.com/libxls/libxls", "v{v}");

    t.PackageDefinitions = true;
    t -= "src/xls2csv.c";

    if (s.getBuildSettings().TargetOS.Type == OSType::Windows)
    {
        t += "size_t=uint64_t"_def;
        t += "ssize_t=long long"_def;

        t.writeFileOnce(t.BinaryPrivateDir / "unistd.h");
    }
    t.writeFileOnce(t.BinaryPrivateDir / "config.h");
}
