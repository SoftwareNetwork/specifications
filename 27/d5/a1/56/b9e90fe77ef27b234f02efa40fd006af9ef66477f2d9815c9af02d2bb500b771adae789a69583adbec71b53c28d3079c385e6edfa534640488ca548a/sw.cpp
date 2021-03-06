void build(Solution &s)
{
    auto &libpq = s.addTarget<LibraryTarget>("libpq", "master");
    libpq += EmptySource();
    libpq.AutoDetectOptions = false;
    libpq.Empty = true;
    if (s.Settings.TargetOS.Type == OSType::Windows)
    {
        path base = s.Settings.TargetOS.Arch == ArchType::x86_64 ?
            "c:/Program Files/PostgreSQL" :
            "c:/Program Files (x86)/PostgreSQL"
            ;
        for (int v : {11, 10})
        {
            auto p = base / std::to_string(v);
            if (fs::exists(p))
            {
                libpq.Public.IncludeDirectories.insert(p / "include");
                libpq.Public.LinkDirectories.insert(p / "lib");
            }
        }
        libpq.Public += "libpq.lib"_lib;
    }
    else if (s.Settings.TargetOS.Type == OSType::Linux)
    {
        libpq.Public.IncludeDirectories.insert("/usr/include/postgresql");
        libpq.Public += "pq"_slib;
    }
}
