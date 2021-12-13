void build(Solution &s)
{
    auto &t = s.addStaticLibrary("urnathan.libcody", "master");
    t += Git("https://github.com/urnathan/libcody");
    t.PackageDefinitions = true;
    t += ".*\\.hh"_r;
    t += ".*\\.cc"_r;
    t += "SRCDIR=\"\""_def;
    t += "BUGURL=\"\""_def;  
    t += "REVISION=\"\""_def;        
}
