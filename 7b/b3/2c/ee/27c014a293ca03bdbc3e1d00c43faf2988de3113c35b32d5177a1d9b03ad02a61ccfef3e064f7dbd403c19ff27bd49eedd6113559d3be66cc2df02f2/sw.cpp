void build(Solution &s)
{
    auto &t = s.addTarget<StaticLibraryTarget>("ngtcp2.sfparse", "main");
    t += Git("https://github.com/ngtcp2/sfparse");

    t += "sfparse.[hc]"_r;

    if (!t.DryRun) {
        fs::create_directories(t.BinaryDir / "sfparse");
        fs::copy_file(t.SourceDir / "sfparse.h", t.BinaryDir / "sfparse" / "sfparse.h", fs::copy_options::update_existing);
    }
}
