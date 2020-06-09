void build(Solution &s)
{
    auto &p = s.addProject("howardhinnant.date", "3.0.0");
    p += Git("https://github.com/HowardHinnant/date", "v{v}");

    auto &date = p.addTarget<StaticLibraryTarget>("date");
    date += "include/date/date.h";

    auto &date_full = p.addTarget<StaticLibraryTarget>("date_full");
    date_full += cpp11;
    date_full += "org.sw.demo.badger.curl.libcurl"_dep;
    date_full -= "src/ios.mm";
}
