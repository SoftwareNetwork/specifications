void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("target.libdart", "master");
    t += Git("https://github.com/target/libdart");
}
