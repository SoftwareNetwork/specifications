void build(Solution &s)
{
    auto &t = s.addLibrary("enzo1982.mp4v2", "2.1.2");
    t += Git("https://github.com/enzo1982/mp4v2", "", "main");

    t += "include/.*"_rr;
    t += "libplatform/.*"_rr;
    t -= "libplatform/.*posix.*"_rr;
    t -= "libplatform/.*win32.*"_rr;
    t += "src/.*"_rr;

    if (t.getBuildSettings().TargetOS.is(OSType::Windows)) {
        t += "libplatform/.*win32.*"_rr;
    } else {
        t += "libplatform/.*posix.*"_rr;
    }

    t += sw::Shared, "MP4V2_EXPORTS"_def;
    //t.Interface += sw::Shared, "MP4V2_USE_DLL_IMPORT"_def;
    t.Public += sw::Static, "MP4V2_USE_STATIC_LIB"_def;

    t.Public += "include"_idir;
    t += "."_idir;

    /*#define MP4V2_PROJECT_version_hex     @PROJECT_version_hex@
    #define MP4V2_PROJECT_version_major   @PROJECT_version_major@
    #define MP4V2_PROJECT_version_minor   @PROJECT_version_minor@
    #define MP4V2_PROJECT_version_point   @PROJECT_version_point@*/
    t.configureFile("include/mp4v2/project.h.in", "mp4v2/project.h");
}
