void build(Solution &s)
{
    auto &utfcpp = s.addTarget<LibraryTarget>("nemtrif.utfcpp", "4.1.1");
    utfcpp += Git("https://github.com/nemtrif/utfcpp", "v{v}");
}
