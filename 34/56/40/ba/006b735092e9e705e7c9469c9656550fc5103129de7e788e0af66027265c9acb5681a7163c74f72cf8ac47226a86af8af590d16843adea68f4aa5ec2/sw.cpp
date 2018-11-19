void build(Solution &s)
{
    auto &charls = s.addTarget<LibraryTarget>("vbaderks.charls", "2.0.0");
    charls += Git("https://github.com/team-charls/charls", "{v}");

    charls +=
        "src.*\\.cpp"_rr,
        "src/.*\\.h"_rr;

    charls.Public +=
        "src"_id;

    charls.ApiName = "CHARLS_API";
    charls.replaceInFileOnce("src/charls.h",
        "#include \"publictypes.h\"",
        "#include  \"publictypes.h\"\n#define CHARLS_IMEXPORT(x) CHARLS_API x");
}
