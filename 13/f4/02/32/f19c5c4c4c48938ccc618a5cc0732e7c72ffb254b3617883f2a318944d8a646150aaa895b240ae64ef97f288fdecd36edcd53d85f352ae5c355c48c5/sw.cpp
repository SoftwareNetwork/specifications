void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("TheLartians.Event", "master");
    t += Git("https://github.com/TheLartians/Event");
}
