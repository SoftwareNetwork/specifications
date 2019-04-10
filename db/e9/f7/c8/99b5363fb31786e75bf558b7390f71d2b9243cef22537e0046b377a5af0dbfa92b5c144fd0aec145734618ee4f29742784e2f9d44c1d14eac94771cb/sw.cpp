void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("objectcomputing.mFAST", "master");
    t += Git("https://github.com/objectcomputing/mFAST");

    t.ApiNames.insert("MFAST_EXPORT");
    t.ApiNames.insert("MFAST_XML_PARSER_EXPORT");
    t.ApiNames.insert("MFAST_SQLITE3_EXPORT");
    t.ApiNames.insert("MFAST_JSON_EXPORT");
    t.ApiNames.insert("MFAST_CODER_EXPORT");

    t.Public += "NOMINMAX"_def;

    t.Public += "org.sw.demo.boost.assign"_dep;
    t.Public += "org.sw.demo.sqlite3"_dep;
    t.Public += "org.sw.demo.leethomason.tinyxml2"_dep;

    t.patch("src/mfast/xml_parser/templates_builder.cpp", "XMLDocument", "tinyxml2::XMLDocument");
}
