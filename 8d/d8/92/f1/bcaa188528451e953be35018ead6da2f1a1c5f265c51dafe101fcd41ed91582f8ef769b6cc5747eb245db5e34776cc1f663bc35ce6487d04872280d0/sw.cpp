void build(Solution &s)
{
    auto &t = s.addStaticLibrary("ArthurSonzogni.FTXUI", "7.0.0");
    t += Git("https://github.com/ArthurSonzogni/FTXUI");

    t += cpp20;
    t -= "src/.*_test.cpp"_rr;
    t += "UNICODE"_def;
}
