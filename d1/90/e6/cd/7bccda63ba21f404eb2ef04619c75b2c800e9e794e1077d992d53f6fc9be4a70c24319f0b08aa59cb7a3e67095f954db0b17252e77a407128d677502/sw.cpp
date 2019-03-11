void build(Solution &s)
{
    auto &p = s.addTarget<StaticLibrary>("DuffsDevice.tinyutf8", "master");
    p += Git("https://github.com/DuffsDevice/tinyutf8", "", "{v}");
    p.CPPVersion = CPPLanguageStandard::CPP17;
}
