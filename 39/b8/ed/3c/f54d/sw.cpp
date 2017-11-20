void build(Solution &sln)
{
    auto &s = sln.addDirectory("demo");
    auto &date = s.addTarget<StaticLibraryTarget>("howardhinnant.date", "2.2.0");
    date.Source = Git("https://github.com/HowardHinnant/date", "v{M}.{m}");
    date += "date.h";
    if (s.Settings.TargetOS.Type == OSType::Windows)
        date.Public += "Ole32.lib"_l, "Shell32.lib"_l;
}