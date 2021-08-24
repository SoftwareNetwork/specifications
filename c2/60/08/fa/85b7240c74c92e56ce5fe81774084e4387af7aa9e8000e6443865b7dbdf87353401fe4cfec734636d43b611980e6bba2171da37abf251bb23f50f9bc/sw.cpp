void build(Solution &s)
{
    auto &json = s.addTarget<LibraryTarget>("nlohmann.json", "3.10.1");
    json += Git("https://github.com/nlohmann/json", "v{v}");

    json += "include/.*"_rr;
    json += "nlohmann_json.natvis";
    json.Public += "nlohmann_json.natvis";
}
