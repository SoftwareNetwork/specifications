void build(Solution &s)
{
    auto &p = s.addTarget<LibraryTarget>("dkormalev.asynqro", "0.2.0");
    p += Git("https://github.com/dkormalev/asynqro");
    p.CPPVersion = CPPLanguageStandard::CPP17;
    p.ApiName = "ASYNQRO_EXPORT";
    p.writeFileOnce("asynqro/impl/asynqro_export.h");
}
