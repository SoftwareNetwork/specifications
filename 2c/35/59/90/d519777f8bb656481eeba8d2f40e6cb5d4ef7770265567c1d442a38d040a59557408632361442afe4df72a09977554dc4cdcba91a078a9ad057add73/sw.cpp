void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("apache.apr.iconv", "1.2.2");
    t += Git("https://github.com/apache/apr-iconv", "{v}");

    t.Public += "WIN32"_d;
    t += sw::Shared, "API_DECLARE_EXPORT"_d;
    t.Public += sw::Static, "API_DECLARE_STATIC"_d;

    t.Public += "org.sw.demo.apache.apr"_dep;
}
