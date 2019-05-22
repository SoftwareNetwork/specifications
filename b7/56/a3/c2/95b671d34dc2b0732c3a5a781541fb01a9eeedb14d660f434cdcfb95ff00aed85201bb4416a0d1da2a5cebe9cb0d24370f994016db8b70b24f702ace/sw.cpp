void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("dmitigr.fcgi", "master");
    t += Git("https://github.com/dmitigr/fcgi");
    t += cpp17;

    t += "DMITIGR_FCGI_DLL_BUILDING"_def;
    t.Public += sw::Shared, "DMITIGR_FCGI_DLL"_def;

    t -= "lib/dmitigr/.*"_rr;
    t += "lib/dmitigr/fcgi.cpp"_rr;

    t += "org.sw.demo.dmitigr.common-master"_dep;
}
