void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("sqlite3", "3.38.5");
    {
        auto tv = time(0);
        t += RemoteFile("http://www.sqlite.org/" + std::to_string(localtime(&tv)->tm_year + 1900) + "/sqlite-amalgamation-{M}{m:02}{p:02}00.zip");
        t += "sqlite3.[hc]"_r;
        t += "sqlite3ext.h";
        t.ApiName = "SQLITE_API";

        t += "SQLITE_ENABLE_COLUMN_METADATA"_def;
        t += "SQLITE_ENABLE_FTS5"_def;

        if (!t.getBuildSettings().TargetOS.is(OSType::Windows))
        {
            t += "dl"_slib;
            t += "pthread"_slib;
        }
    }

    auto &sh = t.addExecutable("shell");
    sh += "shell.c";
    sh += t;
}
