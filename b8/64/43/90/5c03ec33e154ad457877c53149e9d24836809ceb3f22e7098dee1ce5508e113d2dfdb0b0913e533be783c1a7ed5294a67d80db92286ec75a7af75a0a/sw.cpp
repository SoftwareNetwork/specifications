void build(Solution &s)
{
    auto &t = s.addTarget<Library>("myproject", "1.1.1");
    t += Git("https://github.com/zchunk/zchunk");
    //t.configureFile("");
}
