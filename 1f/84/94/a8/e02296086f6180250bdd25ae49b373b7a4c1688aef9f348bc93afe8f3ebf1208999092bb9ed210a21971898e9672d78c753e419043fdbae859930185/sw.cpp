void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("fnc12.sqlite_orm", "1.9.0");
    t += Git("https://github.com/fnc12/sqlite_orm", "v{M}.{m}{po}");
}
