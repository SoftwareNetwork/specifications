void build(Solution &s)
{
    auto &xtl = s.addTarget<LibraryTarget>("quantstack.xtensor", "0.19.4");
    xtl += Git("https://github.com/QuantStack/xtensor", "{v}");
}
