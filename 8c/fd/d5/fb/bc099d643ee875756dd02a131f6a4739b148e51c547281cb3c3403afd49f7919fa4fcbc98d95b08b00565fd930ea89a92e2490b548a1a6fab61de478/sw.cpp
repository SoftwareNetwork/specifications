void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("uNetworking.uWebSockets", "19.3.0");
    t += Git("https://github.com/uNetworking/uWebSockets", "v{v}");

    //t += cpp20; // we are header only
    t.Public += "org.sw.demo.uNetworking.uSockets"_dep;
    t.Public += "org.sw.demo.madler.zlib"_dep;
}
