void build(Solution &s)
{
    auto &charls = s.addTarget<LibraryTarget>("vbaderks.charls", "2.1.0");
    charls += Git("https://github.com/team-charls/charls");

    charls += "CHARLS_LIBRARY_BUILD"_def;
}
