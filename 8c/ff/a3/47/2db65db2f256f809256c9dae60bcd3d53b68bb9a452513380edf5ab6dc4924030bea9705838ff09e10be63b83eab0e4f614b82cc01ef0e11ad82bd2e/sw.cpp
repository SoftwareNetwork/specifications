void build(Solution &s)
{
    auto &t = s.addTarget<StaticLibrary>("rizkg.BBHash", "master");
    t += Git("https://github.com/rizkg/BBHash");
    t += "BooPHF.h";
}
