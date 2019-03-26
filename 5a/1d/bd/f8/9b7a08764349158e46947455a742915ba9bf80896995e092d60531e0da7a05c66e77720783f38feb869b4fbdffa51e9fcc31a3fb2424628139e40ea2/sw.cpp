void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("mpark.variant", "1.4.0");
    t += Git("https://github.com/mpark/variant", "v{v}");
}
