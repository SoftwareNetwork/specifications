void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("sqlite3", "3.49.2");
    {
        auto tv = time(0);
        t += RemoteFile("http://www.sqlite.org/" + std::to_string(localtime(&tv)->tm_year + 1900) + "/sqlite-amalgamation-{M}{m:02}{p:02}00.zip");
        t += "sqlite3.[hc]"_r;
        t += "sqlite3ext.h";
        t.ApiName = "SQLITE_API";

        t += "SQLITE_ENABLE_COLUMN_METADATA"_def;
        t += "SQLITE_ENABLE_FTS5"_def;

        if (!t.getBuildSettings().TargetOS.is(OSType::Windows) && !t.getBuildSettings().TargetOS.is(OSType::Mingw))
        {
            t += "dl"_slib;
            if (!t.getBuildSettings().TargetOS.Android)
                t += "pthread"_slib;
        }
    }

    auto &sh = t.addExecutable("shell");
    sh += "shell.c";
    sh += t;
}
