void build(Solution &s)
{
    auto &t = s.addLibrary("beached.daw_json_link", "3.28.3");
    t += Git("https://github.com/beached/daw_json_link", "v{v}");
}
