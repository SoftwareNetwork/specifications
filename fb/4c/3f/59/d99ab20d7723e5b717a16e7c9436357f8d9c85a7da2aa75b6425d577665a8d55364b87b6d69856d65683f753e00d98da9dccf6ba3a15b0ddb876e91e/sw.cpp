void build(Solution &s)
{
    auto &t = s.addLibrary("mariusmuja.flann", "master");
    t += Git("https://github.com/mariusmuja/flann");

    t.setRootDirectory("src/cpp");
    t += "flann/.*"_rr;
    t -= "flann/mpi/.*"_rr;

    t += sw::Shared, "FLANN_EXPORTS"_def;
    t.Public += sw::Static, "FLANN_STATIC"_def;

    if (t.getBuildSettings().TargetOS.is(OSType::Windows))
        t += "WIN32"_def;

    //t += "org.sw.demo.hdfgroup.hdf5.hdf5"_dep;
    t.Public += "org.sw.demo.lz4"_dep;

    if (t.getCompilerType() == CompilerType::MSVC)
        t.CompileOptions.push_back("-bigobj");
}
