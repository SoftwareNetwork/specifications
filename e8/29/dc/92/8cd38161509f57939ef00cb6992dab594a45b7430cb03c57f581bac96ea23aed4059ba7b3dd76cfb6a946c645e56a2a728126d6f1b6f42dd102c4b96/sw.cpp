#pragma sw require header org.sw.demo.qtproject.qt.base.core-5

void build(Solution &s)
{
    auto &quazip = s.addTarget<LibraryTarget>("stachenov.quazip", "0.7.6");
    quazip += Git("https://github.com/stachenov/quazip", "{v}");

    quazip += "quazip/.*"_rr;

    quazip.Private += sw::Shared, "QUAZIP_BUILD"_d;
    quazip.Public += sw::Static, "QUAZIP_STATIC"_d;

    quazip.Public += "org.sw.demo.madler.zlib-1"_dep;
    quazip.Public += "org.sw.demo.qtproject.qt.base.core-5"_dep;
    automoc("org.sw.demo.qtproject.qt.base.tools.moc-5"_dep, quazip);
}
