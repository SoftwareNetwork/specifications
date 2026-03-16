void build(Solution &s)
{
    auto &t = s.addLibrary("skeeto.pdjson", "master");
    t += Git("https://github.com/skeeto/pdjson");
    t += "PDJSON_SYMEXPORT"_api;
    t += "pdjson\\.[hc]"_r;
}
