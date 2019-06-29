void build(Solution &s)
{
    auto &p = s.addTarget<LibraryTarget>("billyquith.ponder", "master");
    p += Git("https://github.com/billyquith/ponder");
}
