void build(Solution &s)
{
    auto &t = s.addLibrary("skypjack.entt", "3.15.0");
    t += Git("https://github.com/skypjack/entt", "v{v}");
}
