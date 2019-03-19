void build(Solution &s)
{
    auto &t = s.addTarget<StaticLibrary>("cmuratori.meow_hash", "master");
    t += Git("https://github.com/cmuratori/meow_hash");
    t += "meow_hash.h";
    t += "meow_intrinsics.h";
}
