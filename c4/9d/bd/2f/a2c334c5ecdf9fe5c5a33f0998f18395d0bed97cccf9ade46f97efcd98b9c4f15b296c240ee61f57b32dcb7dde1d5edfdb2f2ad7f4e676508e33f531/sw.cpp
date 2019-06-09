void build(Solution &s)
{
    auto &t = s.addTarget<StaticLibraryTarget>("lewissbaker.cppcoro", "master");
    t += Git("https://github.com/lewissbaker/cppcoro");

    t += cpp17;
    t += "include/.*"_rr;
    t += "lib/.*"_rr;

    if (t.getCompilerType() == CompilerType::MSVC)
    {
        for (auto &[p, sf] : t)
            sf->args.push_back("-await");
    }
}
