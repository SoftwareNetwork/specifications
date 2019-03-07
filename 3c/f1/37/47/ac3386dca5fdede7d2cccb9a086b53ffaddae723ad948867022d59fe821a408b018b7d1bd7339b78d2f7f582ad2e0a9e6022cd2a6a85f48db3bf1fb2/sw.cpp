#pragma sw require header org.sw.demo.qtproject.qt.base.tools.moc-*

void build(Solution &s)
{
    auto &t = s.addTarget<StaticLibrary>("libxls", "1.5.0");
    t += Git("https://github.com/libxls/libxls", "", "v{v}");

    t.PackageDefinitions = true;
    t -= "src/xls2csv.c";

    t += "size_t=uint64_t"_def;
    t += "ssize_t=long long"_def;

    if (s.Settings.TargetOS.Type == OSType::Windows)
    {
        t += "size_t=uint64_t"_def;
        t += "ssize_t=long long"_def;

        t.writeFileOnce(t.BinaryPrivateDir / "unistd.h");
    }
    t.writeFileOnce(t.BinaryPrivateDir / "config.h");
}
