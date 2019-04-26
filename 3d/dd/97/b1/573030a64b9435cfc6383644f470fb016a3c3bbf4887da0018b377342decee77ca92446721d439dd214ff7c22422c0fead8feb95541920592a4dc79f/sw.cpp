void build(Solution &s)
{
    auto &t = s.addTarget<Library>("redxdev.ECS", "master");
    t += Git("https://github.com/redxdev/ECS");
    t += "ECS.h";
}
