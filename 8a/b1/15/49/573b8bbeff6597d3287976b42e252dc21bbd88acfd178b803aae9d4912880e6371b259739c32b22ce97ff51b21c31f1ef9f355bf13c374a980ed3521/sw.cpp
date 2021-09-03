void build(Solution &s)
{
    auto &Imath = s.addLibrary("AcademySoftwareFoundation.Imath", "3.1.3");
    Imath += Git("https://github.com/AcademySoftwareFoundation/Imath", "v{v}");
    {
        Imath.Variables["IMATH_VERSION_STRING"] = "\"" + Imath.Variables["PACKAGE_VERSION"].toString() + "\"";
        Imath.Variables["IMATH_PACKAGE_STRING"] = "\"IlmBase " + Imath.Variables["PACKAGE_VERSION"].toString() + "\"";
        Imath.Variables["IMATH_VERSION_MAJOR"] = Imath.Variables["PACKAGE_VERSION_MAJOR"];
        Imath.Variables["IMATH_VERSION_MINOR"] = Imath.Variables["PACKAGE_VERSION_MINOR"];
        Imath.Variables["IMATH_VERSION_PATCH"] = Imath.Variables["PACKAGE_VERSION_PATCH"];
        Imath.Variables["IMATH_INTERNAL_NAMESPACE"] = "Imath";
        Imath.Variables["IMATH_NAMESPACE"] = "Imath";
        Imath.configureFile("config/ImathConfig.h.in", "ImathConfig.h");

        Imath.setRootDirectory("src/Imath");
        Imath -= "toFloat.cpp";
        Imath.Public += sw::Shared, "IMATH_DLL"_def;
        Imath += sw::Shared, "IMATH_EXPORTS"_def;
    }
}
