void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("brechtsanders.xlsxio", "0.2.21");
    t += Git("https://github.com/brechtsanders/xlsxio");

    t.ApiName = "DLL_EXPORT_XLSXIO";

    t += "include/.*"_rr;
    t += "lib/.*"_rr;

    t += "org.sw.demo.expat"_dep;
    t += "org.sw.demo.nih_at.libzip"_dep;
}
