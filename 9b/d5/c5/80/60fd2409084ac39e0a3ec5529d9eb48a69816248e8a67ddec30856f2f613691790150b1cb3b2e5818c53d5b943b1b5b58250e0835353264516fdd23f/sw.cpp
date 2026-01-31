void build(Solution &s)
{
    auto &utfcpp = s.addTarget<LibraryTarget>("nemtrif.utfcpp", "4.0.9");
    utfcpp += Git("https://github.com/nemtrif/utfcpp", "v{v}");
}
