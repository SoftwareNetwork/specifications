void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("boyerjohn.rapidstring", "master");
    t += Git("https://github.com/boyerjohn/rapidstring");
}
