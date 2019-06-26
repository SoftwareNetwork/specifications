void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("SqliteModernCpp.sqlite_modern_cpp", "master");
    t += Git("https://github.com/SqliteModernCpp/sqlite_modern_cpp");
    t.setRootDirectory("hdr");
}
