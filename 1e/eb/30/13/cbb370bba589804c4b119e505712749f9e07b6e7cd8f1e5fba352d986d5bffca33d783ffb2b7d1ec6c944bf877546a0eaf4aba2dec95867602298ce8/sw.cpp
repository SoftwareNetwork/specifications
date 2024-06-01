void build(Solution &s)
{
    auto &wrk = s.addExecutable("wg.wrk", "4.2.0");
    wrk += Git("https://github.com/wg/wrk");

    wrk += "src/.*"_rr;
    wrk -= "src/ae_.*"_rr;

    wrk.writeFileOnce("version.c", "const char *VERSION=\"0\";");
    wrk += "version.c";

    wrk += "org.sw.demo.luajit"_dep;
    wrk += "org.sw.demo.openssl.ssl"_dep;
}
