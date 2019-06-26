void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("fnc12.sqlite_orm", "master");
    t += Git("https://github.com/fnc12/sqlite_orm");
}
