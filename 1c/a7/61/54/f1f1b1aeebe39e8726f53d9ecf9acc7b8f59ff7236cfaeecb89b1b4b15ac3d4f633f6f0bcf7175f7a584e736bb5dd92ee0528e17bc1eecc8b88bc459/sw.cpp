void build(Solution &s)
{
    auto &t = s.addLibrary("beached.header_libraries", "master");
    t += Git("https://github.com/beached/header_libraries");
}
