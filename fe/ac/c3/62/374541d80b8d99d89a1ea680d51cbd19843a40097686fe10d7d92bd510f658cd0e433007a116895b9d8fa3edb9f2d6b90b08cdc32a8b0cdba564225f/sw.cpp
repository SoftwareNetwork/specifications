void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("wqking.eventpp", "master");
    t += Git("https://github.com/wqking/eventpp");
}
