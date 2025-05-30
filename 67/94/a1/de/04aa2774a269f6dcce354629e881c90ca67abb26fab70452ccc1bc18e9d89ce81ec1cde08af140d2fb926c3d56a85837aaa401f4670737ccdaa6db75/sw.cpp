void build(Solution &s)
{
    auto &spdlog = s.addTarget<LibraryTarget>("gabime.spdlog", "1.15.3");
    spdlog += Git("https://github.com/gabime/spdlog", "v{v}");

    spdlog += "include/.*"_rr;
    spdlog -= "include/spdlog/fmt/.*"_rr;
    spdlog.Public += "org.sw.demo.fmt-*"_dep;
}
