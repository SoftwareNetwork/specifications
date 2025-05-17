void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("amazon.awslabs.c_sdkutils", "0.2.4");
    t += Git("https://github.com/awslabs/aws-c-sdkutils", "v{v}");

    t.Private += sw::Shared, "AWS_SDKUTILS_EXPORTS"_d;
    t.Public += sw::Shared, "AWS_SDKUTILS_USE_IMPORT_EXPORT"_d;

    t.Public += "org.sw.demo.amazon.awslabs.c_common"_dep;
}
