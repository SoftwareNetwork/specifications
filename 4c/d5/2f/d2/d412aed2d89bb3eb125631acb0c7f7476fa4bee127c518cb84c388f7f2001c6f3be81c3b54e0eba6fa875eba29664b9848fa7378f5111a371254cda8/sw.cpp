void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("kthohr.gcem", "1.8.1");
    t += Git("https://github.com/kthohr/gcem", "v{v}");
}
