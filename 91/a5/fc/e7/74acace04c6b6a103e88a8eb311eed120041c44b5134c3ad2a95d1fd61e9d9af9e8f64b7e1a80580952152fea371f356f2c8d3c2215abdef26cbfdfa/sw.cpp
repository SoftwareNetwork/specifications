void build(Solution &s)
{
    auto &libpq = s.addTarget<LibraryTarget>("find.libpq", "master");
    libpq += EmptySource();
    libpq.AutoDetectOptions = false;
    libpq.Empty = true;
    if (libpq.getSettings().TargetOS.Type == OSType::Windows)
    {
        path base = libpq.getSettings().TargetOS.Arch == ArchType::x86_64 ?
            "c:/Program Files/PostgreSQL" :
            "c:/Program Files (x86)/PostgreSQL"
            ;
        for (int v : {13, 12, 11, 10})
        {
            auto p = base / std::to_string(v);
            if (fs::exists(p))
            {
                libpq.Public.IncludeDirectories.insert(p / "include");
                libpq.Public.LinkLibraries.insert(p / "lib" / "libpq.lib");
            }
        }
    }
    else if (libpq.getSettings().TargetOS.Type == OSType::Linux)
    {
        libpq.Public.IncludeDirectories.insert("/usr/include/postgresql");
        libpq.Public += "pq"_slib;
    }
}
