void build(Solution &s)
{
    auto &charls = s.addTarget<LibraryTarget>("vbaderks.charls", "master");
    charls += Git("https://github.com/team-charls/charls", "", "{v}");

    charls.Public += sw::Static, "CHARLS_STATIC"_def;
    charls += "CHARLS_LIBRARY_BUILD"_def;
}
