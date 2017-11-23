void build(Solution &sln)
{
    auto &s = sln.addDirectory("demo");
    auto &charls = s.addTarget<LibraryTarget>("vbaderks.charls", "1.1.0");
    charls.Source = Git("https://github.com/team-charls/charls", "{v}");
    charls.ApiName = "SW_CHARLS_LIBRARY_API";

    charls +=
        "src.*\\.cpp"_rr,
        "src/.*\\.h"_rr;

    charls.Public +=
        "src"_id;

    charls.replaceInFileOnce("src/interface.h", "#include \"publictypes.h\"",
        "#include \"publictypes.h\"\n#define CHARLS_IMEXPORT(x) SW_CHARLS_LIBRARY_API x");
}
