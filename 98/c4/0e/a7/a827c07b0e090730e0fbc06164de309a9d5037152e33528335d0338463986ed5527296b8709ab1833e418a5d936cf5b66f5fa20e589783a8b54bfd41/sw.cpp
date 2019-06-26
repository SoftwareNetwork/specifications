void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("NTNU_IHB.FMI4cpp", "master");
    t += Git("https://github.com/NTNU-IHB/FMI4cpp");
}
