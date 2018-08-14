void build(Solution &s)
{
    auto &dirent = s.addTarget<LibraryTarget>("tronkko.dirent", "master");
    dirent += Git("https://github.com/tronkko/dirent", "", "{b}");
}