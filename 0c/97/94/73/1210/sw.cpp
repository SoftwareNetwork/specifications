void build(Solution &sln)
{
    auto &s = sln.addDirectory("demo");
    auto &dirent = s.addTarget<LibraryTarget>("tronkko.dirent", "master");
    dirent.Source = Git("https://github.com/tronkko/dirent", "", "{b}", "");
}