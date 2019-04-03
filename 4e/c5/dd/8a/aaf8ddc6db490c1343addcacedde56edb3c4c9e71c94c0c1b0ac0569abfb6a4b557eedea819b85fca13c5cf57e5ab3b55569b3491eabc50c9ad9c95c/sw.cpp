void build(Solution &s)
{
    auto &p = s.addTarget<StaticLibrary>("LeonineKing1199.foxy", "master");
    p += Git("https://github.com/LeonineKing1199/foxy");

    if (s.Settings.Native.CompilerType == CompilerType::MSVC)
        p.CompileOptions.push_back("-bigobj");

    p += "org.sw.demo.boost.beast"_dep;
    p += "org.sw.demo.boost.callable_traits"_dep;
    p += "org.sw.demo.boost.hof"_dep;
    p += "org.sw.demo.boost.logic"_dep;
    p += "org.sw.demo.boost.mp11"_dep;
    p += "org.sw.demo.openssl.ssl"_dep;
}
