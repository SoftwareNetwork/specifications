void build(Solution &s)
{
    auto &re2 = s.addTarget<StaticLibraryTarget>("google.re2", "2024.5.1");
    re2 += Git("https://github.com/google/re2", "{M}-{m:02}-{p:02}");

    re2 += cpp17;
    re2.ExportAllSymbols = true;
    re2.ImportFromBazel = true;
    re2.BazelTargetName = "re2";
    re2.Public += "."_idir;
    re2 += "NOMINMAX"_def;
    re2 += "org.sw.demo.google.abseil"_dep;
}
