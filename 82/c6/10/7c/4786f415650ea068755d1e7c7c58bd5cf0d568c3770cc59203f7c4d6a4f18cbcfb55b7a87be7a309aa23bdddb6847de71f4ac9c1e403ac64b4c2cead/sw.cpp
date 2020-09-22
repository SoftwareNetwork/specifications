void build(Solution &s)
{
    auto &t = s.addLibrary("skypjack.entt", "3.5.2");
    t += Git("https://github.com/skypjack/entt", "v{v}");
}
