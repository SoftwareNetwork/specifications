void build(Solution &s)
{
    auto &t = s.addTarget<StaticLibraryTarget>("AgnerF.vectorclass", "master");
    t += Git("https://github.com/vectorclass/version2");
    t += ".*\\h"_r;
    t += "instrset_detect.cpp";
}
