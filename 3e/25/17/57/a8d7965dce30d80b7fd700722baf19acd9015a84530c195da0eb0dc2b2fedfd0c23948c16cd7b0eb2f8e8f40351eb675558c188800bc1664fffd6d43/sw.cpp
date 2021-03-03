void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("amazon.awslabs.lc", "main");
    t += Git("https://github.com/awslabs/aws-lc", "main");

    t -= "include/.*"_rr;
    t += "crypto/.*"_rr;
    t += "ssl/.*"_r;
    t += "third_party/fiat/.*"_r;

    t += "generated-src/.*\\.c"_r;

    String dir;
    if (t.getBuildSettings().TargetOS.Type == OSType::Windows)
        dir = "win";
    else if (t.getBuildSettings().TargetOS.Type == OSType::Macos)
        dir = "mac";
    else if (t.getBuildSettings().TargetOS.Type == OSType::Linux)
        dir = "linux";
    // also ios

    String arch;
    if (t.getBuildSettings().TargetOS.Arch == ArchType::x86)
        arch = "x64";
    else if (t.getBuildSettings().TargetOS.Arch == ArchType::x86_64)
        arch = "x86_64";
    // aarch64
    // arm
    // ppc64le

    t -= "generated-src/.*/.*"_rr;
    t += FileRegex("generated-src/" + dir + "-" + arch, ".*", true);
    t -= "generated-src/.*/test/.*"_rr;

    t -= "crypto/fipsmodule/.*"_rr;
    t += "crypto/fipsmodule/.*"_r;
    t -= "crypto/test/.*"_rr;
    t -= "crypto/.*test\\.cc"_rr;
    t -= "ssl/.*_test.cc"_rr;

    t.Public += sw::Shared, "BORINGSSL_SHARED_LIBRARY"_d;
    t += sw::Shared, "BORINGSSL_IMPLEMENTATION"_d;

    if (t.getBuildSettings().TargetOS.Type != OSType::Windows)
        t += "pthread"_slib;
}
