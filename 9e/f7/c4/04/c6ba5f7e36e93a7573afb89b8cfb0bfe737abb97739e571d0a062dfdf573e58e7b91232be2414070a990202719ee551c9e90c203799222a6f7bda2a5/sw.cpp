void build(Solution &s)
{
    auto &t = s.addTarget<StaticLibraryTarget>("blake2.blake2", "2019.07.24");
    t += Git("https://github.com/BLAKE2/BLAKE2", "{M}{m:02}{p:02}");
    t += "ref/blake.*"_r;
    //t += "sse/blake.*"_r;
}
