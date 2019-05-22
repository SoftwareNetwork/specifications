void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("dmitigr.pgfe", "master");
    t += Git("https://github.com/dmitigr/pgfe");
    t += cpp17;

    t += "DMITIGR_PGFE_DLL_BUILDING"_def;
    t.Public += sw::Shared, "DMITIGR_PGFE_DLL"_def;

    t -= "lib/dmitigr/.*"_rr;

    t += "org.sw.demo.dmitigr.common-master"_dep;
}
