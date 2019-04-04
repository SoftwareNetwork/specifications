void build(Solution &s)
{
    auto &p = s.addTarget<LibraryTarget>("pyyaml.yaml", "0.2.2");
    p += Git("https://github.com/yaml/libyaml");

    p.Public += sw::Static, "YAML_DECLARE_STATIC"_def;
    p += sw::Shared, "YAML_DECLARE_EXPORT"_def;
    if (s.Settings.TargetOS.is(OSType::Windows))
        p.Public += "WIN32"_def;

    p += Definition("YAML_VERSION_STRING=\"" + p.Variables["PACKAGE_VERSION"].toString() + "\"");
    p += Definition("YAML_VERSION_MAJOR=" + p.Variables["PACKAGE_VERSION_MAJOR"].toString());
    p += Definition("YAML_VERSION_MINOR=" + p.Variables["PACKAGE_VERSION_MINOR"].toString());
    p += Definition("YAML_VERSION_PATCH=" + p.Variables["PACKAGE_VERSION_PATCH"].toString());
}
