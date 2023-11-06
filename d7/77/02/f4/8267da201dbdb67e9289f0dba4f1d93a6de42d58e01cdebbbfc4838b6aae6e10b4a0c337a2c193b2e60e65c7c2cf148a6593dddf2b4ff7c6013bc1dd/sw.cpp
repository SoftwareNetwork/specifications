void build(Solution &s)
{
    auto &utfcpp = s.addTarget<LibraryTarget>("nemtrif.utfcpp", "4.0.2");
    utfcpp += Git("https://github.com/nemtrif/utfcpp", "v{v}");
}
