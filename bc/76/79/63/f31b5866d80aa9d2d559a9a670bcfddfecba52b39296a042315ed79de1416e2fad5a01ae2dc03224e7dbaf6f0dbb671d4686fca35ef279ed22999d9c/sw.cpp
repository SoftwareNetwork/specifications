void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("anatoliifrolov.postgres_cxx_client", "master");
    t += Git("https://github.com/anatoliifrolov/postgres-cxx-client");
    t += cpp17;
    t += "org.sw.demo.find.libpq-master"_dep;
}
