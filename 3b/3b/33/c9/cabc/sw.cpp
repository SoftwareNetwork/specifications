void build(Solution &sln)
{
    auto &s = sln.addDirectory("demo");
    auto &args = s.addTarget<LibraryTarget>("taywee.args", "6.2.0");
    args.Source = Git("https://github.com/Taywee/args", "{v}");

    args +=
        "args.hxx";

}
