void build(Solution &s)
{
    auto &mach7 = s.addTarget<LibraryTarget>("mach7", "master");
    mach7 += Git("https://github.com/solodon4/Mach7", "", "{v}");

    mach7 +=
        "code/mach7/.*"_rr;

    mach7.Public +=
        "code"_id;
}
