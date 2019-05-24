void build(Solution &s)
{
    auto &t = s.addLibrary("dmlc.rabit", "master");
    t += Git("https://github.com/dmlc/rabit");

    t +=
        "src/allreduce_base.cc",
        "src/allreduce_robust.cc",
        "src/engine.cc",
        "src/c_api.cc"
        ;

    t += "org.sw.demo.dmlc.core-master"_dep;

    if (t.DryRun)
        return;

    auto io = t.getFile("org.sw.demo.dmlc.core-master"_dep, "include/dmlc/io.h");
    for (auto &[p, sf] : t)
    {
        if (auto nsf = sf->as<NativeSourceFile>())
        {
            if (auto c = nsf->compiler->as<VisualStudioCompiler>())
            {
                c->ForcedIncludeFiles().push_back(io);
            }
            // add more compilers
        }
    }
}
