void build(Solution &s)
{
    auto &json = s.addTarget<LibraryTarget>("nlohmann.json", "3.6.1");
    json += Git("https://github.com/nlohmann/json", "v{v}");

    json += "include/.*"_rr;
    json.Public += "nlohmann_json.natvis";
}
