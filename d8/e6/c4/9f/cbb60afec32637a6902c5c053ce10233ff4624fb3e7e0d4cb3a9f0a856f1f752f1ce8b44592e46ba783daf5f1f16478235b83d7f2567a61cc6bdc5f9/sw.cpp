void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("brainboxdotcc.dpp", "10.0.29");
    t += Git("https://github.com/brainboxdotcc/DPP", "v{v}");

    t += cpp20;
    t += "include/dpp/.*"_rr;
    t += "src/dpp/.*"_rr;
    t.Public += sw::Static, "DPP_STATIC"_def;
    t += "DPP_BUILD"_def;
    t.Public += "include"_idir;
    //t += "include/dpp"_idir;
    t += "org.sw.demo.madler.zlib"_dep;
    t += "org.sw.demo.openssl.ssl"_dep;
    t.Public += "DPP_USE_EXTERNAL_JSON"_def;
    t.Public += "org.sw.demo.nlohmann.json.natvis"_dep;
    if (t.getCompilerType() == CompilerType::MSVC)
        t.CompileOptions.push_back("/bigobj");
    //t.pushBackToFileOnce("include/dpp/nlohmann/json_fwd.hpp", "#include \"json.hpp\"");
}
