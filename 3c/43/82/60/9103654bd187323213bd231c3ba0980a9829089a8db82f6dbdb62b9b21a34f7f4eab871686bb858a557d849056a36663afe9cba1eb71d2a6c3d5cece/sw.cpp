void build(Solution &s)
{
    auto &args = s.addTarget<LibraryTarget>("taywee.args", "6.2.1");
    args += Git("https://github.com/Taywee/args", "{v}");
    args += "args.hxx";
}
