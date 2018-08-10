void build(Solution &s)
{
    auto &json = s.addTarget<LibraryTarget>("nlohmann.json", "3.1.2");
    json += Git("https://github.com/nlohmann/json", "v{v}");
}
