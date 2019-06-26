void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("p_ranav.csv", "master");
    t += Git("https://github.com/p-ranav/csv");
}
