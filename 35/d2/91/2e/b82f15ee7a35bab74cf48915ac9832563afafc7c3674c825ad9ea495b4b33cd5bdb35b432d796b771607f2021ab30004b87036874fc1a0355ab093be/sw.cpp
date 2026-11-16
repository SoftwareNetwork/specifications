void build(Solution &s)
{
    auto &utfcpp = s.addTarget<LibraryTarget>("nemtrif.utfcpp", "5.0.0");
    utfcpp += Git("https://github.com/nemtrif/utfcpp", "v{v}");
}
