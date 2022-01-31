void build(Solution &s)
{
    auto &p = s.addProject("howardhinnant.date", "3.0.1");
    p += Git("https://github.com/HowardHinnant/date", "v{v}");

    auto &date = p.addTarget<StaticLibraryTarget>("date");
    date += "include/date/date.h";

    auto &date_full = p.addLibrary("date_full");
    date_full += cpp17;
    //date_full += "org.sw.demo.badger.curl.libcurl"_dep;
    date_full -= "src/ios.mm";
    date_full.Public += "HAS_REMOTE_API=0"_def;
    if (date_full.getBuildSettings().TargetOS.Type != OSType::Windows) {
        date_full.Public += "USE_OS_TZDB=1"_def;
    } else {
        date_full += "Ole32.lib"_slib;
        date_full += "Shell32.lib"_slib;
    }
    date_full += sw::Shared, "DATE_BUILD_DLL"_def;
    date_full.Interface += sw::Shared, "DATE_USE_DLL"_def;
}
