void build(Solution &s)
{
    auto &utfcpp = s.addTarget<LibraryTarget>("nemtrif.utfcpp", "3.2.5");
    utfcpp += Git("https://github.com/nemtrif/utfcpp", "v{v}");
}
