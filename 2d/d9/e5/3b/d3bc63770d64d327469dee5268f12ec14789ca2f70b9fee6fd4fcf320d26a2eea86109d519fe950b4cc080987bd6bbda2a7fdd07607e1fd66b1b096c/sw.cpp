void build(Solution &s)
{
    auto &xtl = s.addTarget<LibraryTarget>("quantstack.xframe", "0.1.0");
    xtl += Git("https://github.com/QuantStack/xframe", "{v}");
}
