void build(Solution &s)
{
    auto &mstch = s.addTarget<StaticLibraryTarget>("no1msd.mstch", "1.0.2");
    mstch += Git("https://github.com/no1msd/mstch", "1.0.2");

    mstch +=
        "include/.*\\.hpp"_rr,
        "src/.*\\.cpp"_rr,
        "src/.*\\.hpp"_rr;

    mstch +=
        "src"_id;

    mstch.Public +=
        "include"_id;

    if (mstch.getCompilerType() == CompilerType::MSVC)
        mstch.Public += "BOOST_VARIANT_DO_NOT_USE_VARIADIC_TEMPLATES"_d;

    mstch.Public += "org.sw.demo.boost.variant"_dep;
}
