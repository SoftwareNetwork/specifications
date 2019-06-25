void build(Solution &s)
{
    auto &t = s.addTarget<Library>("SergiusTheBest.plog", "1.1.4");
    t += Git("https://github.com/SergiusTheBest/plog");
}
