void build(Solution &s)
{
    auto &ucdn = s.addTarget<StaticLibraryTarget>("grigorig.ucdn", "master");
    ucdn += Git("https://github.com/grigorig/ucdn", "", "{v}");

    ucdn +=
        "ucdn.c",
        "ucdn.h",
        "ucdn_db.h";

}
