void build(Solution &s)
{
    auto &rapidjson = s.addTarget<LibraryTarget>("miloyip.rapidjson", "1.1.0");
    rapidjson += Git("https://github.com/miloyip/rapidjson", "v{v}");
    rapidjson += "include/.*"_rr;
    rapidjson.Public += "RAPIDJSON_HAS_STDSTRING"_d;
}
