void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("sqlite3", "3.30.1");
    t += RemoteFile("http://www.sqlite.org/2019/sqlite-amalgamation-{M}{m:02}{p:02}00.zip");
    t += "sqlite3.[hc]"_r;
    t += "sqlite3ext.h";
    t.ApiName = "SQLITE_API";
    t += "SQLITE_ENABLE_COLUMN_METADATA"_def;
}
