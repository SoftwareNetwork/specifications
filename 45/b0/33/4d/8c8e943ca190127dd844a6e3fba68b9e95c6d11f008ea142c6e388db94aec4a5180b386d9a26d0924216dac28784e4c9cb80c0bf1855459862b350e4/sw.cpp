void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("palacaze.sigslot", "1.2.2");
    t += Git("https://github.com/palacaze/sigslot", "v{v}");
}
