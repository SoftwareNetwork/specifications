void build(Solution &s)
{
    auto &re2 = s.addTarget<StaticLibraryTarget>("google.re2", "2020.8.1");
    re2 += Git("https://github.com/google/re2", "{M}-{m:02}-{p:02}");

    re2 += cpp11;
    re2.ImportFromBazel = true;
    re2.BazelTargetName = "re2";
    re2.Public += "."_idir;
}
