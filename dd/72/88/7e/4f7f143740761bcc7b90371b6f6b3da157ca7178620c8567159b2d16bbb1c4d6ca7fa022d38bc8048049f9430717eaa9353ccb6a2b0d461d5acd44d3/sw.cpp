void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("brainboxdotcc.dpp", "10.1.2");
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
    t += "org.sw.demo.xiph.opus"_dep;
    t += "org.sw.demo.cisco.mlspp-master"_dep;
    t.Public += "DPP_USE_EXTERNAL_JSON"_def;
    t.Public += "HAVE_VOICE"_def;
    t -= "src/dpp/voice/stub/.*"_rr;
    t.Public += "org.sw.demo.nlohmann.json.natvis"_dep;
    if (t.getCompilerType() == CompilerType::MSVC) {
        t.CompileOptions.push_back("/bigobj");
    }
    if (t.getBuildSettings().TargetOS.Type == OSType::Windows)
    {
        t -= "src/dpp/socketengines/epoll.cpp";
        t -= "src/dpp/socketengines/kqueue.cpp";
    }
    if (t.getBuildSettings().TargetOS.Type == OSType::Linux)
    {
        t -= "src/dpp/socketengines/poll.cpp";
        t -= "src/dpp/socketengines/kqueue.cpp";
    }
    if (t.getBuildSettings().TargetOS.isApple())
    {
        t -= "src/dpp/socketengines/poll.cpp";
        t -= "src/dpp/socketengines/epoll.cpp";
    }
    //t.pushBackToFileOnce("include/dpp/nlohmann/json_fwd.hpp", "#include \"json.hpp\"");

    t += "mlspp/include/.*"_rr;
    t += "mlspp/src/.*"_rr;
    t += "mlspp/lib/bytes/.*"_rr;
    t += "mlspp/lib/tls_syntax/.*"_rr;
    t += "mlspp/lib/hpke/.*"_rr;

    t.Public += "include"_idir;
    t += "mlspp/include"_idir;
    t += "mlspp/lib/bytes/include"_idir;
    t += "mlspp/lib/tls_syntax/include"_idir;
    t += "mlspp/lib/hpke/include"_idir;
}
