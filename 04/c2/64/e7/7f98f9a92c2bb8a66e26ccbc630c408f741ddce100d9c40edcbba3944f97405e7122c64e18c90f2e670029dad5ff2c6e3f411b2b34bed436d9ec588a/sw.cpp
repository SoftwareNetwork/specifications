void build(Solution &s)
{
    auto &t = s.addTarget<Library>("AlexeyAB.object_threadsafe", "master");
    t += Git("https://github.com/AlexeyAB/object_threadsafe");
}
