void build(Solution &s)
{
    auto &t = s.addTarget<StaticLibraryTarget>("brunocodutra.metal", "2.0.1");
    t += Git("https://github.com/brunocodutra/metal", "v{v}");
}
