#pragma sw header on

void re2c(const DependencyPtr &d, NativeExecutedTarget &t, const path &f, const String &ext = ".c")
{
    auto c = t.addCommand();
    c << cmd::prog(d)
        << "-o"
        << cmd::out(path(f) += ext, cmd::NormalizePath)
        << cmd::in(f, cmd::NormalizePath)
        ;
}

#pragma sw header off

void build(Solution &s)
{
    auto &p = s.addProject("re2c", "2.2.0");
    p += Git("https://github.com/skvadrik/re2c", "{M}.{m}{po}");

    auto &libre2c = p.addStaticLibrary("libre2c");
    libre2c.SourceDir /= "libre2c_old";
    libre2c += "libre2c/.*"_rr;
    if (libre2c.getBuildSettings().TargetOS.Arch == ArchType::x86)
        libre2c += "ssize_t=int"_def;
    else
        libre2c += "ssize_t=long long"_def;
    if (libre2c.getBuildSettings().TargetOS.Type == OSType::Windows)
    {
        libre2c -= "libre2c/readfd.c";
        libre2c -= "libre2c/readrand.c";
    }

    auto &re2c = p.addExecutable("re2c");
    re2c.PackageDefinitions = true;
    re2c.setChecks("re2c", true);
    re2c += "bootstrap/src/.*"_rr;
    re2c += "src/.*"_rr;
    re2c -= "src/test/.*"_rr;
    re2c += "."_idir, "bootstrap"_idir;
    re2c += "RE2C_STDLIB_DIR=\"\""_def;
    re2c.writeFileOnce(re2c.BinaryPrivateDir / "config.h");
}

void check(Checker &c)
{
    auto &s = c.addSet("re2c");
    s.checkIncludeExists("fcntl.h");
    s.checkIncludeExists("io.h");
    s.checkIncludeExists("stdint.h");
    s.checkIncludeExists("sys/stat.h");
    s.checkIncludeExists("sys/types.h");
    s.checkIncludeExists("unistd.h");
}
