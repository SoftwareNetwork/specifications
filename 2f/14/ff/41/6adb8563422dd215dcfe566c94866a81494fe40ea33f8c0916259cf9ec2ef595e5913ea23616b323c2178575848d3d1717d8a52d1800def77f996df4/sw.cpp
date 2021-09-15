void build(Solution &s)
{
    auto &rapidjson = s.addTarget<LibraryTarget>("tencent.rapidjson", "master");
    rapidjson += Git("https://github.com/tencent/rapidjson");
}
