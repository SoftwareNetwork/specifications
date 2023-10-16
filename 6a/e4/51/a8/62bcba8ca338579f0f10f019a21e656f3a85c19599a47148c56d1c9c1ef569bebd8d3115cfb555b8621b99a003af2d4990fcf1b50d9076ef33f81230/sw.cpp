void build(Solution &s)
{
    auto &t = s.addLibrary("beached.daw_json_link", "3.20.1");
    t += Git("https://github.com/beached/daw_json_link", "v{v}");
}
