void build(Solution &s)
{
    auto &t = s.addLibrary("SanderMertens.flecs", "2.4.2");
    t += Git("https://github.com/SanderMertens/flecs", "v{v}");

    t += "FLECS_IMPL"_def;
    t.Public += sw::Static, "FLECS_STATIC"_def;
}
