void build(Solution &s)
{
    auto &t = s.addTarget<StaticLibraryTarget>("google.cpu_features", "0.4.1");
    t += Git("https://github.com/google/cpu_features", "v{v}");

    t += "include/.*"_rr;
    t += "src/.*"_rr;

    t -= "src/.*aarch64.*"_rr;
    t -= "src/.*arm.*"_rr;
    t -= "src/.*mips.*"_rr;
    t -= "src/.*ppc.*"_rr;

    t += "STACK_LINE_READER_BUFFER_SIZE=1024"_def;
}
