void build(Solution &s)
{
    auto &fmt = s.addTarget<LibraryTarget>("fmt", "5.1.0");
    fmt += Git("https://github.com/fmtlib/fmt", "{v}");

    fmt +=
        "fmt/format.*"_rr,
        "fmt/ostream.*"_rr;

    fmt.Public +=
        "fmt"_id,
        "."_id;

    fmt.Private += sw::Shared, "FMT_EXPORT"_d;
    fmt.Public += sw::Shared, "FMT_SHARED"_d;
}
