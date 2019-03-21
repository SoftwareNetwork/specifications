void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("tcbrindle.NanoRange", "master");
    t += Git("https://github.com/tcbrindle/NanoRange");
}
