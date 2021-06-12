void build(Solution &s)
{
    auto &t = s.addLibrary("beached.daw_json_link", "2.9.5");
    t += Git("https://github.com/beached/daw_json_link", "v{v}");
}
