void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("amazon.awslabs.c_s3", "0.3.22");
    t += Git("https://github.com/awslabs/aws-c-s3", "v{v}");

    t.Private += sw::Shared, "AWS_S3_EXPORTS"_d;
    t.Public += sw::Shared, "AWS_S3_USE_IMPORT_EXPORT"_d;

    t.Public += "org.sw.demo.amazon.awslabs.c_auth"_dep;
    t.Public += "org.sw.demo.amazon.awslabs.checksums"_dep;
}
