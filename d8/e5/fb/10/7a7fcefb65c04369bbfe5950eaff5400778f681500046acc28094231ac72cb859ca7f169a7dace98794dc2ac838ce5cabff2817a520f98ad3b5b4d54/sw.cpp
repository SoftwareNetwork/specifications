void build(Solution &s)
{
    auto &t = s.addLibrary("JuliaStrings.utf8proc", "2.11.3");
    t += Git("https://github.com/JuliaStrings/utf8proc");
    t += "utf8proc.*"_r;
    t -= "utf8proc_data.*"_r;
    t.Public += sw::Static, "UTF8PROC_STATIC"_def;
    t += sw::Shared, "UTF8PROC_EXPORTS"_def;
}
