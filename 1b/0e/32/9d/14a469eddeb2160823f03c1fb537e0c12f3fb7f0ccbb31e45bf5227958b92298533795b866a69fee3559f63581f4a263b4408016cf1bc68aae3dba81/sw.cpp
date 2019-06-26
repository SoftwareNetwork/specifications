void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("TheLartians.Visitor", "master");
    t += Git("https://github.com/TheLartians/Visitor");
}
