void build(Solution &s)
{
    auto &catch2 = s.addTarget<LibraryTarget>("catchorg.catch2", "2.8.0");
    catch2 += Git("https://github.com/catchorg/Catch2", "v{v}");

    catch2 +=
        "single_include/.*"_rr;
    catch2.Public +=
        "single_include"_id;
}
