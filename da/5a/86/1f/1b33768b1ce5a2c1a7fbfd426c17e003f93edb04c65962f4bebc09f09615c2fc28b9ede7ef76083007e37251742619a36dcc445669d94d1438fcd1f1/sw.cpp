void build(Solution &s)
{
    auto &tls = s.addTarget<StaticLibraryTarget>("mbed.tls", "2.16.0");
    tls += Git("https://github.com/ARMmbed/mbedtls", "mbedtls-{v}");
}
