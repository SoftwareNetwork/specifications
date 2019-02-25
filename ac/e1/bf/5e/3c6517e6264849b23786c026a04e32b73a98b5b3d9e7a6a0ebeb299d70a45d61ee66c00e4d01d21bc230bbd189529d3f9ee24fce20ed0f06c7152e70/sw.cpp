void build(Solution &s)
{
    auto &xtl = s.addTarget<LibraryTarget>("quantstack.xtl", "0.5.4");
    xtl += Git("https://github.com/QuantStack/xtl", "{v}");
}
