void build(Solution &s)
{
    auto &t = s.addTarget<StaticLibraryTarget>("skypjack.meta", "1.4.1");
    t += Git("https://github.com/skypjack/meta", "v{v}");
}
