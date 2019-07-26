void build(Solution &s)
{
    auto &t = s.addTarget<StaticLibraryTarget>("ulfjack.ryu", "master");
    t += Git("https://github.com/ulfjack/ryu");
    t += "ryu/.*"_r;
}
