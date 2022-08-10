void build(Solution &s)
{
    auto &t = s.addLibrary("DigiDNA.ISOBMFF", "main");
    t += Git("https://github.com/DigiDNA/ISOBMFF");

    t.setRootDirectory("ISOBMFF");
    t += sw::Shared, "ISOBMFF_DLL_BUILD"_def;
    t.Public += sw::Static, "ISOBMFF_LIB_BUILD"_def;

    auto &e = t.addExecutable("Dump");
    e.setRootDirectory("ISOBMFF-Dump");
    e += t;
}
