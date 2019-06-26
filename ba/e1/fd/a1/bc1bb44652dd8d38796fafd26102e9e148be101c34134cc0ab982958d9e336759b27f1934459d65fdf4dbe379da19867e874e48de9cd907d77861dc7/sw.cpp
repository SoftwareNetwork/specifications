void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("beached.daw_json_link", "master");
    t += Git("https://github.com/beached/daw_json_link");
}
