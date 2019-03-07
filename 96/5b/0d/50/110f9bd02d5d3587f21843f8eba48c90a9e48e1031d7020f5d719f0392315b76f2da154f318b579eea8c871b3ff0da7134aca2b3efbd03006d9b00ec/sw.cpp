void build(Solution &s)
{
    auto &utfcpp = s.addTarget<LibraryTarget>("ledger.utfcpp", "master");
    utfcpp += Git("https://github.com/ledger/utfcpp", "", "{v}");
}
