void build(Solution &s)
{
    auto &jsoncpp = s.addTarget<LibraryTarget>("open_source_parsers.jsoncpp", "1.9.4");
    jsoncpp += Git("https://github.com/open-source-parsers/jsoncpp", "{v}");

    jsoncpp += cpp11;
    jsoncpp +=
        "include/.*\\.h"_rr,
        "src/lib_json/.*\\.cpp"_rr,
        "src/lib_json/.*\\.h"_rr,
        "src/lib_json/.*\\.inl"_rr;

    jsoncpp.Interface += sw::Shared, "JSON_DLL"_d;
    jsoncpp += sw::Shared, "JSON_DLL_BUILD"_d;
}
