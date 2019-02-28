void build(Solution &s)
{
    auto &sqlite3 = s.addTarget<LibraryTarget>("sqlite3", "3.27.0");
    sqlite3 += RemoteFile("http://www.sqlite.org/2019/sqlite-amalgamation-{M}{m:02}{p:02}00.zip");
    sqlite3 += "sqlite3.[hc]"_r;
    sqlite3 += "sqlite3ext.h";
    sqlite3.ApiName = "SQLITE_API";
}
