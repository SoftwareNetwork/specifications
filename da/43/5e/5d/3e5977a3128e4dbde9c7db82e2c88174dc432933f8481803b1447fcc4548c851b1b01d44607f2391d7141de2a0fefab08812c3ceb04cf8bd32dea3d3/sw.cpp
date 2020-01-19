void build(Solution &s)
{
    const String uv_ver = "1.30";
    auto uv = "org.sw.demo.libuv"_dep;
    uv->package.range = uv_ver;

    auto &p = s.addLibrary("skypjack.uvw", "1.18.0");
    p += Git("https://github.com/skypjack/uvw", "v{v}_libuv-v" + uv_ver);
    p.Public += uv;
}