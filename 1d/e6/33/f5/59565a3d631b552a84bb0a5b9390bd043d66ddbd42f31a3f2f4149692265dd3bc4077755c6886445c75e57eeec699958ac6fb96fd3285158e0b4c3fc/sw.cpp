void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("ericniebler.meta", "master");
    t += Git("https://github.com/ericniebler/meta");
}
