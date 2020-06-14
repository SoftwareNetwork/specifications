void build(Solution &s)
{
    auto &t = s.addTarget<Library>("cegui.cegui", "master");
    t += Git("https://github.com/cegui/cegui");

    t.setRootDirectory("cegui");

    t += cpp11;
    t.Public += sw::Static, "CEGUI_STATIC"_def;

    t.Variables["CEGUI_USE_GLEW"] = 1;
    t.Variables["CEGUI_HAS_STD11_REGEX"] = 1;
    t.Variables["CEGUI_STRING_CLASS"] = "CEGUI_STRING_CLASS_UTF_8";

    t.Variables["CEGUI_VERSION_MAJOR"] = t.Variables["PACKAGE_VERSION_MAJOR"];
    t.Variables["CEGUI_VERSION_MINOR"] = t.Variables["PACKAGE_VERSION_MINOR"];
    t.Variables["CEGUI_VERSION_PATCH"] = t.Variables["PACKAGE_VERSION_PATCH"];

    t.configureFile("include/CEGUI/Config.h.in", "CEGUI/Config.h");
    t.configureFile("include/CEGUI/ModuleConfig.h.in", "CEGUI/ModuleConfig.h");
    t.configureFile("include/CEGUI/Version.h.in", "CEGUI/Version.h");

    t += "org.sw.demo.boost.python"_dep;
    t += "org.sw.demo.g_truc.glm"_dep;
    t += "org.sw.demo.glew"_dep;
    t += "org.sw.demo.xmlsoft.libxml2"_dep;
}
