void build(Solution &s)
{
    auto &p = s.addStaticLibrary("lemire.SIMDxorshift", "master");
    p += Git("https://github.com/lemire/SIMDxorshift");
}
