void build(Solution &s)
{
    auto &json = s.addTarget<LibraryTarget>("nlohmann.json", "3.5.0");
    json += Git("https://github.com/nlohmann/json", "v{v}");
}
