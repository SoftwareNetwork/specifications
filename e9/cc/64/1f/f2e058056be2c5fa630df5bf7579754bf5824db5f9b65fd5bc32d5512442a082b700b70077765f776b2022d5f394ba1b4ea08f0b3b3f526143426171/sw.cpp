void build(Solution &s)
{
    auto &t = s.addLibrary("SanderMertens.flecs", "3.2.0");
    t += Git("https://github.com/SanderMertens/flecs", "v{v}");

    t += "FLECS_IMPL"_def;
    t.Public += sw::Static, "FLECS_STATIC"_def;
}
