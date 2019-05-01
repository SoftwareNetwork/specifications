void build(Solution &s)
{
    auto &t = s.addTarget<Library>("zchunk.zchunk", "1.1.1");
    t += Git("https://github.com/zchunk/zchunk");
    //t.configureFile("");
}
