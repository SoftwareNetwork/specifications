void build(Solution &sln)
{
    auto &s = sln.addDirectory("demo");
    auto &stacktrace = s.addTarget<LibraryTarget>("apolukhin.stacktrace", "master");
    stacktrace.Source = Git("https://github.com/apolukhin/stacktrace", "{b}");

    stacktrace +=
        "include/.*"_rr;

    stacktrace.Public +=
        "include"_id;


    stacktrace.Public += "pub.cppan2.demo.boost.tokenizer"_dep;
    stacktrace.Public += "pub.cppan2.demo.boost.algorithm"_dep;
    stacktrace.Public += "pub.cppan2.demo.boost.align"_dep;
    stacktrace.Public += "pub.cppan2.demo.boost.intrusive"_dep;
    stacktrace.Public += "pub.cppan2.demo.boost.preprocessor"_dep;
    stacktrace.Public += "pub.cppan2.demo.boost.static_assert"_dep;
    stacktrace.Public += "pub.cppan2.demo.boost.array"_dep;
    stacktrace.Public += "pub.cppan2.demo.boost.iostreams"_dep;
    stacktrace.Public += "pub.cppan2.demo.boost.chrono"_dep;
    stacktrace.Public += "pub.cppan2.demo.boost.endian"_dep;
    stacktrace.Public += "pub.cppan2.demo.boost.assert"_dep;
    stacktrace.Public += "pub.cppan2.demo.boost.concept_check"_dep;
    stacktrace.Public += "pub.cppan2.demo.boost.typeof"_dep;
    stacktrace.Public += "pub.cppan2.demo.boost.atomic"_dep;
    stacktrace.Public += "pub.cppan2.demo.boost.lambda"_dep;
    stacktrace.Public += "pub.cppan2.demo.boost.exception"_dep;
    stacktrace.Public += "pub.cppan2.demo.boost.bind"_dep;
    stacktrace.Public += "pub.cppan2.demo.boost.type_traits"_dep;
    stacktrace.Public += "pub.cppan2.demo.boost.config"_dep;
    stacktrace.Public += "pub.cppan2.demo.boost.mpl"_dep;
    stacktrace.Public += "pub.cppan2.demo.boost.container"_dep;
    stacktrace.Public += "pub.cppan2.demo.boost.thread"_dep;
    stacktrace.Public += "pub.cppan2.demo.boost.integer"_dep;
    stacktrace.Public += "pub.cppan2.demo.boost.function"_dep;
    stacktrace.Public += "pub.cppan2.demo.boost.conversion"_dep;
    stacktrace.Public += "pub.cppan2.demo.boost.range"_dep;
    stacktrace.Public += "pub.cppan2.demo.boost.core"_dep;
    stacktrace.Public += "pub.cppan2.demo.boost.date_time"_dep;
    stacktrace.Public += "pub.cppan2.demo.boost.detail"_dep;
    stacktrace.Public += "pub.cppan2.demo.boost.filesystem"_dep;
    stacktrace.Public += "pub.cppan2.demo.boost.tuple"_dep;
    stacktrace.Public += "pub.cppan2.demo.boost.optional"_dep;
    stacktrace.Public += "pub.cppan2.demo.boost.io"_dep;
    stacktrace.Public += "pub.cppan2.demo.boost.foreach"_dep;
    stacktrace.Public += "pub.cppan2.demo.boost.function_types"_dep;
    stacktrace.Public += "pub.cppan2.demo.boost.functional"_dep;
    stacktrace.Public += "pub.cppan2.demo.boost.fusion"_dep;
    stacktrace.Public += "pub.cppan2.demo.boost.iterator"_dep;
    stacktrace.Public += "pub.cppan2.demo.boost.unordered"_dep;
    stacktrace.Public += "pub.cppan2.demo.boost.serialization"_dep;
    stacktrace.Public += "pub.cppan2.demo.boost.lexical_cast"_dep;
    stacktrace.Public += "pub.cppan2.demo.boost.rational"_dep;
    stacktrace.Public += "pub.cppan2.demo.boost.locale"_dep;
    stacktrace.Public += "pub.cppan2.demo.boost.smart_ptr"_dep;
    stacktrace.Public += "pub.cppan2.demo.boost.math"_dep;
    stacktrace.Public += "pub.cppan2.demo.boost.move"_dep;
    stacktrace.Public += "pub.cppan2.demo.boost.phoenix"_dep;
    stacktrace.Public += "pub.cppan2.demo.boost.numeric"_dep;
    stacktrace.Public += "pub.cppan2.demo.boost.pool"_dep;
    stacktrace.Public += "pub.cppan2.demo.boost.predef"_dep;
    stacktrace.Public += "pub.cppan2.demo.boost.proto"_dep;
    stacktrace.Public += "pub.cppan2.demo.boost.variant"_dep;
    stacktrace.Public += "pub.cppan2.demo.boost.random"_dep;
    stacktrace.Public += "pub.cppan2.demo.boost.ratio"_dep;
    stacktrace.Public += "pub.cppan2.demo.boost.system"_dep;
    stacktrace.Public += "pub.cppan2.demo.boost.regex"_dep;
    stacktrace.Public += "pub.cppan2.demo.boost.spirit"_dep;
    stacktrace.Public += "pub.cppan2.demo.boost.throw_exception"_dep;
    stacktrace.Public += "pub.cppan2.demo.boost.tti"_dep;
    stacktrace.Public += "pub.cppan2.demo.boost.type_index"_dep;
    stacktrace.Public += "pub.cppan2.demo.boost.winapi"_dep;
    stacktrace.Public += "pub.cppan2.demo.boost.utility"_dep;
}