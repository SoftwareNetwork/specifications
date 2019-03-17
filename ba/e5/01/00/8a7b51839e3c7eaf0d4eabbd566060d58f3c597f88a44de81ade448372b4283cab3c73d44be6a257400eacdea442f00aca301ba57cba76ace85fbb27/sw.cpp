void build(Solution &s)
{
    auto &t = s.addTarget<Library>("freeglut", "3.0.0");
    t += RemoteFile("https://datapacket.dl.sourceforge.net/project/freeglut/freeglut/{v}/freeglut-{v}.tar.gz");

    t.setChecks("freeglut", true);

    t += "include/.*"_rr;
    t -= "src/.*"_rr;

    t += "src/.*"_r;
    //t += "src/egl/.*"_r;
    t += "src/util/.*"_r;
    t -= "src/gles_stubs.c";

    t.Public += sw::Static, "FREEGLUT_STATIC"_def;
    t += "FREEGLUT_EXPORTS"_def;

    if (s.Settings.TargetOS.Type == OSType::Windows)
        t += "src/mswin/.*"_r;
    else if (s.Settings.TargetOS.Type == OSType::Macos)
        t += "src/osx/.*"_r;
    else
        t += "src/x11/.*"_r;

    t += "NEED_XPARSEGEOMETRY_IMPL"_def;
}

void check(Checker &c)
{
    auto &s = c.addSet("freeglut");
    s.checkFunctionExists("alloca");
    s.checkFunctionExists("doprnt");
    s.checkFunctionExists("gettimeofday");
    s.checkFunctionExists("vprintf");
    s.checkIncludeExists("fcntl.h");
    s.checkIncludeExists("errno.h");
    s.checkIncludeExists("inttypes.h");
    s.checkIncludeExists("stdbool.h");
    s.checkIncludeExists("stdint.h");
    s.checkIncludeExists("sys/event.h");
    s.checkIncludeExists("sys/filio.h");
    s.checkIncludeExists("sys/inotify.h");
    s.checkIncludeExists("sys/ioctl.h");
    s.checkIncludeExists("sys/mntctl.h");
    s.checkIncludeExists("sys/mnttab.h");
    s.checkIncludeExists("sys/mount.h");
    s.checkIncludeExists("sys/param.h");
    s.checkIncludeExists("sys/resource.h");
    s.checkIncludeExists("sys/select.h");
    s.checkIncludeExists("sys/statfs.h");
    s.checkIncludeExists("sys/statvfs.h");
    s.checkIncludeExists("sys/sysctl.h");
    s.checkIncludeExists("sys/time.h");
    s.checkIncludeExists("sys/vfstab.h");
    s.checkIncludeExists("sys/vfs.h");
    s.checkIncludeExists("sys/vmount.h");
    s.checkIncludeExists("sys/xattr.h");
    s.checkIncludeExists("usbhid.h");
    s.checkIncludeExists("unistd.h");
    s.checkTypeSize("char");
    s.checkTypeSize("int");
    s.checkTypeSize("intmax_t");
    s.checkTypeSize("long");
    s.checkTypeSize("long long");
    s.checkTypeSize("unsigned long long").Definitions.insert("HAVE_ULONG_LONG");
    s.checkTypeSize("ptrdiff_t");
    s.checkTypeSize("short");
    s.checkTypeSize("size_t");
    s.checkTypeSize("ssize_t");
    s.checkTypeSize("unsigned");
    s.checkTypeSize("void *");
    s.checkTypeSize("__int64");
    s.checkTypeSize("unsigned __int64").Definitions.insert("HAVE_U__INT64");
}
