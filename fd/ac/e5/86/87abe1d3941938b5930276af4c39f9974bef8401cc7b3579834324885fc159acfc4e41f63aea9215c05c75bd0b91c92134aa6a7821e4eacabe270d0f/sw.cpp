void build(Solution &s)
{
    auto &t = s.addStaticLibrary("x42.liboauth", "1.0.3");
    t += Git("https://github.com/x42/liboauth", "v{v}");

    t += "src/.*"_rr;
    t -= "src/sha.*"_rr;

    t += "__LITTLE_ENDIAN__"_def;
    t += "org.sw.demo.openssl.ssl"_dep;
}
