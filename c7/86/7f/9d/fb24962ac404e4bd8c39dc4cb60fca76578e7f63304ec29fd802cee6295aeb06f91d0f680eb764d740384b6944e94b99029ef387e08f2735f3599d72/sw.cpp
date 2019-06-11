void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("optimad.bitpit", "master");
    t += Git("https://github.com/optimad/bitpit");

    if (!t.DryRun)
    {
        for (auto &f : fs::directory_iterator(t.SourceDir / "src"))
        {
            if (!fs::is_directory(f))
                continue;
            t += IncludeDirectory(normalize_path(f));
        }
    }
}
