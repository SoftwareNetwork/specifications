void build(Solution &sln)
{
    auto &s = sln.addDirectory("demo");
    auto &nowide = s.addTarget<LibraryTarget>("artyom_beilis.nowide", "master");
    nowide.Source = Git("https://github.com/artyom-beilis/nowide", "", "{b}");

    nowide +=
        "include/.*"_rr,
        "src/.*"_rr;

    nowide.Private +=
        "src"_id;

    nowide.Public +=
        "include"_id;

    nowide.Private += sw::Shared, "NOWIDE_SOURCE"_d;
    nowide.Public += sw::Shared, "NOWIDE_EXPORT"_d;

    nowide.Private += "BOOST_NOWIDE_BUILDING_THE_LIB"_d;
    nowide.Private += "BOOST_NOWIDE_SOURCE"_d;

    nowide.Public += sw::Static, "BOOST_NOWIDE_BUILD_LIB"_d;
    nowide.Public += sw::Static, "BOOST_NOWIDE_STATIC_LINK"_d;

    nowide.Private += sw::Shared, "BOOST_NOWIDE_BUILD_DLL"_d;

    nowide.Public += sw::Shared, "BOOST_NOWIDE_DYN_LINK"_d;
    nowide.Public += sw::Shared, "BOOST_NOWIDE_USE_DLL"_d;

    nowide.Public += "pub.cppan2.demo.boost.config-1"_dep;
    nowide.Public += "pub.cppan2.demo.boost.locale-1"_dep;
}