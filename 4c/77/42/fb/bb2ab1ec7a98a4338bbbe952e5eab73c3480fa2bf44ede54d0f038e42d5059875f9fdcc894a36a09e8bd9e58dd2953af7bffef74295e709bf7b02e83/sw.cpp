void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("Stiffstream.sobjectizer", "master");
    t += Git("https://github.com/Stiffstream/sobjectizer");
    t += "dev/so_5/.*\\.[hc]pp"_rr;
}
