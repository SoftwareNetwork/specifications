void build(Solution &sln)
{
    auto &s = sln.addDirectory("demo");
    auto &ucdn = s.addTarget<StaticLibraryTarget>("grigorig.ucdn", "master");
    ucdn.Source = Git("https://github.com/grigorig/ucdn", "", "master");

    ucdn +=
        "ucdn.c",
        "ucdn.h",
        "ucdn_db.h";
}