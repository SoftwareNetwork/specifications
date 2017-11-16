void build(Solution &sln)
{
    auto &s = sln.addDirectory("demo");
    auto &argagg = s.addTarget<LibraryTarget>("vietjtnguyen.argagg", "0.4.6");
    argagg.Source = Git("https://github.com/vietjtnguyen/argagg", "{v}");
}