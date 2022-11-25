void build(Solution &s) {
    auto &t = s.addLibrary("richgel999.miniz", "3.0.1");
    t += Git("https://github.com/richgel999/miniz");
    t += "MINIZ_EXPORT"_api;
    t.writeFileOnce("miniz_export.h");
}
