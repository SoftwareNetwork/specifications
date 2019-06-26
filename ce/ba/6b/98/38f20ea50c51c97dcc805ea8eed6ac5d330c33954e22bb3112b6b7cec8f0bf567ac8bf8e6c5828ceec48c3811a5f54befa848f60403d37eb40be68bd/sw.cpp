void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("TheLartians.EasyIterator", "master");
    t += Git("https://github.com/TheLartians/EasyIterator");
}
