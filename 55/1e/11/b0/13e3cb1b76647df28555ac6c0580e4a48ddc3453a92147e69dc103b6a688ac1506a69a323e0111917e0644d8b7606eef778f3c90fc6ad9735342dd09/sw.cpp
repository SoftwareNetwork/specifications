void build(Solution &s)
{
    auto &tinyxml2 = s.addTarget<LibraryTarget>("leethomason.tinyxml2", "7.0.1");
    tinyxml2 += Git("https://github.com/leethomason/tinyxml2");

    tinyxml2 +=
        "tinyxml2.cpp",
        "tinyxml2.h";

    tinyxml2.Interface += sw::Shared, "TINYXML2_IMPORT"_d;
    tinyxml2 += sw::Shared, "TINYXML2_EXPORT"_d;
}
