#ifdef SW_PRAGMA_HEADER
#pragma sw header on

void re2c(NativeExecutedTarget &t, const path &f, const String &ext)
{
    auto re2c = THIS_PREFIX "." "re2c.re2c" "-" THIS_VERSION_DEPENDENCY;
    {
        auto d = t + re2c;
        d->Dummy = true;
    }

    auto in = t.SourceDir / f;
    auto out = t.BinaryPrivateDir / (f.u8string() + "." + ext);
    fs::create_directories(out.parent_path());

    SW_MAKE_COMMAND_AND_ADD(c, t);
    c->setProgram(re2c);
    c->args.push_back("-o");
    c->args.push_back(normalize_path(out));
    c->args.push_back(normalize_path(in));
    c->addInput(in);
    c->addOutput(out);
    t += f;
    t += out;
}

#pragma sw header off
#endif

void build(Solution &s)
{
    auto &p = s.addProject("re2c", "1.1.1");
    p += Git("https://github.com/skvadrik/re2c", "{M}.{m}{po}");

    auto &libre2c = p.addStaticLibrary("libre2c");
    libre2c.SourceDir /= "libre2c";
    libre2c += "libre2c/.*"_rr;
    if (s.Settings.TargetOS.Arch == ArchType::x86)
        libre2c += "ssize_t=int"_def;
    else
        libre2c += "ssize_t=long long"_def;
    if (s.Settings.TargetOS.Type == OSType::Windows)
    {
        libre2c -= "libre2c/readfd.c";
        libre2c -= "libre2c/readrand.c";
    }

    auto &re2c = p.addExecutable("re2c");
    re2c.PackageDefinitions = true;
    re2c.setChecks("re2c");
    re2c.SourceDir /= "re2c";
    re2c += "bootstrap/src/.*"_rr;
    re2c += "src/.*"_rr;
    re2c -= "src/test/.*"_rr;
    re2c += "."_idir, "bootstrap"_idir;
    re2c.writeFileOnce(re2c.BinaryPrivateDir / "config.h");
}

void check(Checker &c)
{
    auto &s = c.addSet("re2c");
    s.checkIncludeExists("stdint.h");
}
