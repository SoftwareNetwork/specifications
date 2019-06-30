void build(Solution &s)
{
    auto &p = s.addTarget<LibraryTarget>("eyalz800.serializer", "master");
    p += Git("https://github.com/eyalz800/serializer");
}
