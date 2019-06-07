void build(Solution &s)
{
    auto &p = s.addLibrary("mlivesu.cinolib", "master");
    p += Git("https://github.com/mlivesu/cinolib");
}
