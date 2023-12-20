void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("uNetworking.uWebSockets", "20.51.0");
    t += Git("https://github.com/uNetworking/uWebSockets", "v{v}");

    t += "src/.*"_rr;
    String f;
    for (auto &&[p,_] : t["src/.*"_rr]) {
        f += "#include <" + normalize_path(p).string() + ">\n";
    }
    t.writeFileOnce("uWS/uWS.h", f);

    //t += cpp20; // we are header only
    t.Public += "org.sw.demo.uNetworking.uSockets"_dep;
    t.Public += "org.sw.demo.madler.zlib"_dep;
}
