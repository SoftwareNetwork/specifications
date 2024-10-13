void build(Solution &s)
{
    auto &t = s.addStaticLibrary("whoshuu.cpr", "1.11.0");
    t += Git("https://github.com/whoshuu/cpr");

    t += cpp17;

    t += "include/.*"_rr;
    t += "cpr/.*"_rr;

    t.Public += "org.sw.demo.badger.curl.libcurl"_dep;

    t.Variables["cpr_VERSION"] = t.Variables["PACKAGE_VERSION"];
    t.Variables["cpr_VERSION_MAJOR"] = t.Variables["PACKAGE_VERSION_MAJOR"];
    t.Variables["cpr_VERSION_MINOR"] = t.Variables["PACKAGE_VERSION_MINOR"];
    t.Variables["cpr_VERSION_PATCH"] = t.Variables["PACKAGE_VERSION_PATCH"];
    t.Variables["cpr_VERSION_NUM"] = "CPR_VERSION_MAJOR * 0x10000 + cpr_VERSION_MINOR * 0x100 + cpr_VERSION_PATCH";
    t.configureFile("cmake/cprver.h.in", "cpr/cprver.h");
}
