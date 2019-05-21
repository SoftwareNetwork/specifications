void build(Solution &s)
{
    auto &t = s.addTarget<StaticLibraryTarget>("r_lyeh_archived.sole", "master");
    t += Git("https://github.com/r-lyeh-archived/sole");
    t += ".*\\.hpp"_rr;
}
