struct ValaBinary : ExecutableTarget
{
    void setupCommand(builder::Command &c) const
    {
        c.push_back("--vapidir");
        c.push_back(SourceDir / "vapi");
        ExecutableTarget::setupCommand(c);
    }
};

void build(Solution &s)
{
    // release version does not work on win for some reason yet
    // (gobject ctors)
    // debug works

    auto &p = s.addProject("gnome.vala", "0.48.1");
    p += RemoteFile("https://download.gnome.org/sources/vala/{M}.{m}/vala-{v}.tar.xz");

    auto &gee = p.addStaticLibrary("gee");
    {
        gee += "gee/.*\\.[hc]"_r;
        gee.Public += "gee"_idir;
        gee.Public += "org.sw.demo.gnome.glib.gobject"_dep;

        gee.writeFileOnce("version.h", "#define BUILD_VERSION \"" + gee.getPackage().getVersion().toString() + "\"");
    }

    auto &lib = p.addStaticLibrary("lib");
    {
        lib.PackageDefinitions = true;
        lib += "vala/.*\\.[hc]"_r;
        lib.Public += "vala"_idir;
        lib.Public += gee;
        lib.Public += "org.sw.demo.gnome.glib.gmodule"_dep;

        lib.configureFile("config.h.in", "config.h", ConfigureFlags::EnableUndefReplacements);
    }

    auto &ccode = p.addStaticLibrary("ccode");
    {
        ccode += "ccode/.*\\.[hc]"_r;
        ccode.Public += "ccode"_idir;
        ccode.Public += lib;

        if (ccode.getBuildSettings().TargetOS.Type == OSType::Windows)
            ccode.writeFileOnce("utime.h", "#include <sys/utime.h>");
    }

    auto &codegen = p.addStaticLibrary("codegen");
    {
        codegen += "codegen/.*\\.[hc]"_r;
        codegen.Public += "codegen"_idir;
        codegen.Public += lib, ccode;
    }

    auto &cl = p.add<ValaBinary>("compiler");
    {
        cl.PackageDefinitions = true;
        cl += "compiler/.*\\.c"_r;
        cl -= "vapi/.*"_rr;
        cl += codegen;
        if (cl.getBuildSettings().TargetOS.Type == OSType::Windows)
            cl.writeFileOnce("unistd.h");
    }
}
