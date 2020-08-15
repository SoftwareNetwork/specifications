template <class T>
auto &addBoostTarget(Solution &s, const String &name)
{
    static const std::map<String, String> commits
    {
        { "accumulators", "b12a189fefd17ff3a6a68fc24b4135f0e0c498f6" },
        { "algorithm", "7f527f3c4dc3b88b11f1584d7decf0537450b441" },
        { "align", "05bfdf34c047418b7525502d76f95918190f8dc2" },
        { "any", "1483752ab69e064e4816e7a726fcfaff62712785" },
        { "array", "d026fbcee301275a101c5c67e1f4161766028108" },
        { "asio", "f550ad31912783269bfca3405b9344abc8394849" },
        { "assert", "35c31e7778cd5e02cdcdea7bedce4c7f8d767803" },
        { "assign", "d30e5c2d3796740445e992230eb7e4d7712d219b" },
        { "atomic", "51c84b6d0376f969ab5e70e83457446521bbbd38" },
        { "beast", "201105e66ab738bd8e027cff8c11ddd7cd5c7587" },
        { "bimap", "a4317e5c82589d5635a27c2fea02003eeabd072c" },
        { "bind", "cd32792f0ef444347b1a522de945ee298e1112a8" },
        { "callable_traits", "d0d48799a0add36b3f1a3611d80499c80d59f539" },
        { "chrono", "aa51cbd5121ed29093484f53e5f96e13a9a915b4" },
        { "circular_buffer", "eed21df804f25e3d65af65722c384577a305b1e0" },
        { "compatibility", "445fbc1633cbaf49b0c3310db95a642e9c3fe89b" },
        { "compute", "36c89134d4013b2e5e45bc55656a18bd6141995a" },
        { "concept_check", "a8cf003d4a5d0328b1c58610b2f5a7a84994b357" },
        { "config", "83fd6b3e9d3709d3efdc609a5aceb9d5851212b0" },
        { "container", "06417de498fbfd6f6684efe85a99c6adcc0721a5" },
        { "container_hash", "8a7fc581ed987fde8cae4e7145378d23b9b6b351" },
        { "context", "ca52f2da4908fbee36c5e0b00f800793f6b52b50" },
        { "contract", "3c77288d52fd19ef8c317bbecc7b08362251ff3e" },
        { "conversion", "9540be42b4fa1ea66ae850f233e3ddff45e9efeb" },
        { "convert", "2e3386ff0f5ee4728696fb0d9853feb844cae993" },
        { "core", "dcc04c55089b8a74c5a37e3d25694fca697ffb9f" },
        { "coroutine", "69ba7f82fd1fad814e2c66a01290bf3f9c575176" },
        { "coroutine2", "32999f282aa05638c59bbe2ba0db888e32d3ffab" },
        { "crc", "7002d9850d78c279c28724b2e58227d9b8bf7422" },
        { "date_time", "fa46e55ca8089d51fbf61e4c9da475a037a7be19" },
        { "detail", "5d285a2d5a561308ad6703e1ff284d106eb01032" },
        { "dll", "6fed722419caec8ff1049ea0e78afcd75daa6494" },
        { "dynamic_bitset", "b59fc97a670a1cd1866ea4a46612525a17752978" },
        { "endian", "dffcf99ca6a3d772dfdd880dd8ddff3c10941ee7" },
        { "exception", "63b57749a2e1fc1505ec8a382c82cf32103ab68c" },
        { "fiber", "680c0ac7b9cf1f223e2c1ed307d828db3d52c53a" },
        { "filesystem", "4748f6e39da4948468391705fdd140433a7df21c" },
        { "flyweight", "6dd4814faf0e62754040a2d3cc6498258be3318c" },
        { "foreach", "88c8eae6759f1984dd7e22629660f0e39c5907ee" },
        { "format", "5884c3d8140cdcd2d1bef374fcc216244d5faaa5" },
        { "function", "4841cbf4754043a8bcf02fc081106c6f26259324" },
        { "function_types", "0f29b0147885e525acc87e86169f538e55bdb3e9" },
        { "functional", "28b1b25b9464abb3fd5783e10f49df40d6aac2ff" },
        { "fusion", "c46e492d4556b97da12f35e8d9a4e0eef1ad7bde" },
        { "geometry", "c6fc80093b8cb815191876cd479993686b4978f3" },
        { "gil", "10d91ee5ab9f09a848509c924289c40f7b727ff3" },
        { "graph", "554da2950e99636b9bc068785c611ca6e05d3a4d" },
        { "graph_parallel", "22385ccb6c35fcb07971f295807519bd031ed57d" },
        { "hana", "07b42492765f7384e053c4761f4d0eda32b75834" },
        { "heap", "c0a8773051cd8bc0691ea3c067d59b4d4c5e96dc" },
        { "histogram", "f6563de91172c805b6a030dea2cd149d32159c82" },
        { "hof", "a5fcba3ae4410d217a7a80a782238bb18fa7925a" },
        { "icl", "c8135f85bff06d0e4dfbec0c1a26c2e0034da881" },
        { "integer", "54dcfa677d8883d0bc7179f32cb806d89d4f8e40" },
        { "interprocess", "056663fcc1db321c0af1f56a7c7bd1becf20791f" },
        { "intrusive", "d8761780b0386c53ab144173a5fdc536ee2e1da8" },
        { "io", "40b87939b2ce6a0d607e37877f2e36bbaf1f23f7" },
        { "iostreams", "bc868902b0e42a0ff7440d52c584a14a04d0fad3" },
        { "iterator", "72f0ebe8d06991968c6f792c0a846519a795cac1" },
        { "lambda", "8824e81d52489be9c8cb21d8f8477b03e675ce66" },
        { "lexical_cast", "9fdbf5ce67d9a9302cf16de4760d99090da97f44" },
        { "local_function", "0a6ceaaec6cc849b38c3475340fce74fc3960e51" },
        { "locale", "ccb8fbb9a1a0dbdffb1054ffa34e4aba1e425642" },
        { "lockfree", "dcd878cd67b3dc710a0f37bbbb2be0ea4a7526e3" },
        { "log", "e7d4fd2d13f753cee86ac2ffd82259ab460e1237" },
        { "logic", "1a0a89c5bdd096777c59a7e43dc4cdfb8f6b1cc7" },
        { "math", "692077bf1647c87117929ec111788e67a8bbf68a" },
        { "metaparse", "e0350c0bfe92a257bf0be2083f2a003f237b5dd8" },
        { "move", "fa3aa147b4b0a8e02edb0ac516acc21b4b139b78" },
        { "mp11", "29764aad4881fde809af6a025c12012e47a55515" },
        { "mpi", "ace80263884ee2af3cef2ab3644ff4b41d154e49" },
        { "mpl", "9c0f98f088bbed5943382ebfcfd1b0159aaa7462" },
        { "msm", "2286d12deb237e8325c00fcb28c666190daca2ac" },
        { "multi_array", "3b405720559ff9acc968e1a40102ca6245a64e6c" },
        { "multi_index", "1dcb5fe1370d7f76d4f3fe844533fc6fee30452a" },
        { "multiprecision", "953d6e5b8b3eb91b9fde24606d7d328e16ab2fe7" },
        { "nowide", "5d4444622fae8b10f4840d97bc83bd95d879c531" },
        { "numeric", "de922921408bc7116a3c7c67b2fcfbe3e3214f3b" },
        { "interval", "ed01cdab6d89579e56ed5b831ed339fe98deba98" },
        { "odeint", "db8f91a51da630957d6bfa1ff87be760b0be97a6" },
        { "ublas", "53361d7f42e7d88661c147a2490e10ee32324e48" },
        { "optional", "c486bb724c087b18a1f9a6397408213219a4f3e2" },
        { "outcome", "6946e340c798fc312c48e28762737c77378e99da" },
        { "parameter", "c31433af1f41d228bbf4c6161c1b2e028cbfd2ad" },
        { "parameter_python", "787d8d38d9fd49c34a757b20361f8042dd5ac820" },
        { "phoenix", "ba414c01c82bed1d4ff22a0c155098d94691f8e2" },
        { "poly_collection", "c00e7bcf9deece68e5c69a0ba31fb77145b775bb" },
        { "polygon", "335650e36062db5638f143a958ee6995ada1de3e" },
        { "pool", "8edafbec99cefa00b84b1c95e5b3cbbf9a6a5498" },
        { "predef", "133ae86ef5a7811321b2dae92455ba609418885e" },
        { "preprocessor", "e4fb5f4866f69a917b7eeb522a3257d4625e52c1" },
        { "process", "b2a96a3e139ae42ea2bcb80cc58f108494a995c5" },
        { "program_options", "bc0c90a70f0435ee4ce65b97a68d864a7d627e35" },
        { "property_map", "a1510e7e0fb6914c49109ceee1b2c5a83dc84428" },
        { "property_tree", "bdfe275d172ac30bc5e89a6375a5a64dea20b3c0" },
        { "proto", "333db1d6e21ad6a4d543c0a6e837a2ef3e155f39" },
        { "ptr_container", "c7b6a7ee5b41d378c4c95d6f6edb25c98dfffed7" },
        { "python", "102acf1d34ff91e67fe9aae401edc6edd68be89e" },
        { "qvm", "4e5548998c9d2d8159b90ae3a4244e58c59bc285" },
        { "random", "5d9ba8578ac1d78525488077dafb99b536a399d3" },
        { "range", "f3ff60c3bc41e42ffebef1452deb285047a5639f" },
        { "ratio", "96888514780dbe8d1729a8248fbafdeda79bcdc4" },
        { "rational", "47c1b26964b06a99200769474d58a309267c50f1" },
        { "regex", "e9645776f90c1ad64fc92de32ece96a5727e7b99" },
        { "safe_numerics", "94cbf677737563c8e7ab55d95ba63052e7b6cc88" },
        { "scope_exit", "3a2c1d7da44af2bb4bf47a4dc051998a80d3194f" },
        { "serialization", "ed44fcd268e0691e8afe09d879a107fbff194d75" },
        { "signals2", "dd4c67ff88c1663f5209729ff11134c950c4b1cb" },
        { "smart_ptr", "620620df3d232365bb75216250eafb37421ba7eb" },
        { "sort", "9ff0221684eba9f1a63d2e0c37269c9f4fa5aa70" },
        { "spirit", "a1ace8d8eb614785c6f619a46af13c1f032b8cd0" },
        { "stacktrace", "e75d2ff93bbe63ed3f1f578823cae6b8039da821" },
        { "statechart", "798d77087ecc144f36f8f5873dd1e9a4f4926344" },
        { "static_assert", "ba846a309164183757683b64df4b88f2d72cb0ff" },
        { "static_string", "6809e0a673a1d93e2d7239bec63f16bd5368aebc" },
        { "stl_interfaces", "6ab42d5111e6f3e6dc281c0a21d0683184850ccd" },
        { "system", "8220aa5700d0e23ffe88b3145f9269cc15b71ccc" },
        { "test", "65892881d7393ab3df2ba63813f8fc48b7983d2d" },
        { "thread", "96cd717b331e62095aa58d6768dab3baa03fcdce" },
        { "throw_exception", "da6f5420fe7e96f673ee185c2ff37cbaa3f95b84" },
        { "timer", "f345749e649d5c3b2719b46129b504934002d1fa" },
        { "tokenizer", "1c8a8bfd3a8fc2071458884da23ef5ac3a778a0c" },
        { "tti", "0fafbefdf9867f04a8dce3564a2271939fd48443" },
        { "tuple", "a4dff52f676679d07a7ed74621fd4c0a371c0e21" },
        { "type_erasure", "46a8945bfbc1b691112500b4e07865e9090b1274" },
        { "type_index", "9862011b6b5e900eee31d32684766c15d3506adf" },
        { "type_traits", "5588bb1ab7ba1809aaa492c4f6c6620dc2aaa927" },
        { "typeof", "27890f7ec1a1f25432474cb73abfc5bc4bafc0c7" },
        { "units", "57389b7374a6f7a4caf87cc44092bb8d0db65ec6" },
        { "unordered", "9abce00f240f4d7b38c335c1f86262e7ea16018d" },
        { "utility", "a4feaf4f243b9865dc8c65102232394d0c20473d" },
        { "uuid", "ca0185f9f2f1c8d58af8c54ce24579e1c6ebf348" },
        { "variant", "5e989572aea2c2d39c1268acb29558431deb8902" },
        { "variant2", "1ebc29aa0277cbb284a70327b5af14519f2702bc" },
        { "vmd", "d7ad0c5efe4715a89f296bae2f037fa3f68a7215" },
        { "wave", "a84bebde0b4b6f04d84d7d5696ece1c9177f598f" },
        { "winapi", "37607cc3d3ea5145381f65b6fe51eac8bb8f361f" },
        { "xpressive", "a79ffb49e84dc5083e31e2070c5effd3b7f5301a" },
        { "yap", "262624ac36652d1354e6b2ee38a79f2bc9a66cae" },
    };

    PackagePath p = "boost";
    p /= name;

    auto &t = s.addTarget<T>(p, "1.74.0");

    String url = p.back();
    if (url == "numeric")
        url = "numeric_conversion";

    auto ci = commits.find(p.back());
    if (ci == commits.end())
        throw SW_RUNTIME_ERROR("No commit for package: " + p.toString());

    t += Git("https://github.com/boostorg/" + url, "", "", ci->second);

    return t;
}

void addPrivateDefinitions(NativeExecutedTarget &t, const String &N)
{
    DefinitionsType defs;
    defs["BOOST_" + N + "_BUILDING_THE_LIB"];
    defs["BOOST_" + N + "_SOURCE"];
    t.Private += defs;
}

void addStaticDefinitions(NativeExecutedTarget &t, const String &N)
{
    DefinitionsType defs2;
    defs2["BOOST_" + N + "_BUILD_LIB"];
    t.Private += sw::Static, defs2;

    DefinitionsType defs;
    defs["BOOST_" + N + "_USE_LIB"];
    defs["BOOST_" + N + "_STATIC_LINK"];
    t.Public += sw::Static, defs;
}

void addSharedDefinitions(NativeExecutedTarget &t, const String &N)
{
    DefinitionsType defs2;
    defs2["BOOST_" + N + "_BUILD_DLL"];
    t.Private += sw::Shared, defs2;

    DefinitionsType defs;
    defs["BOOST_" + N + "_USE_DLL"];
    defs["BOOST_" + N + "_DYN_LINK"];
    t.Public += sw::Shared, defs;
}

auto &addCompiledBoostTarget(Solution &s, String name)
{
    auto &t = addBoostTarget<LibraryTarget>(s, name);
    if (name == "fiber")
        name += "s";
    auto N = boost::to_upper_copy(name);
    addPrivateDefinitions(t, N);
    addSharedDefinitions(t, N);
    addStaticDefinitions(t, N);
    return t;
}

auto &addStaticOnlyCompiledBoostTarget(Solution &s, const String &name)
{
    auto &t = addBoostTarget<StaticLibraryTarget>(s, name);
    auto N = boost::to_upper_copy(name);
    addPrivateDefinitions(t, N);
    addStaticDefinitions(t, N);
    return t;
}

void boost_deps(std::unordered_map<String, NativeExecutedTarget*> &boost_targets);

void build(Solution &s)
{
    std::unordered_map<String, NativeExecutedTarget*> boost_targets;

    auto header_only_target_names = {
        "accumulators",
        "algorithm",
        "align",
        "any",
        "array",
        "asio",
        "assert",
        "assign",
        "beast",
        "bimap",
        "bind",
        "callable_traits",
        "circular_buffer",
        "compatibility",
        "compute",
        "concept_check",
        "config",
        "container_hash",
        "conversion",
        "convert",
        "core",
        "coroutine2",
        "crc",
        "date_time",
        "detail",
        "dll",
        "dynamic_bitset",
        "endian",
        "flyweight",
        "foreach",
        "format",
        "function",
        "function_types",
        "functional",
        "fusion",
        "geometry",
        "gil",
        "hana",
        "heap",
        "histogram",
        "hof",
        "icl",
        "integer",
        "interprocess",
        "intrusive",
        "io",
        "iterator",
        "lambda",
        "lexical_cast",
        "local_function",
        "lockfree",
        "logic",
        "metaparse",
        "move",
        "mp11",
        "mpl",
        "msm",
        "multi_array",
        "multi_index",
        "multiprecision",
        "numeric",
        "interval",
        "odeint",
        "ublas",
        "optional",
        "outcome",
        "parameter",
        "phoenix",
        "poly_collection",
        "polygon",
        "pool",
        "predef",
        "preprocessor",
        "process",
        "property_map",
        "property_tree",
        "proto",
        "ptr_container",
        "qvm",
        "range",
        "ratio",
        "rational",
        "safe_numerics",
        "scope_exit",
        "signals2",
        "smart_ptr",
        "sort",
        "spirit",
        "stacktrace",
        "statechart",
        "static_assert",
        "static_string",
        "stl_interfaces",
        "system",
        "throw_exception",
        "tokenizer",
        "tti",
        "tuple",
        "type_index",
        "type_traits",
        "typeof",
        "units",
        "unordered",
        "utility",
        "uuid",
        "variant",
        "variant2",
        "vmd",
        "winapi",
        "xpressive",
        "yap",
    };
    for (auto &t : header_only_target_names)
    {
        auto &tgt = addBoostTarget<LibraryTarget>(s, t);
        tgt.HeaderOnly = true;
        boost_targets[t] = &tgt;
        tgt += "include/.*"_rr;
    }

    if (boost_targets["asio"]->getBuildSettings().TargetOS.Type == OSType::Windows)
        *boost_targets["asio"] += "Mswsock.lib"_slib;
    else
        *boost_targets["asio"] += "pthread"_slib;

    // some settings
    /**boost_targets["function"] += "include/.*\\.hpp"_rr;
    *boost_targets["pool"] += "include/.*\\.[ih]pp"_rr;
    *boost_targets["spirit"] += "include/.*\\.[cih]pp"_rr;*/

    if (boost_targets["config"]->getBuildSettings().TargetOS.Type == OSType::Windows)
    {
        boost_targets["config"]->Public.Definitions["BOOST_USE_WINDOWS_H"];
        boost_targets["config"]->Public.Definitions["BOOST_USE_INTRIN_H"];
    }

    if (boost_targets["dll"]->getBuildSettings().TargetOS.Type != OSType::Windows)
        *boost_targets["dll"] += "dl"_slib;

    // dll
    for (auto f : {
        "include/boost/dll/import_class.hpp",
        "include/boost/dll/import_mangled.hpp",
        "include/boost/dll/smart_library.hpp",
        })
    {
        boost_targets["dll"]->patch(f,
            "#if (__cplusplus < 201103L)",
            "#if !defined(_MSC_VER) && (__cplusplus < 201103L)");
    }

    // mpl
    for (auto f : {
        "include/boost/mpl/and.hpp",
        "include/boost/mpl/or.hpp",
        "include/boost/mpl/bitand.hpp",
        "include/boost/mpl/bitor.hpp",
        })
    {
        boost_targets["mpl"]->patch(f,
            "#if defined(_MSC_VER) && !defined(__clang__)",
            "#if defined(_MSC_VER)// && !defined(__clang__)");
    }

    if (boost_targets["stacktrace"]->getBuildSettings().TargetOS.Type != OSType::Windows)
        boost_targets["stacktrace"]->Public.Definitions["BOOST_STACKTRACE_GNU_SOURCE_NOT_REQUIRED"];
    else
    {
        *boost_targets["stacktrace"] += "dbgeng.lib"_slib;
        *boost_targets["stacktrace"] += "Ole32.lib"_slib;
    }

    // compiled
    auto compiled_target_names = {
        "atomic",
        "chrono",
        "container",
        "context",
        "contract",
        //"coroutine",
        //"exception", added below
        "fiber",
        "filesystem",
        "graph",
        //"graph_parallel",
        "iostreams",
        "locale",
        "log",
        "math",
        //"mpi",
        "nowide",
        "program_options",
        "python",
        "random",
        "regex",
        "serialization",
        "test",
        "thread",
        "timer",
        "type_erasure",
        "wave",
    };
    for (auto &t : compiled_target_names)
        boost_targets[t] = &addCompiledBoostTarget(s, t);
    boost_targets["exception"] = &addStaticOnlyCompiledBoostTarget(s, "exception");

    // some settings
    *boost_targets["container"] -= "src/dlmalloc.*\\.c"_rr;
    if (boost_targets["filesystem"]->getBuildSettings().TargetOS.Type == OSType::Windows)
        *boost_targets["filesystem"] += "advapi32.lib"_slib;
    *boost_targets["iostreams"] += "org.sw.demo.xz_utils.lzma-*"_dep;
    *boost_targets["iostreams"] += "org.sw.demo.bzip2-1"_dep;
    *boost_targets["iostreams"] += "org.sw.demo.madler.zlib-1"_dep;
    *boost_targets["iostreams"] += "org.sw.demo.facebook.zstd.zstd-1"_dep;
    if (boost_targets["random"]->getBuildSettings().TargetOS.Type == OSType::Windows)
        *boost_targets["random"] += "Advapi32.lib"_slib;

    boost_targets["math"]->Private.IncludeDirectories.insert(boost_targets["math"]->SourceDir / "src/tr1");
    boost_targets["math"]->Private.IncludeDirectories.insert(boost_targets["math"]->SourceDir / "src");
    boost_targets["math"]->Public.IncludeDirectories.insert(boost_targets["math"]->SourceDir / "include");
    boost_targets["math"]->Public += sw::Shared, "BOOST_MATH_TR1_DYN_LINK"_d;

    *boost_targets["locale"] -= "org.sw.demo.unicode.icu.i18n"_dep;
    if (boost_targets["locale"]->getBuildSettings().TargetOS.Type == OSType::Windows)
    {
        *boost_targets["locale"] -= "src/icu/.*"_rr;
        *boost_targets["locale"] -= "src/posix/.*"_rr;
        boost_targets["locale"]->Public.Definitions["BOOST_LOCALE_NO_POSIX_BACKEND"];
        boost_targets["locale"]->Definitions["NOMINMAX"];
    }
    else
    {
        *boost_targets["locale"] -= "src/win32/.*"_rr;
        boost_targets["locale"]->Public.Definitions["BOOST_LOCALE_NO_WINAPI_BACKEND"];
        boost_targets["locale"]->Public.Definitions["BOOST_LOCALE_WITH_ICONV"];
        *boost_targets["locale"] += "org.sw.demo.unicode.icu.i18n"_dep;
    }

    *boost_targets["log"] +=
        "include/.*"_rr,
        "src/.*\\.mc"_rr,
        "src/.*\\.hpp"_rr,
        "src/attribute_name.cpp",
        "src/attribute_set.cpp",
        "src/attribute_value_set.cpp",
        "src/code_conversion.cpp",
        "src/core.cpp",
        "src/date_time_format_parser.cpp",
        "src/default_attribute_names.cpp",
        "src/default_sink.cpp",
        "src/dump.cpp",
        "src/event.cpp",
        "src/exceptions.cpp",
        "src/format_parser.cpp",
        "src/global_logger_storage.cpp",
        "src/named_scope.cpp",
        "src/named_scope_format_parser.cpp",
        "src/once_block.cpp",
        "src/process_id.cpp",
        "src/process_name.cpp",
        "src/record_ostream.cpp",
        "src/severity_level.cpp",
        "src/spirit_encoding.cpp",
        "src/syslog_backend.cpp",
        "src/text_file_backend.cpp",
        "src/text_multifile_backend.cpp",
        "src/text_ostream_backend.cpp",
        "src/thread_id.cpp",
        "src/threadsafe_queue.cpp",
        "src/thread_specific.cpp",
        "src/timer.cpp",
        "src/timestamp.cpp",
        "src/trivial.cpp"
        ;
    boost_targets["log"]->Public.Definitions["BOOST_LOG_WITHOUT_EVENT_LOG"];
    boost_targets["log"]->Private += sw::Shared, "BOOST_LOG_DLL"_d;
    if (boost_targets["log"]->getBuildSettings().TargetOS.Type == OSType::Windows)
    {
        boost_targets["log"]->Public.Definitions["WIN32_LEAN_AND_MEAN"];
        boost_targets["log"]->Public.Definitions["NOMINMAX"];
        *boost_targets["log"] += "ws2_32.lib"_slib;
    }
    else
    {
        *boost_targets["log"] += "pthread"_slib;
    }

    //boost_targets["fiber"])->Shared.Private.Definitions["BOOST_FIBERS_SOURCE"];
    //boost_targets["fiber"]->Private.Definitions["BOOST_FIBERS_DYN_LINK"];

    //*boost_targets["python"] += "pvt.cppan.demo.python.libcompat";

    if (boost_targets["process"]->getBuildSettings().TargetOS.Type == OSType::Windows)
        *boost_targets["process"] += "Shell32.lib"_slib;

    if (boost_targets["regex"]->getBuildSettings().TargetOS.Type == OSType::Windows)
        *boost_targets["regex"] += "User32.lib"_slib;

    *boost_targets["test"] -= "src/test_main.cpp";

    *boost_targets["thread"] -= "src/pthread/once_atomic.cpp";
    if (boost_targets["thread"]->getBuildSettings().TargetOS.Type == OSType::Windows)
    {
        *boost_targets["thread"] -= "src/pthread/.*"_rr;
        *boost_targets["thread"] -= sw::Shared, "src/win32/tss_pe.cpp";
        *boost_targets["thread"] -= sw::Static, "src/win32/tss_dll.cpp";
    }
    else
    {
        *boost_targets["thread"] -= "src/win32/.*"_rr;
        (*boost_targets["thread"]).Public += "pthread"_slib; // some apis are in headers (cond_vars)
    }

    if (boost_targets["uuid"]->getBuildSettings().TargetOS.Type == OSType::Windows)
        *boost_targets["uuid"] += "Bcrypt.lib"_slib;

    // context, fiber
    *boost_targets["context"] += "include/.*"_rr;
    *boost_targets["context"] += "src/.*"_rr;
    *boost_targets["context"] -= "src/asm/.*"_rr;
    //*boost_targets["context"] -= "src/dummy.cpp";
    *boost_targets["context"] -= "src/untested.cpp";

    String a = ".*";
    if (boost_targets["context"]->getBuildSettings().TargetOS.is(ArchType::x86_64))
        a += "x86_64";
    else
        a += "i386";
    if (boost_targets["context"]->getBuildSettings().TargetOS.Type == OSType::Windows)
    {
        a += "_ms_pe_masm.asm";

        *boost_targets["context"] += sw::Shared, "BOOST_CONTEXT_EXPORT=EXPORT"_def;
        *boost_targets["context"] += sw::Static, "BOOST_CONTEXT_EXPORT="_def;
        boost_targets["context"]->Public += "BOOST_USE_WINFIB"_def;

        *boost_targets["context"] -= "src/posix/.*"_rr;
    }
    else if (boost_targets["context"]->getBuildSettings().TargetOS.isApple())
    {
        a += "_sysv_macho_gas.S";
    }
    else
    {
        a += "_sysv_elf_gas.S";

        *boost_targets["context"] -= "src/windows/.*"_rr;
        boost_targets["context"]->Public += "BOOST_USE_UCONTEXT"_def;
    }
    *boost_targets["context"] += FileRegex("src/asm/", a, false);

    *boost_targets["fiber"] += "include/.*"_rr;
    *boost_targets["fiber"] += "src/.*"_r;
    *boost_targets["fiber"] += "src/algo/.*"_rr;
    *boost_targets["fiber"] += "src/numa/algo/.*"_r;
    if (boost_targets["fiber"]->getBuildSettings().TargetOS.Type == OSType::Windows)
        *boost_targets["fiber"] += "src/numa/windows/.*"_rr;
    else if (boost_targets["fiber"]->getBuildSettings().TargetOS.Type == OSType::Linux)
        *boost_targets["fiber"] += "src/numa/linux/.*"_rr;
    else
        *boost_targets["fiber"] += "src/numa/.*"_r;
    if (boost_targets["fiber"]->getBuildSettings().TargetOS.Type != OSType::Windows)
        *boost_targets["fiber"] += "pthread"_slib;
    //

    //
    boost_targets["python"]->Public += "org.sw.demo.python.lib"_dep;
    *boost_targets["python"] -= "src/numpy/.*"_rr;
    //if (boost_targets["fiber"]->getBuildSettings().TargetOS.Type != OSType::Windows)
    //*boost_targets["python"] -= "src/.*"_rr;
    //

    boost_deps(boost_targets);

    //
    boost_targets["config"]->writeFileOnce(boost_targets["config"]->SourceDir / "include/boost/config/auto_link.hpp",
        R"(
#ifdef BOOST_LIB_PREFIX
#  undef BOOST_LIB_PREFIX
#endif
#if defined(BOOST_LIB_NAME)
#  undef BOOST_LIB_NAME
#endif
#if defined(BOOST_LIB_THREAD_OPT)
#  undef BOOST_LIB_THREAD_OPT
#endif
#if defined(BOOST_LIB_RT_OPT)
#  undef BOOST_LIB_RT_OPT
#endif
#if defined(BOOST_LIB_LINK_OPT)
#  undef BOOST_LIB_LINK_OPT
#endif
#if defined(BOOST_LIB_DEBUG_OPT)
#  undef BOOST_LIB_DEBUG_OPT
#endif
#if defined(BOOST_DYN_LINK)
#  undef BOOST_DYN_LINK
#endif
)"
);
}

void boost_deps(std::unordered_map<String, NativeExecutedTarget*> &boost_targets)
{
    auto add_public_dependency = [&boost_targets](const auto &n1, const auto &n2, bool idir_only = false)
    {
        auto t1 = boost_targets[n1];
        auto t2 = boost_targets[n2];
        if (!t1 || !t2)
            return;
        auto dt = t1->Public + *t2;
        dt->IncludeDirectoriesOnly = idir_only;
    };

    {
        add_public_dependency("accumulators", "algorithm", true);
        add_public_dependency("accumulators", "any", true);
        add_public_dependency("accumulators", "array", true);
        add_public_dependency("accumulators", "assert", true);
        add_public_dependency("accumulators", "atomic", true);
        add_public_dependency("accumulators", "bind", true);
        add_public_dependency("accumulators", "chrono", true);
        add_public_dependency("accumulators", "circular_buffer", true);
        add_public_dependency("accumulators", "compute", true);
        add_public_dependency("accumulators", "concept_check", true);
        add_public_dependency("accumulators", "config", true);
        add_public_dependency("accumulators", "container", true);
        add_public_dependency("accumulators", "container_hash", true);
        add_public_dependency("accumulators", "conversion", true);
        add_public_dependency("accumulators", "core", true);
        add_public_dependency("accumulators", "date_time", true);
        add_public_dependency("accumulators", "detail", true);
        add_public_dependency("accumulators", "dynamic_bitset", true);
        add_public_dependency("accumulators", "endian", true);
        add_public_dependency("accumulators", "exception", true);
        add_public_dependency("accumulators", "filesystem", true);
        add_public_dependency("accumulators", "foreach", true);
        add_public_dependency("accumulators", "format", true);
        add_public_dependency("accumulators", "function", true);
        add_public_dependency("accumulators", "function_types", true);
        add_public_dependency("accumulators", "fusion", true);
        add_public_dependency("accumulators", "integer", true);
        add_public_dependency("accumulators", "interval", true);
        add_public_dependency("accumulators", "intrusive", true);
        add_public_dependency("accumulators", "io", true);
        add_public_dependency("accumulators", "iterator", true);
        add_public_dependency("accumulators", "lambda", true);
        add_public_dependency("accumulators", "lexical_cast", true);
        add_public_dependency("accumulators", "logic", true);
        add_public_dependency("accumulators", "math", true);
        add_public_dependency("accumulators", "move", true);
        add_public_dependency("accumulators", "mp11", true);
        add_public_dependency("accumulators", "mpl", true);
        add_public_dependency("accumulators", "multi_index", true);
        add_public_dependency("accumulators", "multiprecision", true);
        add_public_dependency("accumulators", "numeric", true);
        add_public_dependency("accumulators", "optional", true);
        add_public_dependency("accumulators", "parameter", true);
        add_public_dependency("accumulators", "phoenix", true);
        add_public_dependency("accumulators", "pool", true);
        add_public_dependency("accumulators", "predef", true);
        add_public_dependency("accumulators", "preprocessor", true);
        add_public_dependency("accumulators", "property_tree", true);
        add_public_dependency("accumulators", "proto", true);
        add_public_dependency("accumulators", "random", true);
        add_public_dependency("accumulators", "range", true);
        add_public_dependency("accumulators", "ratio", true);
        add_public_dependency("accumulators", "rational", true);
        add_public_dependency("accumulators", "regex", true);
        add_public_dependency("accumulators", "serialization", true);
        add_public_dependency("accumulators", "smart_ptr", true);
        add_public_dependency("accumulators", "spirit", true);
        add_public_dependency("accumulators", "static_assert", true);
        add_public_dependency("accumulators", "system", true);
        add_public_dependency("accumulators", "thread", true);
        add_public_dependency("accumulators", "throw_exception", true);
        add_public_dependency("accumulators", "tokenizer", true);
        add_public_dependency("accumulators", "tti", true);
        add_public_dependency("accumulators", "tuple", true);
        add_public_dependency("accumulators", "type_index", true);
        add_public_dependency("accumulators", "type_traits", true);
        add_public_dependency("accumulators", "typeof", true);
        add_public_dependency("accumulators", "ublas", true);
        add_public_dependency("accumulators", "unordered", true);
        add_public_dependency("accumulators", "utility", true);
        add_public_dependency("accumulators", "uuid", true);
        add_public_dependency("accumulators", "variant", true);
        add_public_dependency("accumulators", "winapi", true);
        add_public_dependency("algorithm", "array", true);
        add_public_dependency("algorithm", "assert", true);
        add_public_dependency("algorithm", "bind", true);
        add_public_dependency("algorithm", "concept_check", true);
        add_public_dependency("algorithm", "config", true);
        add_public_dependency("algorithm", "container", true);
        add_public_dependency("algorithm", "container_hash", true);
        add_public_dependency("algorithm", "conversion", true);
        add_public_dependency("algorithm", "core", true);
        add_public_dependency("algorithm", "detail", true);
        add_public_dependency("algorithm", "exception", true);
        add_public_dependency("algorithm", "function", true);
        add_public_dependency("algorithm", "function_types", true);
        add_public_dependency("algorithm", "fusion", true);
        add_public_dependency("algorithm", "integer", true);
        add_public_dependency("algorithm", "intrusive", true);
        add_public_dependency("algorithm", "io", true);
        add_public_dependency("algorithm", "iterator", true);
        add_public_dependency("algorithm", "move", true);
        add_public_dependency("algorithm", "mpl", true);
        add_public_dependency("algorithm", "numeric", true);
        add_public_dependency("algorithm", "optional", true);
        add_public_dependency("algorithm", "predef", true);
        add_public_dependency("algorithm", "preprocessor", true);
        add_public_dependency("algorithm", "range", true);
        add_public_dependency("algorithm", "regex", true);
        add_public_dependency("algorithm", "smart_ptr", true);
        add_public_dependency("algorithm", "static_assert", true);
        add_public_dependency("algorithm", "throw_exception", true);
        add_public_dependency("algorithm", "tuple", true);
        add_public_dependency("algorithm", "type_index", true);
        add_public_dependency("algorithm", "type_traits", true);
        add_public_dependency("algorithm", "typeof", true);
        add_public_dependency("algorithm", "unordered", true);
        add_public_dependency("algorithm", "utility", true);
        add_public_dependency("align", "assert", true);
        add_public_dependency("align", "config", true);
        add_public_dependency("align", "core", true);
        add_public_dependency("align", "static_assert", true);
        add_public_dependency("any", "assert", true);
        add_public_dependency("any", "config", true);
        add_public_dependency("any", "container_hash", true);
        add_public_dependency("any", "core", true);
        add_public_dependency("any", "detail", true);
        add_public_dependency("any", "integer", true);
        add_public_dependency("any", "move", true);
        add_public_dependency("any", "preprocessor", true);
        add_public_dependency("any", "smart_ptr", true);
        add_public_dependency("any", "static_assert", true);
        add_public_dependency("any", "throw_exception", true);
        add_public_dependency("any", "type_index", true);
        add_public_dependency("any", "type_traits", true);
        add_public_dependency("array", "assert", true);
        add_public_dependency("array", "config", true);
        add_public_dependency("array", "core", true);
        add_public_dependency("array", "static_assert", true);
        add_public_dependency("array", "throw_exception", true);
        add_public_dependency("asio", "algorithm", true);
        add_public_dependency("asio", "array", true);
        add_public_dependency("asio", "assert", true);
        add_public_dependency("asio", "atomic", true);
        add_public_dependency("asio", "bind", true);
        add_public_dependency("asio", "chrono", true);
        add_public_dependency("asio", "concept_check", true);
        add_public_dependency("asio", "config", true);
        add_public_dependency("asio", "container", true);
        add_public_dependency("asio", "container_hash", true);
        add_public_dependency("asio", "context", true);
        add_public_dependency("asio", "conversion", true);
        add_public_dependency("asio", "core", true);
        add_public_dependency("asio", "coroutine", true);
        add_public_dependency("asio", "date_time", true);
        add_public_dependency("asio", "detail", true);
        add_public_dependency("asio", "exception", true);
        add_public_dependency("asio", "function", true);
        add_public_dependency("asio", "function_types", true);
        add_public_dependency("asio", "fusion", true);
        add_public_dependency("asio", "integer", true);
        add_public_dependency("asio", "intrusive", true);
        add_public_dependency("asio", "io", true);
        add_public_dependency("asio", "iterator", true);
        add_public_dependency("asio", "lambda", true);
        add_public_dependency("asio", "lexical_cast", true);
        add_public_dependency("asio", "math", true);
        add_public_dependency("asio", "move", true);
        add_public_dependency("asio", "mp11", true);
        add_public_dependency("asio", "mpl", true);
        add_public_dependency("asio", "numeric", true);
        add_public_dependency("asio", "optional", true);
        add_public_dependency("asio", "pool", true);
        add_public_dependency("asio", "predef", true);
        add_public_dependency("asio", "preprocessor", true);
        add_public_dependency("asio", "range", true);
        add_public_dependency("asio", "ratio", true);
        add_public_dependency("asio", "rational", true);
        add_public_dependency("asio", "regex", true);
        add_public_dependency("asio", "smart_ptr", true);
        add_public_dependency("asio", "static_assert", true);
        add_public_dependency("asio", "system", true);
        add_public_dependency("asio", "thread", true);
        add_public_dependency("asio", "throw_exception", true);
        add_public_dependency("asio", "tokenizer", true);
        add_public_dependency("asio", "tuple", true);
        add_public_dependency("asio", "type_index", true);
        add_public_dependency("asio", "type_traits", true);
        add_public_dependency("asio", "typeof", true);
        add_public_dependency("asio", "unordered", true);
        add_public_dependency("asio", "utility", true);
        add_public_dependency("asio", "winapi", true);
        add_public_dependency("assert", "config", true);
        add_public_dependency("assign", "algorithm", true);
        add_public_dependency("assign", "array", true);
        add_public_dependency("assign", "assert", true);
        add_public_dependency("assign", "atomic", true);
        add_public_dependency("assign", "bind", true);
        add_public_dependency("assign", "chrono", true);
        add_public_dependency("assign", "circular_buffer", true);
        add_public_dependency("assign", "concept_check", true);
        add_public_dependency("assign", "config", true);
        add_public_dependency("assign", "container", true);
        add_public_dependency("assign", "container_hash", true);
        add_public_dependency("assign", "conversion", true);
        add_public_dependency("assign", "core", true);
        add_public_dependency("assign", "date_time", true);
        add_public_dependency("assign", "detail", true);
        add_public_dependency("assign", "endian", true);
        add_public_dependency("assign", "exception", true);
        add_public_dependency("assign", "foreach", true);
        add_public_dependency("assign", "function", true);
        add_public_dependency("assign", "function_types", true);
        add_public_dependency("assign", "fusion", true);
        add_public_dependency("assign", "integer", true);
        add_public_dependency("assign", "intrusive", true);
        add_public_dependency("assign", "io", true);
        add_public_dependency("assign", "iterator", true);
        add_public_dependency("assign", "lambda", true);
        add_public_dependency("assign", "lexical_cast", true);
        add_public_dependency("assign", "math", true);
        add_public_dependency("assign", "move", true);
        add_public_dependency("assign", "mp11", true);
        add_public_dependency("assign", "mpl", true);
        add_public_dependency("assign", "numeric", true);
        add_public_dependency("assign", "optional", true);
        add_public_dependency("assign", "phoenix", true);
        add_public_dependency("assign", "pool", true);
        add_public_dependency("assign", "predef", true);
        add_public_dependency("assign", "preprocessor", true);
        add_public_dependency("assign", "proto", true);
        add_public_dependency("assign", "ptr_container", true);
        add_public_dependency("assign", "range", true);
        add_public_dependency("assign", "ratio", true);
        add_public_dependency("assign", "rational", true);
        add_public_dependency("assign", "regex", true);
        add_public_dependency("assign", "serialization", true);
        add_public_dependency("assign", "smart_ptr", true);
        add_public_dependency("assign", "spirit", true);
        add_public_dependency("assign", "static_assert", true);
        add_public_dependency("assign", "system", true);
        add_public_dependency("assign", "thread", true);
        add_public_dependency("assign", "throw_exception", true);
        add_public_dependency("assign", "tokenizer", true);
        add_public_dependency("assign", "tuple", true);
        add_public_dependency("assign", "type_index", true);
        add_public_dependency("assign", "type_traits", true);
        add_public_dependency("assign", "typeof", true);
        add_public_dependency("assign", "unordered", true);
        add_public_dependency("assign", "utility", true);
        add_public_dependency("assign", "variant", true);
        add_public_dependency("assign", "winapi", true);
        add_public_dependency("atomic", "assert", true);
        add_public_dependency("atomic", "config", true);
        add_public_dependency("atomic", "predef", true);
        add_public_dependency("atomic", "preprocessor", true);
        add_public_dependency("atomic", "static_assert", true);
        add_public_dependency("atomic", "type_traits", true);
        add_public_dependency("atomic", "winapi", true);
        add_public_dependency("beast", "algorithm", true);
        add_public_dependency("beast", "array", true);
        add_public_dependency("beast", "asio", true);
        add_public_dependency("beast", "assert", true);
        add_public_dependency("beast", "atomic", true);
        add_public_dependency("beast", "bind", true);
        add_public_dependency("beast", "chrono", true);
        add_public_dependency("beast", "concept_check", true);
        add_public_dependency("beast", "config", true);
        add_public_dependency("beast", "container", true);
        add_public_dependency("beast", "container_hash", true);
        add_public_dependency("beast", "context", true);
        add_public_dependency("beast", "conversion", true);
        add_public_dependency("beast", "core", true);
        add_public_dependency("beast", "coroutine", true);
        add_public_dependency("beast", "date_time", true);
        add_public_dependency("beast", "detail", true);
        add_public_dependency("beast", "endian", true);
        add_public_dependency("beast", "exception", true);
        add_public_dependency("beast", "function", true);
        add_public_dependency("beast", "function_types", true);
        add_public_dependency("beast", "fusion", true);
        add_public_dependency("beast", "integer", true);
        add_public_dependency("beast", "intrusive", true);
        add_public_dependency("beast", "io", true);
        add_public_dependency("beast", "iterator", true);
        add_public_dependency("beast", "lambda", true);
        add_public_dependency("beast", "lexical_cast", true);
        add_public_dependency("beast", "logic", true);
        add_public_dependency("beast", "math", true);
        add_public_dependency("beast", "move", true);
        add_public_dependency("beast", "mp11", true);
        add_public_dependency("beast", "mpl", true);
        add_public_dependency("beast", "numeric", true);
        add_public_dependency("beast", "optional", true);
        add_public_dependency("beast", "pool", true);
        add_public_dependency("beast", "predef", true);
        add_public_dependency("beast", "preprocessor", true);
        add_public_dependency("beast", "range", true);
        add_public_dependency("beast", "ratio", true);
        add_public_dependency("beast", "rational", true);
        add_public_dependency("beast", "regex", true);
        add_public_dependency("beast", "smart_ptr", true);
        add_public_dependency("beast", "static_assert", true);
        add_public_dependency("beast", "system", true);
        add_public_dependency("beast", "thread", true);
        add_public_dependency("beast", "throw_exception", true);
        add_public_dependency("beast", "tokenizer", true);
        add_public_dependency("beast", "tuple", true);
        add_public_dependency("beast", "type_index", true);
        add_public_dependency("beast", "type_traits", true);
        add_public_dependency("beast", "typeof", true);
        add_public_dependency("beast", "unordered", true);
        add_public_dependency("beast", "utility", true);
        add_public_dependency("beast", "winapi", true);
        add_public_dependency("bimap", "algorithm", true);
        add_public_dependency("bimap", "any", true);
        add_public_dependency("bimap", "array", true);
        add_public_dependency("bimap", "assert", true);
        add_public_dependency("bimap", "atomic", true);
        add_public_dependency("bimap", "bind", true);
        add_public_dependency("bimap", "chrono", true);
        add_public_dependency("bimap", "concept_check", true);
        add_public_dependency("bimap", "config", true);
        add_public_dependency("bimap", "container", true);
        add_public_dependency("bimap", "container_hash", true);
        add_public_dependency("bimap", "conversion", true);
        add_public_dependency("bimap", "core", true);
        add_public_dependency("bimap", "date_time", true);
        add_public_dependency("bimap", "detail", true);
        add_public_dependency("bimap", "dynamic_bitset", true);
        add_public_dependency("bimap", "endian", true);
        add_public_dependency("bimap", "exception", true);
        add_public_dependency("bimap", "foreach", true);
        add_public_dependency("bimap", "format", true);
        add_public_dependency("bimap", "function", true);
        add_public_dependency("bimap", "function_types", true);
        add_public_dependency("bimap", "fusion", true);
        add_public_dependency("bimap", "graph", true);
        add_public_dependency("bimap", "integer", true);
        add_public_dependency("bimap", "intrusive", true);
        add_public_dependency("bimap", "io", true);
        add_public_dependency("bimap", "iterator", true);
        add_public_dependency("bimap", "lambda", true);
        add_public_dependency("bimap", "lexical_cast", true);
        add_public_dependency("bimap", "math", true);
        add_public_dependency("bimap", "move", true);
        add_public_dependency("bimap", "mp11", true);
        add_public_dependency("bimap", "mpi", true);
        add_public_dependency("bimap", "mpl", true);
        add_public_dependency("bimap", "multi_index", true);
        add_public_dependency("bimap", "multiprecision", true);
        add_public_dependency("bimap", "numeric", true);
        add_public_dependency("bimap", "optional", true);
        add_public_dependency("bimap", "parameter", true);
        add_public_dependency("bimap", "phoenix", true);
        add_public_dependency("bimap", "pool", true);
        add_public_dependency("bimap", "predef", true);
        add_public_dependency("bimap", "preprocessor", true);
        add_public_dependency("bimap", "property_map", true);
        add_public_dependency("bimap", "property_tree", true);
        add_public_dependency("bimap", "proto", true);
        add_public_dependency("bimap", "python", true);
        add_public_dependency("bimap", "random", true);
        add_public_dependency("bimap", "range", true);
        add_public_dependency("bimap", "ratio", true);
        add_public_dependency("bimap", "rational", true);
        add_public_dependency("bimap", "regex", true);
        add_public_dependency("bimap", "serialization", true);
        add_public_dependency("bimap", "smart_ptr", true);
        add_public_dependency("bimap", "spirit", true);
        add_public_dependency("bimap", "static_assert", true);
        add_public_dependency("bimap", "system", true);
        add_public_dependency("bimap", "test", true);
        add_public_dependency("bimap", "thread", true);
        add_public_dependency("bimap", "throw_exception", true);
        add_public_dependency("bimap", "tokenizer", true);
        add_public_dependency("bimap", "tti", true);
        add_public_dependency("bimap", "tuple", true);
        add_public_dependency("bimap", "type_index", true);
        add_public_dependency("bimap", "type_traits", true);
        add_public_dependency("bimap", "typeof", true);
        add_public_dependency("bimap", "unordered", true);
        add_public_dependency("bimap", "utility", true);
        add_public_dependency("bimap", "variant", true);
        add_public_dependency("bimap", "winapi", true);
        add_public_dependency("bimap", "xpressive", true);
        add_public_dependency("bind", "assert", true);
        add_public_dependency("bind", "config", true);
        add_public_dependency("bind", "core", true);
        add_public_dependency("chrono", "assert", true);
        add_public_dependency("chrono", "config", true);
        add_public_dependency("chrono", "container_hash", true);
        add_public_dependency("chrono", "core", true);
        add_public_dependency("chrono", "detail", true);
        add_public_dependency("chrono", "integer", true);
        add_public_dependency("chrono", "io", true);
        add_public_dependency("chrono", "move", true);
        add_public_dependency("chrono", "mpl", true);
        add_public_dependency("chrono", "predef", true);
        add_public_dependency("chrono", "preprocessor", true);
        add_public_dependency("chrono", "ratio", true);
        add_public_dependency("chrono", "rational", true);
        add_public_dependency("chrono", "static_assert", true);
        add_public_dependency("chrono", "system", true);
        add_public_dependency("chrono", "throw_exception", true);
        add_public_dependency("chrono", "type_traits", true);
        add_public_dependency("chrono", "typeof", true);
        add_public_dependency("chrono", "utility", true);
        add_public_dependency("chrono", "winapi", true);
        add_public_dependency("circular_buffer", "assert", true);
        add_public_dependency("circular_buffer", "concept_check", true);
        add_public_dependency("circular_buffer", "config", true);
        add_public_dependency("circular_buffer", "core", true);
        add_public_dependency("circular_buffer", "move", true);
        add_public_dependency("circular_buffer", "preprocessor", true);
        add_public_dependency("circular_buffer", "static_assert", true);
        add_public_dependency("circular_buffer", "throw_exception", true);
        add_public_dependency("circular_buffer", "type_traits", true);
        add_public_dependency("compute", "algorithm", true);
        add_public_dependency("compute", "any", true);
        add_public_dependency("compute", "array", true);
        add_public_dependency("compute", "assert", true);
        add_public_dependency("compute", "atomic", true);
        add_public_dependency("compute", "bind", true);
        add_public_dependency("compute", "chrono", true);
        add_public_dependency("compute", "concept_check", true);
        add_public_dependency("compute", "config", true);
        add_public_dependency("compute", "container", true);
        add_public_dependency("compute", "container_hash", true);
        add_public_dependency("compute", "conversion", true);
        add_public_dependency("compute", "core", true);
        add_public_dependency("compute", "date_time", true);
        add_public_dependency("compute", "detail", true);
        add_public_dependency("compute", "dynamic_bitset", true);
        add_public_dependency("compute", "endian", true);
        add_public_dependency("compute", "exception", true);
        add_public_dependency("compute", "filesystem", true);
        add_public_dependency("compute", "foreach", true);
        add_public_dependency("compute", "format", true);
        add_public_dependency("compute", "function", true);
        add_public_dependency("compute", "function_types", true);
        add_public_dependency("compute", "fusion", true);
        add_public_dependency("compute", "integer", true);
        add_public_dependency("compute", "intrusive", true);
        add_public_dependency("compute", "io", true);
        add_public_dependency("compute", "iterator", true);
        add_public_dependency("compute", "lambda", true);
        add_public_dependency("compute", "lexical_cast", true);
        add_public_dependency("compute", "math", true);
        add_public_dependency("compute", "move", true);
        add_public_dependency("compute", "mp11", true);
        add_public_dependency("compute", "mpl", true);
        add_public_dependency("compute", "multi_index", true);
        add_public_dependency("compute", "multiprecision", true);
        add_public_dependency("compute", "numeric", true);
        add_public_dependency("compute", "optional", true);
        add_public_dependency("compute", "phoenix", true);
        add_public_dependency("compute", "pool", true);
        add_public_dependency("compute", "predef", true);
        add_public_dependency("compute", "preprocessor", true);
        add_public_dependency("compute", "property_tree", true);
        add_public_dependency("compute", "proto", true);
        add_public_dependency("compute", "random", true);
        add_public_dependency("compute", "range", true);
        add_public_dependency("compute", "ratio", true);
        add_public_dependency("compute", "rational", true);
        add_public_dependency("compute", "regex", true);
        add_public_dependency("compute", "serialization", true);
        add_public_dependency("compute", "smart_ptr", true);
        add_public_dependency("compute", "spirit", true);
        add_public_dependency("compute", "static_assert", true);
        add_public_dependency("compute", "system", true);
        add_public_dependency("compute", "thread", true);
        add_public_dependency("compute", "throw_exception", true);
        add_public_dependency("compute", "tokenizer", true);
        add_public_dependency("compute", "tti", true);
        add_public_dependency("compute", "tuple", true);
        add_public_dependency("compute", "type_index", true);
        add_public_dependency("compute", "type_traits", true);
        add_public_dependency("compute", "typeof", true);
        add_public_dependency("compute", "unordered", true);
        add_public_dependency("compute", "utility", true);
        add_public_dependency("compute", "uuid", true);
        add_public_dependency("compute", "variant", true);
        add_public_dependency("compute", "winapi", true);
        add_public_dependency("concept_check", "config", true);
        add_public_dependency("concept_check", "preprocessor", true);
        add_public_dependency("concept_check", "static_assert", true);
        add_public_dependency("concept_check", "type_traits", true);
        add_public_dependency("container", "assert", true);
        add_public_dependency("container", "config", true);
        add_public_dependency("container", "container_hash", true);
        add_public_dependency("container", "core", true);
        add_public_dependency("container", "detail", true);
        add_public_dependency("container", "integer", true);
        add_public_dependency("container", "intrusive", true);
        add_public_dependency("container", "move", true);
        add_public_dependency("container", "preprocessor", true);
        add_public_dependency("container", "static_assert", true);
        add_public_dependency("container", "throw_exception", true);
        add_public_dependency("container", "type_traits", true);
        add_public_dependency("container_hash", "assert", true);
        add_public_dependency("container_hash", "config", true);
        add_public_dependency("container_hash", "core", true);
        add_public_dependency("container_hash", "detail", true);
        add_public_dependency("container_hash", "integer", true);
        add_public_dependency("container_hash", "preprocessor", true);
        add_public_dependency("container_hash", "static_assert", true);
        add_public_dependency("container_hash", "throw_exception", true);
        add_public_dependency("container_hash", "type_traits", true);
        add_public_dependency("context", "thread", false);
        add_public_dependency("context", "algorithm", true);
        add_public_dependency("context", "array", true);
        add_public_dependency("context", "assert", true);
        add_public_dependency("context", "atomic", true);
        add_public_dependency("context", "bind", true);
        add_public_dependency("context", "chrono", true);
        add_public_dependency("context", "concept_check", true);
        add_public_dependency("context", "config", true);
        add_public_dependency("context", "container", true);
        add_public_dependency("context", "container_hash", true);
        add_public_dependency("context", "conversion", true);
        add_public_dependency("context", "core", true);
        add_public_dependency("context", "date_time", true);
        add_public_dependency("context", "detail", true);
        add_public_dependency("context", "exception", true);
        add_public_dependency("context", "function", true);
        add_public_dependency("context", "function_types", true);
        add_public_dependency("context", "fusion", true);
        add_public_dependency("context", "integer", true);
        add_public_dependency("context", "intrusive", true);
        add_public_dependency("context", "io", true);
        add_public_dependency("context", "iterator", true);
        add_public_dependency("context", "lambda", true);
        add_public_dependency("context", "lexical_cast", true);
        add_public_dependency("context", "math", true);
        add_public_dependency("context", "move", true);
        add_public_dependency("context", "mp11", true);
        add_public_dependency("context", "mpl", true);
        add_public_dependency("context", "numeric", true);
        add_public_dependency("context", "optional", true);
        add_public_dependency("context", "pool", true);
        add_public_dependency("context", "predef", true);
        add_public_dependency("context", "preprocessor", true);
        add_public_dependency("context", "range", true);
        add_public_dependency("context", "ratio", true);
        add_public_dependency("context", "rational", true);
        add_public_dependency("context", "regex", true);
        add_public_dependency("context", "smart_ptr", true);
        add_public_dependency("context", "static_assert", true);
        add_public_dependency("context", "system", true);
        add_public_dependency("context", "throw_exception", true);
        add_public_dependency("context", "tokenizer", true);
        add_public_dependency("context", "tuple", true);
        add_public_dependency("context", "type_index", true);
        add_public_dependency("context", "type_traits", true);
        add_public_dependency("context", "typeof", true);
        add_public_dependency("context", "unordered", true);
        add_public_dependency("context", "utility", true);
        add_public_dependency("context", "winapi", true);
        add_public_dependency("contract", "algorithm", true);
        add_public_dependency("contract", "any", true);
        add_public_dependency("contract", "array", true);
        add_public_dependency("contract", "assert", true);
        add_public_dependency("contract", "atomic", true);
        add_public_dependency("contract", "bind", true);
        add_public_dependency("contract", "chrono", true);
        add_public_dependency("contract", "concept_check", true);
        add_public_dependency("contract", "config", true);
        add_public_dependency("contract", "container", true);
        add_public_dependency("contract", "container_hash", true);
        add_public_dependency("contract", "conversion", true);
        add_public_dependency("contract", "core", true);
        add_public_dependency("contract", "date_time", true);
        add_public_dependency("contract", "detail", true);
        add_public_dependency("contract", "exception", true);
        add_public_dependency("contract", "function", true);
        add_public_dependency("contract", "function_types", true);
        add_public_dependency("contract", "fusion", true);
        add_public_dependency("contract", "integer", true);
        add_public_dependency("contract", "intrusive", true);
        add_public_dependency("contract", "io", true);
        add_public_dependency("contract", "iterator", true);
        add_public_dependency("contract", "lambda", true);
        add_public_dependency("contract", "lexical_cast", true);
        add_public_dependency("contract", "math", true);
        add_public_dependency("contract", "move", true);
        add_public_dependency("contract", "mp11", true);
        add_public_dependency("contract", "mpl", true);
        add_public_dependency("contract", "numeric", true);
        add_public_dependency("contract", "optional", true);
        add_public_dependency("contract", "predef", true);
        add_public_dependency("contract", "preprocessor", true);
        add_public_dependency("contract", "range", true);
        add_public_dependency("contract", "ratio", true);
        add_public_dependency("contract", "rational", true);
        add_public_dependency("contract", "regex", true);
        add_public_dependency("contract", "smart_ptr", true);
        add_public_dependency("contract", "static_assert", true);
        add_public_dependency("contract", "system", true);
        add_public_dependency("contract", "thread", true);
        add_public_dependency("contract", "throw_exception", true);
        add_public_dependency("contract", "tokenizer", true);
        add_public_dependency("contract", "tuple", true);
        add_public_dependency("contract", "type_index", true);
        add_public_dependency("contract", "type_traits", true);
        add_public_dependency("contract", "typeof", true);
        add_public_dependency("contract", "unordered", true);
        add_public_dependency("contract", "utility", true);
        add_public_dependency("contract", "winapi", true);
        add_public_dependency("conversion", "assert", true);
        add_public_dependency("conversion", "config", true);
        add_public_dependency("conversion", "core", true);
        add_public_dependency("conversion", "move", true);
        add_public_dependency("conversion", "preprocessor", true);
        add_public_dependency("conversion", "smart_ptr", true);
        add_public_dependency("conversion", "static_assert", true);
        add_public_dependency("conversion", "throw_exception", true);
        add_public_dependency("conversion", "type_traits", true);
        add_public_dependency("conversion", "typeof", true);
        add_public_dependency("convert", "algorithm", true);
        add_public_dependency("convert", "array", true);
        add_public_dependency("convert", "assert", true);
        add_public_dependency("convert", "atomic", true);
        add_public_dependency("convert", "bind", true);
        add_public_dependency("convert", "chrono", true);
        add_public_dependency("convert", "concept_check", true);
        add_public_dependency("convert", "config", true);
        add_public_dependency("convert", "container", true);
        add_public_dependency("convert", "container_hash", true);
        add_public_dependency("convert", "conversion", true);
        add_public_dependency("convert", "core", true);
        add_public_dependency("convert", "date_time", true);
        add_public_dependency("convert", "detail", true);
        add_public_dependency("convert", "endian", true);
        add_public_dependency("convert", "exception", true);
        add_public_dependency("convert", "foreach", true);
        add_public_dependency("convert", "function", true);
        add_public_dependency("convert", "function_types", true);
        add_public_dependency("convert", "fusion", true);
        add_public_dependency("convert", "integer", true);
        add_public_dependency("convert", "intrusive", true);
        add_public_dependency("convert", "io", true);
        add_public_dependency("convert", "iterator", true);
        add_public_dependency("convert", "lambda", true);
        add_public_dependency("convert", "lexical_cast", true);
        add_public_dependency("convert", "math", true);
        add_public_dependency("convert", "move", true);
        add_public_dependency("convert", "mp11", true);
        add_public_dependency("convert", "mpl", true);
        add_public_dependency("convert", "numeric", true);
        add_public_dependency("convert", "optional", true);
        add_public_dependency("convert", "parameter", true);
        add_public_dependency("convert", "phoenix", true);
        add_public_dependency("convert", "pool", true);
        add_public_dependency("convert", "predef", true);
        add_public_dependency("convert", "preprocessor", true);
        add_public_dependency("convert", "proto", true);
        add_public_dependency("convert", "range", true);
        add_public_dependency("convert", "ratio", true);
        add_public_dependency("convert", "rational", true);
        add_public_dependency("convert", "regex", true);
        add_public_dependency("convert", "smart_ptr", true);
        add_public_dependency("convert", "spirit", true);
        add_public_dependency("convert", "static_assert", true);
        add_public_dependency("convert", "system", true);
        add_public_dependency("convert", "thread", true);
        add_public_dependency("convert", "throw_exception", true);
        add_public_dependency("convert", "tokenizer", true);
        add_public_dependency("convert", "tuple", true);
        add_public_dependency("convert", "type_index", true);
        add_public_dependency("convert", "type_traits", true);
        add_public_dependency("convert", "typeof", true);
        add_public_dependency("convert", "unordered", true);
        add_public_dependency("convert", "utility", true);
        add_public_dependency("convert", "variant", true);
        add_public_dependency("convert", "winapi", true);
        add_public_dependency("core", "assert", true);
        add_public_dependency("core", "config", true);
        add_public_dependency("coroutine", "context", false);
        add_public_dependency("coroutine", "thread", false);
        add_public_dependency("coroutine", "algorithm", true);
        add_public_dependency("coroutine", "array", true);
        add_public_dependency("coroutine", "assert", true);
        add_public_dependency("coroutine", "atomic", true);
        add_public_dependency("coroutine", "bind", true);
        add_public_dependency("coroutine", "chrono", true);
        add_public_dependency("coroutine", "concept_check", true);
        add_public_dependency("coroutine", "config", true);
        add_public_dependency("coroutine", "container", true);
        add_public_dependency("coroutine", "container_hash", true);
        add_public_dependency("coroutine", "conversion", true);
        add_public_dependency("coroutine", "core", true);
        add_public_dependency("coroutine", "date_time", true);
        add_public_dependency("coroutine", "detail", true);
        add_public_dependency("coroutine", "exception", true);
        add_public_dependency("coroutine", "function", true);
        add_public_dependency("coroutine", "function_types", true);
        add_public_dependency("coroutine", "fusion", true);
        add_public_dependency("coroutine", "integer", true);
        add_public_dependency("coroutine", "intrusive", true);
        add_public_dependency("coroutine", "io", true);
        add_public_dependency("coroutine", "iterator", true);
        add_public_dependency("coroutine", "lambda", true);
        add_public_dependency("coroutine", "lexical_cast", true);
        add_public_dependency("coroutine", "math", true);
        add_public_dependency("coroutine", "move", true);
        add_public_dependency("coroutine", "mp11", true);
        add_public_dependency("coroutine", "mpl", true);
        add_public_dependency("coroutine", "numeric", true);
        add_public_dependency("coroutine", "optional", true);
        add_public_dependency("coroutine", "pool", true);
        add_public_dependency("coroutine", "predef", true);
        add_public_dependency("coroutine", "preprocessor", true);
        add_public_dependency("coroutine", "range", true);
        add_public_dependency("coroutine", "ratio", true);
        add_public_dependency("coroutine", "rational", true);
        add_public_dependency("coroutine", "regex", true);
        add_public_dependency("coroutine", "smart_ptr", true);
        add_public_dependency("coroutine", "static_assert", true);
        add_public_dependency("coroutine", "system", true);
        add_public_dependency("coroutine", "throw_exception", true);
        add_public_dependency("coroutine", "tokenizer", true);
        add_public_dependency("coroutine", "tuple", true);
        add_public_dependency("coroutine", "type_index", true);
        add_public_dependency("coroutine", "type_traits", true);
        add_public_dependency("coroutine", "typeof", true);
        add_public_dependency("coroutine", "unordered", true);
        add_public_dependency("coroutine", "utility", true);
        add_public_dependency("coroutine", "winapi", true);
        add_public_dependency("coroutine2", "algorithm", true);
        add_public_dependency("coroutine2", "array", true);
        add_public_dependency("coroutine2", "assert", true);
        add_public_dependency("coroutine2", "atomic", true);
        add_public_dependency("coroutine2", "bind", true);
        add_public_dependency("coroutine2", "chrono", true);
        add_public_dependency("coroutine2", "concept_check", true);
        add_public_dependency("coroutine2", "config", true);
        add_public_dependency("coroutine2", "container", true);
        add_public_dependency("coroutine2", "container_hash", true);
        add_public_dependency("coroutine2", "context", true);
        add_public_dependency("coroutine2", "conversion", true);
        add_public_dependency("coroutine2", "core", true);
        add_public_dependency("coroutine2", "date_time", true);
        add_public_dependency("coroutine2", "detail", true);
        add_public_dependency("coroutine2", "exception", true);
        add_public_dependency("coroutine2", "function", true);
        add_public_dependency("coroutine2", "function_types", true);
        add_public_dependency("coroutine2", "fusion", true);
        add_public_dependency("coroutine2", "integer", true);
        add_public_dependency("coroutine2", "intrusive", true);
        add_public_dependency("coroutine2", "io", true);
        add_public_dependency("coroutine2", "iterator", true);
        add_public_dependency("coroutine2", "lambda", true);
        add_public_dependency("coroutine2", "lexical_cast", true);
        add_public_dependency("coroutine2", "math", true);
        add_public_dependency("coroutine2", "move", true);
        add_public_dependency("coroutine2", "mp11", true);
        add_public_dependency("coroutine2", "mpl", true);
        add_public_dependency("coroutine2", "numeric", true);
        add_public_dependency("coroutine2", "optional", true);
        add_public_dependency("coroutine2", "pool", true);
        add_public_dependency("coroutine2", "predef", true);
        add_public_dependency("coroutine2", "preprocessor", true);
        add_public_dependency("coroutine2", "range", true);
        add_public_dependency("coroutine2", "ratio", true);
        add_public_dependency("coroutine2", "rational", true);
        add_public_dependency("coroutine2", "regex", true);
        add_public_dependency("coroutine2", "smart_ptr", true);
        add_public_dependency("coroutine2", "static_assert", true);
        add_public_dependency("coroutine2", "system", true);
        add_public_dependency("coroutine2", "thread", true);
        add_public_dependency("coroutine2", "throw_exception", true);
        add_public_dependency("coroutine2", "tokenizer", true);
        add_public_dependency("coroutine2", "tuple", true);
        add_public_dependency("coroutine2", "type_index", true);
        add_public_dependency("coroutine2", "type_traits", true);
        add_public_dependency("coroutine2", "typeof", true);
        add_public_dependency("coroutine2", "unordered", true);
        add_public_dependency("coroutine2", "utility", true);
        add_public_dependency("coroutine2", "winapi", true);
        add_public_dependency("crc", "array", true);
        add_public_dependency("crc", "assert", true);
        add_public_dependency("crc", "config", true);
        add_public_dependency("crc", "core", true);
        add_public_dependency("crc", "integer", true);
        add_public_dependency("crc", "preprocessor", true);
        add_public_dependency("crc", "static_assert", true);
        add_public_dependency("crc", "throw_exception", true);
        add_public_dependency("crc", "type_traits", true);
        add_public_dependency("date_time", "algorithm", true);
        add_public_dependency("date_time", "array", true);
        add_public_dependency("date_time", "assert", true);
        add_public_dependency("date_time", "atomic", true);
        add_public_dependency("date_time", "bind", true);
        add_public_dependency("date_time", "concept_check", true);
        add_public_dependency("date_time", "config", true);
        add_public_dependency("date_time", "container", true);
        add_public_dependency("date_time", "container_hash", true);
        add_public_dependency("date_time", "conversion", true);
        add_public_dependency("date_time", "core", true);
        add_public_dependency("date_time", "detail", true);
        add_public_dependency("date_time", "exception", true);
        add_public_dependency("date_time", "function", true);
        add_public_dependency("date_time", "function_types", true);
        add_public_dependency("date_time", "fusion", true);
        add_public_dependency("date_time", "integer", true);
        add_public_dependency("date_time", "intrusive", true);
        add_public_dependency("date_time", "io", true);
        add_public_dependency("date_time", "iterator", true);
        add_public_dependency("date_time", "lambda", true);
        add_public_dependency("date_time", "lexical_cast", true);
        add_public_dependency("date_time", "math", true);
        add_public_dependency("date_time", "move", true);
        add_public_dependency("date_time", "mp11", true);
        add_public_dependency("date_time", "mpl", true);
        add_public_dependency("date_time", "numeric", true);
        add_public_dependency("date_time", "optional", true);
        add_public_dependency("date_time", "predef", true);
        add_public_dependency("date_time", "preprocessor", true);
        add_public_dependency("date_time", "range", true);
        add_public_dependency("date_time", "regex", true);
        add_public_dependency("date_time", "smart_ptr", true);
        add_public_dependency("date_time", "static_assert", true);
        add_public_dependency("date_time", "throw_exception", true);
        add_public_dependency("date_time", "tokenizer", true);
        add_public_dependency("date_time", "tuple", true);
        add_public_dependency("date_time", "type_index", true);
        add_public_dependency("date_time", "type_traits", true);
        add_public_dependency("date_time", "typeof", true);
        add_public_dependency("date_time", "unordered", true);
        add_public_dependency("date_time", "utility", true);
        add_public_dependency("date_time", "winapi", true);
        add_public_dependency("detail", "assert", true);
        add_public_dependency("detail", "config", true);
        add_public_dependency("detail", "core", true);
        add_public_dependency("detail", "preprocessor", true);
        add_public_dependency("detail", "static_assert", true);
        add_public_dependency("detail", "type_traits", true);
        add_public_dependency("dll", "algorithm", true);
        add_public_dependency("dll", "array", true);
        add_public_dependency("dll", "assert", true);
        add_public_dependency("dll", "atomic", true);
        add_public_dependency("dll", "bind", true);
        add_public_dependency("dll", "chrono", true);
        add_public_dependency("dll", "concept_check", true);
        add_public_dependency("dll", "config", true);
        add_public_dependency("dll", "container", true);
        add_public_dependency("dll", "container_hash", true);
        add_public_dependency("dll", "conversion", true);
        add_public_dependency("dll", "core", true);
        add_public_dependency("dll", "date_time", true);
        add_public_dependency("dll", "detail", true);
        add_public_dependency("dll", "endian", true);
        add_public_dependency("dll", "exception", true);
        add_public_dependency("dll", "filesystem", true);
        add_public_dependency("dll", "foreach", true);
        add_public_dependency("dll", "function", true);
        add_public_dependency("dll", "function_types", true);
        add_public_dependency("dll", "fusion", true);
        add_public_dependency("dll", "integer", true);
        add_public_dependency("dll", "intrusive", true);
        add_public_dependency("dll", "io", true);
        add_public_dependency("dll", "iterator", true);
        add_public_dependency("dll", "lambda", true);
        add_public_dependency("dll", "lexical_cast", true);
        add_public_dependency("dll", "math", true);
        add_public_dependency("dll", "move", true);
        add_public_dependency("dll", "mp11", true);
        add_public_dependency("dll", "mpl", true);
        add_public_dependency("dll", "numeric", true);
        add_public_dependency("dll", "optional", true);
        add_public_dependency("dll", "phoenix", true);
        add_public_dependency("dll", "pool", true);
        add_public_dependency("dll", "predef", true);
        add_public_dependency("dll", "preprocessor", true);
        add_public_dependency("dll", "proto", true);
        add_public_dependency("dll", "range", true);
        add_public_dependency("dll", "ratio", true);
        add_public_dependency("dll", "rational", true);
        add_public_dependency("dll", "regex", true);
        add_public_dependency("dll", "smart_ptr", true);
        add_public_dependency("dll", "spirit", true);
        add_public_dependency("dll", "static_assert", true);
        add_public_dependency("dll", "system", true);
        add_public_dependency("dll", "thread", true);
        add_public_dependency("dll", "throw_exception", true);
        add_public_dependency("dll", "tokenizer", true);
        add_public_dependency("dll", "tuple", true);
        add_public_dependency("dll", "type_index", true);
        add_public_dependency("dll", "type_traits", true);
        add_public_dependency("dll", "typeof", true);
        add_public_dependency("dll", "unordered", true);
        add_public_dependency("dll", "utility", true);
        add_public_dependency("dll", "variant", true);
        add_public_dependency("dll", "winapi", true);
        add_public_dependency("dynamic_bitset", "assert", true);
        add_public_dependency("dynamic_bitset", "config", true);
        add_public_dependency("dynamic_bitset", "container_hash", true);
        add_public_dependency("dynamic_bitset", "core", true);
        add_public_dependency("dynamic_bitset", "detail", true);
        add_public_dependency("dynamic_bitset", "integer", true);
        add_public_dependency("dynamic_bitset", "move", true);
        add_public_dependency("dynamic_bitset", "preprocessor", true);
        add_public_dependency("dynamic_bitset", "static_assert", true);
        add_public_dependency("dynamic_bitset", "throw_exception", true);
        add_public_dependency("dynamic_bitset", "type_traits", true);
        add_public_dependency("endian", "assert", true);
        add_public_dependency("endian", "config", true);
        add_public_dependency("endian", "core", true);
        add_public_dependency("endian", "preprocessor", true);
        add_public_dependency("endian", "static_assert", true);
        add_public_dependency("endian", "type_traits", true);
        add_public_dependency("exception", "assert", true);
        add_public_dependency("exception", "config", true);
        add_public_dependency("exception", "core", true);
        add_public_dependency("exception", "move", true);
        add_public_dependency("exception", "preprocessor", true);
        add_public_dependency("exception", "smart_ptr", true);
        add_public_dependency("exception", "static_assert", true);
        add_public_dependency("exception", "throw_exception", true);
        add_public_dependency("exception", "tuple", true);
        add_public_dependency("exception", "type_traits", true);
        add_public_dependency("fiber", "context", false);
        add_public_dependency("fiber", "filesystem", false);
        add_public_dependency("fiber", "algorithm", true);
        add_public_dependency("fiber", "array", true);
        add_public_dependency("fiber", "assert", true);
        add_public_dependency("fiber", "atomic", true);
        add_public_dependency("fiber", "bind", true);
        add_public_dependency("fiber", "chrono", true);
        add_public_dependency("fiber", "concept_check", true);
        add_public_dependency("fiber", "config", true);
        add_public_dependency("fiber", "container", true);
        add_public_dependency("fiber", "container_hash", true);
        add_public_dependency("fiber", "conversion", true);
        add_public_dependency("fiber", "core", true);
        add_public_dependency("fiber", "date_time", true);
        add_public_dependency("fiber", "detail", true);
        add_public_dependency("fiber", "exception", true);
        add_public_dependency("fiber", "format", true);
        add_public_dependency("fiber", "function", true);
        add_public_dependency("fiber", "function_types", true);
        add_public_dependency("fiber", "fusion", true);
        add_public_dependency("fiber", "integer", true);
        add_public_dependency("fiber", "intrusive", true);
        add_public_dependency("fiber", "io", true);
        add_public_dependency("fiber", "iterator", true);
        add_public_dependency("fiber", "lambda", true);
        add_public_dependency("fiber", "lexical_cast", true);
        add_public_dependency("fiber", "math", true);
        add_public_dependency("fiber", "move", true);
        add_public_dependency("fiber", "mp11", true);
        add_public_dependency("fiber", "mpl", true);
        add_public_dependency("fiber", "numeric", true);
        add_public_dependency("fiber", "optional", true);
        add_public_dependency("fiber", "pool", true);
        add_public_dependency("fiber", "predef", true);
        add_public_dependency("fiber", "preprocessor", true);
        add_public_dependency("fiber", "range", true);
        add_public_dependency("fiber", "ratio", true);
        add_public_dependency("fiber", "rational", true);
        add_public_dependency("fiber", "regex", true);
        add_public_dependency("fiber", "smart_ptr", true);
        add_public_dependency("fiber", "static_assert", true);
        add_public_dependency("fiber", "system", true);
        add_public_dependency("fiber", "thread", true);
        add_public_dependency("fiber", "throw_exception", true);
        add_public_dependency("fiber", "tokenizer", true);
        add_public_dependency("fiber", "tuple", true);
        add_public_dependency("fiber", "type_index", true);
        add_public_dependency("fiber", "type_traits", true);
        add_public_dependency("fiber", "typeof", true);
        add_public_dependency("fiber", "unordered", true);
        add_public_dependency("fiber", "utility", true);
        add_public_dependency("fiber", "winapi", true);
        add_public_dependency("filesystem", "assert", true);
        add_public_dependency("filesystem", "concept_check", true);
        add_public_dependency("filesystem", "config", true);
        add_public_dependency("filesystem", "container_hash", true);
        add_public_dependency("filesystem", "conversion", true);
        add_public_dependency("filesystem", "core", true);
        add_public_dependency("filesystem", "detail", true);
        add_public_dependency("filesystem", "function_types", true);
        add_public_dependency("filesystem", "fusion", true);
        add_public_dependency("filesystem", "integer", true);
        add_public_dependency("filesystem", "io", true);
        add_public_dependency("filesystem", "iterator", true);
        add_public_dependency("filesystem", "move", true);
        add_public_dependency("filesystem", "mpl", true);
        add_public_dependency("filesystem", "optional", true);
        add_public_dependency("filesystem", "predef", true);
        add_public_dependency("filesystem", "preprocessor", true);
        add_public_dependency("filesystem", "smart_ptr", true);
        add_public_dependency("filesystem", "static_assert", true);
        add_public_dependency("filesystem", "system", true);
        add_public_dependency("filesystem", "throw_exception", true);
        add_public_dependency("filesystem", "tuple", true);
        add_public_dependency("filesystem", "type_traits", true);
        add_public_dependency("filesystem", "typeof", true);
        add_public_dependency("filesystem", "utility", true);
        add_public_dependency("filesystem", "winapi", true);
        add_public_dependency("flyweight", "algorithm", true);
        add_public_dependency("flyweight", "array", true);
        add_public_dependency("flyweight", "assert", true);
        add_public_dependency("flyweight", "atomic", true);
        add_public_dependency("flyweight", "bind", true);
        add_public_dependency("flyweight", "chrono", true);
        add_public_dependency("flyweight", "concept_check", true);
        add_public_dependency("flyweight", "config", true);
        add_public_dependency("flyweight", "container", true);
        add_public_dependency("flyweight", "container_hash", true);
        add_public_dependency("flyweight", "conversion", true);
        add_public_dependency("flyweight", "core", true);
        add_public_dependency("flyweight", "date_time", true);
        add_public_dependency("flyweight", "detail", true);
        add_public_dependency("flyweight", "endian", true);
        add_public_dependency("flyweight", "exception", true);
        add_public_dependency("flyweight", "foreach", true);
        add_public_dependency("flyweight", "function", true);
        add_public_dependency("flyweight", "function_types", true);
        add_public_dependency("flyweight", "fusion", true);
        add_public_dependency("flyweight", "integer", true);
        add_public_dependency("flyweight", "interprocess", true);
        add_public_dependency("flyweight", "intrusive", true);
        add_public_dependency("flyweight", "io", true);
        add_public_dependency("flyweight", "iterator", true);
        add_public_dependency("flyweight", "lambda", true);
        add_public_dependency("flyweight", "lexical_cast", true);
        add_public_dependency("flyweight", "math", true);
        add_public_dependency("flyweight", "move", true);
        add_public_dependency("flyweight", "mp11", true);
        add_public_dependency("flyweight", "mpl", true);
        add_public_dependency("flyweight", "multi_index", true);
        add_public_dependency("flyweight", "numeric", true);
        add_public_dependency("flyweight", "optional", true);
        add_public_dependency("flyweight", "parameter", true);
        add_public_dependency("flyweight", "phoenix", true);
        add_public_dependency("flyweight", "pool", true);
        add_public_dependency("flyweight", "predef", true);
        add_public_dependency("flyweight", "preprocessor", true);
        add_public_dependency("flyweight", "proto", true);
        add_public_dependency("flyweight", "range", true);
        add_public_dependency("flyweight", "ratio", true);
        add_public_dependency("flyweight", "rational", true);
        add_public_dependency("flyweight", "regex", true);
        add_public_dependency("flyweight", "serialization", true);
        add_public_dependency("flyweight", "smart_ptr", true);
        add_public_dependency("flyweight", "spirit", true);
        add_public_dependency("flyweight", "static_assert", true);
        add_public_dependency("flyweight", "system", true);
        add_public_dependency("flyweight", "thread", true);
        add_public_dependency("flyweight", "throw_exception", true);
        add_public_dependency("flyweight", "tokenizer", true);
        add_public_dependency("flyweight", "tuple", true);
        add_public_dependency("flyweight", "type_index", true);
        add_public_dependency("flyweight", "type_traits", true);
        add_public_dependency("flyweight", "typeof", true);
        add_public_dependency("flyweight", "unordered", true);
        add_public_dependency("flyweight", "utility", true);
        add_public_dependency("flyweight", "variant", true);
        add_public_dependency("flyweight", "winapi", true);
        add_public_dependency("foreach", "array", true);
        add_public_dependency("foreach", "assert", true);
        add_public_dependency("foreach", "concept_check", true);
        add_public_dependency("foreach", "config", true);
        add_public_dependency("foreach", "container_hash", true);
        add_public_dependency("foreach", "conversion", true);
        add_public_dependency("foreach", "core", true);
        add_public_dependency("foreach", "detail", true);
        add_public_dependency("foreach", "function_types", true);
        add_public_dependency("foreach", "fusion", true);
        add_public_dependency("foreach", "integer", true);
        add_public_dependency("foreach", "io", true);
        add_public_dependency("foreach", "iterator", true);
        add_public_dependency("foreach", "move", true);
        add_public_dependency("foreach", "mpl", true);
        add_public_dependency("foreach", "numeric", true);
        add_public_dependency("foreach", "optional", true);
        add_public_dependency("foreach", "predef", true);
        add_public_dependency("foreach", "preprocessor", true);
        add_public_dependency("foreach", "range", true);
        add_public_dependency("foreach", "regex", true);
        add_public_dependency("foreach", "smart_ptr", true);
        add_public_dependency("foreach", "static_assert", true);
        add_public_dependency("foreach", "throw_exception", true);
        add_public_dependency("foreach", "tuple", true);
        add_public_dependency("foreach", "type_traits", true);
        add_public_dependency("foreach", "typeof", true);
        add_public_dependency("foreach", "utility", true);
        add_public_dependency("format", "assert", true);
        add_public_dependency("format", "config", true);
        add_public_dependency("format", "container_hash", true);
        add_public_dependency("format", "core", true);
        add_public_dependency("format", "detail", true);
        add_public_dependency("format", "integer", true);
        add_public_dependency("format", "io", true);
        add_public_dependency("format", "move", true);
        add_public_dependency("format", "optional", true);
        add_public_dependency("format", "predef", true);
        add_public_dependency("format", "preprocessor", true);
        add_public_dependency("format", "smart_ptr", true);
        add_public_dependency("format", "static_assert", true);
        add_public_dependency("format", "throw_exception", true);
        add_public_dependency("format", "type_traits", true);
        add_public_dependency("format", "utility", true);
        add_public_dependency("function", "assert", true);
        add_public_dependency("function", "bind", true);
        add_public_dependency("function", "config", true);
        add_public_dependency("function", "container_hash", true);
        add_public_dependency("function", "core", true);
        add_public_dependency("function", "detail", true);
        add_public_dependency("function", "integer", true);
        add_public_dependency("function", "move", true);
        add_public_dependency("function", "preprocessor", true);
        add_public_dependency("function", "smart_ptr", true);
        add_public_dependency("function", "static_assert", true);
        add_public_dependency("function", "throw_exception", true);
        add_public_dependency("function", "type_index", true);
        add_public_dependency("function", "type_traits", true);
        add_public_dependency("function", "typeof", true);
        add_public_dependency("function_types", "assert", true);
        add_public_dependency("function_types", "config", true);
        add_public_dependency("function_types", "container_hash", true);
        add_public_dependency("function_types", "core", true);
        add_public_dependency("function_types", "detail", true);
        add_public_dependency("function_types", "integer", true);
        add_public_dependency("function_types", "io", true);
        add_public_dependency("function_types", "mpl", true);
        add_public_dependency("function_types", "predef", true);
        add_public_dependency("function_types", "preprocessor", true);
        add_public_dependency("function_types", "static_assert", true);
        add_public_dependency("function_types", "throw_exception", true);
        add_public_dependency("function_types", "type_traits", true);
        add_public_dependency("function_types", "utility", true);
        add_public_dependency("functional", "assert", true);
        add_public_dependency("functional", "bind", true);
        add_public_dependency("functional", "config", true);
        add_public_dependency("functional", "container_hash", true);
        add_public_dependency("functional", "core", true);
        add_public_dependency("functional", "detail", true);
        add_public_dependency("functional", "function", true);
        add_public_dependency("functional", "function_types", true);
        add_public_dependency("functional", "integer", true);
        add_public_dependency("functional", "io", true);
        add_public_dependency("functional", "move", true);
        add_public_dependency("functional", "mpl", true);
        add_public_dependency("functional", "predef", true);
        add_public_dependency("functional", "preprocessor", true);
        add_public_dependency("functional", "smart_ptr", true);
        add_public_dependency("functional", "static_assert", true);
        add_public_dependency("functional", "throw_exception", true);
        add_public_dependency("functional", "type_index", true);
        add_public_dependency("functional", "type_traits", true);
        add_public_dependency("functional", "typeof", true);
        add_public_dependency("functional", "utility", true);
        add_public_dependency("fusion", "assert", true);
        add_public_dependency("fusion", "config", true);
        add_public_dependency("fusion", "container_hash", true);
        add_public_dependency("fusion", "core", true);
        add_public_dependency("fusion", "detail", true);
        add_public_dependency("fusion", "function_types", true);
        add_public_dependency("fusion", "integer", true);
        add_public_dependency("fusion", "io", true);
        add_public_dependency("fusion", "mpl", true);
        add_public_dependency("fusion", "predef", true);
        add_public_dependency("fusion", "preprocessor", true);
        add_public_dependency("fusion", "static_assert", true);
        add_public_dependency("fusion", "throw_exception", true);
        add_public_dependency("fusion", "tuple", true);
        add_public_dependency("fusion", "type_traits", true);
        add_public_dependency("fusion", "typeof", true);
        add_public_dependency("fusion", "utility", true);
        add_public_dependency("geometry", "algorithm", true);
        add_public_dependency("geometry", "array", true);
        add_public_dependency("geometry", "assert", true);
        add_public_dependency("geometry", "atomic", true);
        add_public_dependency("geometry", "bind", true);
        add_public_dependency("geometry", "chrono", true);
        add_public_dependency("geometry", "concept_check", true);
        add_public_dependency("geometry", "config", true);
        add_public_dependency("geometry", "container", true);
        add_public_dependency("geometry", "container_hash", true);
        add_public_dependency("geometry", "conversion", true);
        add_public_dependency("geometry", "core", true);
        add_public_dependency("geometry", "date_time", true);
        add_public_dependency("geometry", "detail", true);
        add_public_dependency("geometry", "dynamic_bitset", true);
        add_public_dependency("geometry", "endian", true);
        add_public_dependency("geometry", "exception", true);
        add_public_dependency("geometry", "foreach", true);
        add_public_dependency("geometry", "function", true);
        add_public_dependency("geometry", "function_types", true);
        add_public_dependency("geometry", "fusion", true);
        add_public_dependency("geometry", "integer", true);
        add_public_dependency("geometry", "intrusive", true);
        add_public_dependency("geometry", "io", true);
        add_public_dependency("geometry", "iterator", true);
        add_public_dependency("geometry", "lambda", true);
        add_public_dependency("geometry", "lexical_cast", true);
        add_public_dependency("geometry", "math", true);
        add_public_dependency("geometry", "move", true);
        add_public_dependency("geometry", "mp11", true);
        add_public_dependency("geometry", "mpl", true);
        add_public_dependency("geometry", "multiprecision", true);
        add_public_dependency("geometry", "numeric", true);
        add_public_dependency("geometry", "optional", true);
        add_public_dependency("geometry", "phoenix", true);
        add_public_dependency("geometry", "polygon", true);
        add_public_dependency("geometry", "pool", true);
        add_public_dependency("geometry", "predef", true);
        add_public_dependency("geometry", "preprocessor", true);
        add_public_dependency("geometry", "proto", true);
        add_public_dependency("geometry", "qvm", true);
        add_public_dependency("geometry", "random", true);
        add_public_dependency("geometry", "range", true);
        add_public_dependency("geometry", "ratio", true);
        add_public_dependency("geometry", "rational", true);
        add_public_dependency("geometry", "regex", true);
        add_public_dependency("geometry", "serialization", true);
        add_public_dependency("geometry", "smart_ptr", true);
        add_public_dependency("geometry", "spirit", true);
        add_public_dependency("geometry", "static_assert", true);
        add_public_dependency("geometry", "system", true);
        add_public_dependency("geometry", "thread", true);
        add_public_dependency("geometry", "throw_exception", true);
        add_public_dependency("geometry", "tokenizer", true);
        add_public_dependency("geometry", "tuple", true);
        add_public_dependency("geometry", "type_index", true);
        add_public_dependency("geometry", "type_traits", true);
        add_public_dependency("geometry", "typeof", true);
        add_public_dependency("geometry", "unordered", true);
        add_public_dependency("geometry", "utility", true);
        add_public_dependency("geometry", "variant", true);
        add_public_dependency("geometry", "winapi", true);
        add_public_dependency("gil", "assert", true);
        add_public_dependency("gil", "concept_check", true);
        add_public_dependency("gil", "config", true);
        add_public_dependency("gil", "container_hash", true);
        add_public_dependency("gil", "conversion", true);
        add_public_dependency("gil", "core", true);
        add_public_dependency("gil", "detail", true);
        add_public_dependency("gil", "filesystem", true);
        add_public_dependency("gil", "function_types", true);
        add_public_dependency("gil", "fusion", true);
        add_public_dependency("gil", "integer", true);
        add_public_dependency("gil", "io", true);
        add_public_dependency("gil", "iterator", true);
        add_public_dependency("gil", "move", true);
        add_public_dependency("gil", "mp11", true);
        add_public_dependency("gil", "mpl", true);
        add_public_dependency("gil", "numeric", true);
        add_public_dependency("gil", "optional", true);
        add_public_dependency("gil", "predef", true);
        add_public_dependency("gil", "preprocessor", true);
        add_public_dependency("gil", "smart_ptr", true);
        add_public_dependency("gil", "static_assert", true);
        add_public_dependency("gil", "system", true);
        add_public_dependency("gil", "throw_exception", true);
        add_public_dependency("gil", "tuple", true);
        add_public_dependency("gil", "type_traits", true);
        add_public_dependency("gil", "typeof", true);
        add_public_dependency("gil", "utility", true);
        add_public_dependency("gil", "variant2", true);
        add_public_dependency("gil", "winapi", true);
        add_public_dependency("graph", "regex", false);
        add_public_dependency("graph", "algorithm", true);
        add_public_dependency("graph", "any", true);
        add_public_dependency("graph", "array", true);
        add_public_dependency("graph", "assert", true);
        add_public_dependency("graph", "atomic", true);
        add_public_dependency("graph", "bimap", true);
        add_public_dependency("graph", "bind", true);
        add_public_dependency("graph", "chrono", true);
        add_public_dependency("graph", "concept_check", true);
        add_public_dependency("graph", "config", true);
        add_public_dependency("graph", "container", true);
        add_public_dependency("graph", "container_hash", true);
        add_public_dependency("graph", "conversion", true);
        add_public_dependency("graph", "core", true);
        add_public_dependency("graph", "date_time", true);
        add_public_dependency("graph", "detail", true);
        add_public_dependency("graph", "dynamic_bitset", true);
        add_public_dependency("graph", "endian", true);
        add_public_dependency("graph", "exception", true);
        add_public_dependency("graph", "foreach", true);
        add_public_dependency("graph", "format", true);
        add_public_dependency("graph", "function", true);
        add_public_dependency("graph", "function_types", true);
        add_public_dependency("graph", "fusion", true);
        add_public_dependency("graph", "integer", true);
        add_public_dependency("graph", "intrusive", true);
        add_public_dependency("graph", "io", true);
        add_public_dependency("graph", "iterator", true);
        add_public_dependency("graph", "lambda", true);
        add_public_dependency("graph", "lexical_cast", true);
        add_public_dependency("graph", "math", true);
        add_public_dependency("graph", "move", true);
        add_public_dependency("graph", "mp11", true);
        add_public_dependency("graph", "mpi", true);
        add_public_dependency("graph", "mpl", true);
        add_public_dependency("graph", "multi_index", true);
        add_public_dependency("graph", "multiprecision", true);
        add_public_dependency("graph", "numeric", true);
        add_public_dependency("graph", "optional", true);
        add_public_dependency("graph", "parameter", true);
        add_public_dependency("graph", "phoenix", true);
        add_public_dependency("graph", "pool", true);
        add_public_dependency("graph", "predef", true);
        add_public_dependency("graph", "preprocessor", true);
        add_public_dependency("graph", "property_map", true);
        add_public_dependency("graph", "property_tree", true);
        add_public_dependency("graph", "proto", true);
        add_public_dependency("graph", "python", true);
        add_public_dependency("graph", "random", true);
        add_public_dependency("graph", "range", true);
        add_public_dependency("graph", "ratio", true);
        add_public_dependency("graph", "rational", true);
        add_public_dependency("graph", "serialization", true);
        add_public_dependency("graph", "smart_ptr", true);
        add_public_dependency("graph", "spirit", true);
        add_public_dependency("graph", "static_assert", true);
        add_public_dependency("graph", "system", true);
        add_public_dependency("graph", "test", true);
        add_public_dependency("graph", "thread", true);
        add_public_dependency("graph", "throw_exception", true);
        add_public_dependency("graph", "tokenizer", true);
        add_public_dependency("graph", "tti", true);
        add_public_dependency("graph", "tuple", true);
        add_public_dependency("graph", "type_index", true);
        add_public_dependency("graph", "type_traits", true);
        add_public_dependency("graph", "typeof", true);
        add_public_dependency("graph", "unordered", true);
        add_public_dependency("graph", "utility", true);
        add_public_dependency("graph", "variant", true);
        add_public_dependency("graph", "winapi", true);
        add_public_dependency("graph", "xpressive", true);
        add_public_dependency("graph_parallel", "mpi", false);
        add_public_dependency("graph_parallel", "algorithm", true);
        add_public_dependency("graph_parallel", "any", true);
        add_public_dependency("graph_parallel", "array", true);
        add_public_dependency("graph_parallel", "assert", true);
        add_public_dependency("graph_parallel", "atomic", true);
        add_public_dependency("graph_parallel", "bimap", true);
        add_public_dependency("graph_parallel", "bind", true);
        add_public_dependency("graph_parallel", "chrono", true);
        add_public_dependency("graph_parallel", "concept_check", true);
        add_public_dependency("graph_parallel", "config", true);
        add_public_dependency("graph_parallel", "container", true);
        add_public_dependency("graph_parallel", "container_hash", true);
        add_public_dependency("graph_parallel", "conversion", true);
        add_public_dependency("graph_parallel", "core", true);
        add_public_dependency("graph_parallel", "date_time", true);
        add_public_dependency("graph_parallel", "detail", true);
        add_public_dependency("graph_parallel", "dynamic_bitset", true);
        add_public_dependency("graph_parallel", "endian", true);
        add_public_dependency("graph_parallel", "exception", true);
        add_public_dependency("graph_parallel", "filesystem", true);
        add_public_dependency("graph_parallel", "foreach", true);
        add_public_dependency("graph_parallel", "format", true);
        add_public_dependency("graph_parallel", "function", true);
        add_public_dependency("graph_parallel", "function_types", true);
        add_public_dependency("graph_parallel", "fusion", true);
        add_public_dependency("graph_parallel", "graph", true);
        add_public_dependency("graph_parallel", "integer", true);
        add_public_dependency("graph_parallel", "intrusive", true);
        add_public_dependency("graph_parallel", "io", true);
        add_public_dependency("graph_parallel", "iterator", true);
        add_public_dependency("graph_parallel", "lambda", true);
        add_public_dependency("graph_parallel", "lexical_cast", true);
        add_public_dependency("graph_parallel", "math", true);
        add_public_dependency("graph_parallel", "move", true);
        add_public_dependency("graph_parallel", "mp11", true);
        add_public_dependency("graph_parallel", "mpl", true);
        add_public_dependency("graph_parallel", "multi_index", true);
        add_public_dependency("graph_parallel", "multiprecision", true);
        add_public_dependency("graph_parallel", "numeric", true);
        add_public_dependency("graph_parallel", "optional", true);
        add_public_dependency("graph_parallel", "parameter", true);
        add_public_dependency("graph_parallel", "phoenix", true);
        add_public_dependency("graph_parallel", "pool", true);
        add_public_dependency("graph_parallel", "predef", true);
        add_public_dependency("graph_parallel", "preprocessor", true);
        add_public_dependency("graph_parallel", "property_map", true);
        add_public_dependency("graph_parallel", "property_tree", true);
        add_public_dependency("graph_parallel", "proto", true);
        add_public_dependency("graph_parallel", "python", true);
        add_public_dependency("graph_parallel", "random", true);
        add_public_dependency("graph_parallel", "range", true);
        add_public_dependency("graph_parallel", "ratio", true);
        add_public_dependency("graph_parallel", "rational", true);
        add_public_dependency("graph_parallel", "regex", true);
        add_public_dependency("graph_parallel", "serialization", true);
        add_public_dependency("graph_parallel", "smart_ptr", true);
        add_public_dependency("graph_parallel", "spirit", true);
        add_public_dependency("graph_parallel", "static_assert", true);
        add_public_dependency("graph_parallel", "system", true);
        add_public_dependency("graph_parallel", "test", true);
        add_public_dependency("graph_parallel", "thread", true);
        add_public_dependency("graph_parallel", "throw_exception", true);
        add_public_dependency("graph_parallel", "tokenizer", true);
        add_public_dependency("graph_parallel", "tti", true);
        add_public_dependency("graph_parallel", "tuple", true);
        add_public_dependency("graph_parallel", "type_index", true);
        add_public_dependency("graph_parallel", "type_traits", true);
        add_public_dependency("graph_parallel", "typeof", true);
        add_public_dependency("graph_parallel", "unordered", true);
        add_public_dependency("graph_parallel", "utility", true);
        add_public_dependency("graph_parallel", "variant", true);
        add_public_dependency("graph_parallel", "winapi", true);
        add_public_dependency("graph_parallel", "xpressive", true);
        add_public_dependency("hana", "assert", true);
        add_public_dependency("hana", "config", true);
        add_public_dependency("hana", "container_hash", true);
        add_public_dependency("hana", "core", true);
        add_public_dependency("hana", "detail", true);
        add_public_dependency("hana", "function_types", true);
        add_public_dependency("hana", "fusion", true);
        add_public_dependency("hana", "integer", true);
        add_public_dependency("hana", "io", true);
        add_public_dependency("hana", "mpl", true);
        add_public_dependency("hana", "predef", true);
        add_public_dependency("hana", "preprocessor", true);
        add_public_dependency("hana", "static_assert", true);
        add_public_dependency("hana", "throw_exception", true);
        add_public_dependency("hana", "tuple", true);
        add_public_dependency("hana", "type_traits", true);
        add_public_dependency("hana", "typeof", true);
        add_public_dependency("hana", "utility", true);
        add_public_dependency("heap", "array", true);
        add_public_dependency("heap", "assert", true);
        add_public_dependency("heap", "bind", true);
        add_public_dependency("heap", "concept_check", true);
        add_public_dependency("heap", "config", true);
        add_public_dependency("heap", "container_hash", true);
        add_public_dependency("heap", "conversion", true);
        add_public_dependency("heap", "core", true);
        add_public_dependency("heap", "detail", true);
        add_public_dependency("heap", "function", true);
        add_public_dependency("heap", "function_types", true);
        add_public_dependency("heap", "fusion", true);
        add_public_dependency("heap", "integer", true);
        add_public_dependency("heap", "intrusive", true);
        add_public_dependency("heap", "io", true);
        add_public_dependency("heap", "iterator", true);
        add_public_dependency("heap", "move", true);
        add_public_dependency("heap", "mp11", true);
        add_public_dependency("heap", "mpl", true);
        add_public_dependency("heap", "optional", true);
        add_public_dependency("heap", "parameter", true);
        add_public_dependency("heap", "predef", true);
        add_public_dependency("heap", "preprocessor", true);
        add_public_dependency("heap", "smart_ptr", true);
        add_public_dependency("heap", "static_assert", true);
        add_public_dependency("heap", "throw_exception", true);
        add_public_dependency("heap", "tuple", true);
        add_public_dependency("heap", "type_index", true);
        add_public_dependency("heap", "type_traits", true);
        add_public_dependency("heap", "typeof", true);
        add_public_dependency("heap", "utility", true);
        add_public_dependency("histogram", "algorithm", true);
        add_public_dependency("histogram", "array", true);
        add_public_dependency("histogram", "assert", true);
        add_public_dependency("histogram", "atomic", true);
        add_public_dependency("histogram", "bind", true);
        add_public_dependency("histogram", "chrono", true);
        add_public_dependency("histogram", "concept_check", true);
        add_public_dependency("histogram", "config", true);
        add_public_dependency("histogram", "container", true);
        add_public_dependency("histogram", "container_hash", true);
        add_public_dependency("histogram", "conversion", true);
        add_public_dependency("histogram", "core", true);
        add_public_dependency("histogram", "date_time", true);
        add_public_dependency("histogram", "detail", true);
        add_public_dependency("histogram", "endian", true);
        add_public_dependency("histogram", "exception", true);
        add_public_dependency("histogram", "foreach", true);
        add_public_dependency("histogram", "function", true);
        add_public_dependency("histogram", "function_types", true);
        add_public_dependency("histogram", "fusion", true);
        add_public_dependency("histogram", "integer", true);
        add_public_dependency("histogram", "intrusive", true);
        add_public_dependency("histogram", "io", true);
        add_public_dependency("histogram", "iterator", true);
        add_public_dependency("histogram", "lambda", true);
        add_public_dependency("histogram", "lexical_cast", true);
        add_public_dependency("histogram", "math", true);
        add_public_dependency("histogram", "move", true);
        add_public_dependency("histogram", "mp11", true);
        add_public_dependency("histogram", "mpl", true);
        add_public_dependency("histogram", "numeric", true);
        add_public_dependency("histogram", "optional", true);
        add_public_dependency("histogram", "phoenix", true);
        add_public_dependency("histogram", "pool", true);
        add_public_dependency("histogram", "predef", true);
        add_public_dependency("histogram", "preprocessor", true);
        add_public_dependency("histogram", "proto", true);
        add_public_dependency("histogram", "range", true);
        add_public_dependency("histogram", "ratio", true);
        add_public_dependency("histogram", "rational", true);
        add_public_dependency("histogram", "regex", true);
        add_public_dependency("histogram", "serialization", true);
        add_public_dependency("histogram", "smart_ptr", true);
        add_public_dependency("histogram", "spirit", true);
        add_public_dependency("histogram", "static_assert", true);
        add_public_dependency("histogram", "system", true);
        add_public_dependency("histogram", "thread", true);
        add_public_dependency("histogram", "throw_exception", true);
        add_public_dependency("histogram", "tokenizer", true);
        add_public_dependency("histogram", "tuple", true);
        add_public_dependency("histogram", "type_index", true);
        add_public_dependency("histogram", "type_traits", true);
        add_public_dependency("histogram", "typeof", true);
        add_public_dependency("histogram", "unordered", true);
        add_public_dependency("histogram", "utility", true);
        add_public_dependency("histogram", "variant", true);
        add_public_dependency("histogram", "variant2", true);
        add_public_dependency("histogram", "winapi", true);
        add_public_dependency("icl", "algorithm", true);
        add_public_dependency("icl", "array", true);
        add_public_dependency("icl", "assert", true);
        add_public_dependency("icl", "atomic", true);
        add_public_dependency("icl", "bind", true);
        add_public_dependency("icl", "concept_check", true);
        add_public_dependency("icl", "config", true);
        add_public_dependency("icl", "container", true);
        add_public_dependency("icl", "container_hash", true);
        add_public_dependency("icl", "conversion", true);
        add_public_dependency("icl", "core", true);
        add_public_dependency("icl", "date_time", true);
        add_public_dependency("icl", "detail", true);
        add_public_dependency("icl", "exception", true);
        add_public_dependency("icl", "function", true);
        add_public_dependency("icl", "function_types", true);
        add_public_dependency("icl", "fusion", true);
        add_public_dependency("icl", "integer", true);
        add_public_dependency("icl", "intrusive", true);
        add_public_dependency("icl", "io", true);
        add_public_dependency("icl", "iterator", true);
        add_public_dependency("icl", "lambda", true);
        add_public_dependency("icl", "lexical_cast", true);
        add_public_dependency("icl", "math", true);
        add_public_dependency("icl", "move", true);
        add_public_dependency("icl", "mp11", true);
        add_public_dependency("icl", "mpl", true);
        add_public_dependency("icl", "numeric", true);
        add_public_dependency("icl", "optional", true);
        add_public_dependency("icl", "predef", true);
        add_public_dependency("icl", "preprocessor", true);
        add_public_dependency("icl", "range", true);
        add_public_dependency("icl", "rational", true);
        add_public_dependency("icl", "regex", true);
        add_public_dependency("icl", "smart_ptr", true);
        add_public_dependency("icl", "static_assert", true);
        add_public_dependency("icl", "throw_exception", true);
        add_public_dependency("icl", "tokenizer", true);
        add_public_dependency("icl", "tuple", true);
        add_public_dependency("icl", "type_index", true);
        add_public_dependency("icl", "type_traits", true);
        add_public_dependency("icl", "typeof", true);
        add_public_dependency("icl", "unordered", true);
        add_public_dependency("icl", "utility", true);
        add_public_dependency("icl", "winapi", true);
        add_public_dependency("integer", "assert", true);
        add_public_dependency("integer", "config", true);
        add_public_dependency("integer", "core", true);
        add_public_dependency("integer", "static_assert", true);
        add_public_dependency("integer", "throw_exception", true);
        add_public_dependency("interprocess", "algorithm", true);
        add_public_dependency("interprocess", "array", true);
        add_public_dependency("interprocess", "assert", true);
        add_public_dependency("interprocess", "atomic", true);
        add_public_dependency("interprocess", "bind", true);
        add_public_dependency("interprocess", "concept_check", true);
        add_public_dependency("interprocess", "config", true);
        add_public_dependency("interprocess", "container", true);
        add_public_dependency("interprocess", "container_hash", true);
        add_public_dependency("interprocess", "conversion", true);
        add_public_dependency("interprocess", "core", true);
        add_public_dependency("interprocess", "date_time", true);
        add_public_dependency("interprocess", "detail", true);
        add_public_dependency("interprocess", "exception", true);
        add_public_dependency("interprocess", "function", true);
        add_public_dependency("interprocess", "function_types", true);
        add_public_dependency("interprocess", "fusion", true);
        add_public_dependency("interprocess", "integer", true);
        add_public_dependency("interprocess", "intrusive", true);
        add_public_dependency("interprocess", "io", true);
        add_public_dependency("interprocess", "iterator", true);
        add_public_dependency("interprocess", "lambda", true);
        add_public_dependency("interprocess", "lexical_cast", true);
        add_public_dependency("interprocess", "math", true);
        add_public_dependency("interprocess", "move", true);
        add_public_dependency("interprocess", "mp11", true);
        add_public_dependency("interprocess", "mpl", true);
        add_public_dependency("interprocess", "numeric", true);
        add_public_dependency("interprocess", "optional", true);
        add_public_dependency("interprocess", "predef", true);
        add_public_dependency("interprocess", "preprocessor", true);
        add_public_dependency("interprocess", "range", true);
        add_public_dependency("interprocess", "regex", true);
        add_public_dependency("interprocess", "smart_ptr", true);
        add_public_dependency("interprocess", "static_assert", true);
        add_public_dependency("interprocess", "throw_exception", true);
        add_public_dependency("interprocess", "tokenizer", true);
        add_public_dependency("interprocess", "tuple", true);
        add_public_dependency("interprocess", "type_index", true);
        add_public_dependency("interprocess", "type_traits", true);
        add_public_dependency("interprocess", "typeof", true);
        add_public_dependency("interprocess", "unordered", true);
        add_public_dependency("interprocess", "utility", true);
        add_public_dependency("interprocess", "winapi", true);
        add_public_dependency("intrusive", "assert", true);
        add_public_dependency("intrusive", "config", true);
        add_public_dependency("intrusive", "container_hash", true);
        add_public_dependency("intrusive", "core", true);
        add_public_dependency("intrusive", "detail", true);
        add_public_dependency("intrusive", "integer", true);
        add_public_dependency("intrusive", "move", true);
        add_public_dependency("intrusive", "preprocessor", true);
        add_public_dependency("intrusive", "static_assert", true);
        add_public_dependency("intrusive", "throw_exception", true);
        add_public_dependency("intrusive", "type_traits", true);
        add_public_dependency("io", "config", true);
        add_public_dependency("iostreams", "array", true);
        add_public_dependency("iostreams", "assert", true);
        add_public_dependency("iostreams", "atomic", true);
        add_public_dependency("iostreams", "bind", true);
        add_public_dependency("iostreams", "concept_check", true);
        add_public_dependency("iostreams", "config", true);
        add_public_dependency("iostreams", "container", true);
        add_public_dependency("iostreams", "container_hash", true);
        add_public_dependency("iostreams", "conversion", true);
        add_public_dependency("iostreams", "core", true);
        add_public_dependency("iostreams", "detail", true);
        add_public_dependency("iostreams", "dynamic_bitset", true);
        add_public_dependency("iostreams", "function", true);
        add_public_dependency("iostreams", "function_types", true);
        add_public_dependency("iostreams", "fusion", true);
        add_public_dependency("iostreams", "integer", true);
        add_public_dependency("iostreams", "intrusive", true);
        add_public_dependency("iostreams", "io", true);
        add_public_dependency("iostreams", "iterator", true);
        add_public_dependency("iostreams", "lambda", true);
        add_public_dependency("iostreams", "lexical_cast", true);
        add_public_dependency("iostreams", "math", true);
        add_public_dependency("iostreams", "move", true);
        add_public_dependency("iostreams", "mp11", true);
        add_public_dependency("iostreams", "mpl", true);
        add_public_dependency("iostreams", "multiprecision", true);
        add_public_dependency("iostreams", "numeric", true);
        add_public_dependency("iostreams", "optional", true);
        add_public_dependency("iostreams", "predef", true);
        add_public_dependency("iostreams", "preprocessor", true);
        add_public_dependency("iostreams", "random", true);
        add_public_dependency("iostreams", "range", true);
        add_public_dependency("iostreams", "rational", true);
        add_public_dependency("iostreams", "regex", true);
        add_public_dependency("iostreams", "smart_ptr", true);
        add_public_dependency("iostreams", "static_assert", true);
        add_public_dependency("iostreams", "system", true);
        add_public_dependency("iostreams", "throw_exception", true);
        add_public_dependency("iostreams", "tuple", true);
        add_public_dependency("iostreams", "type_index", true);
        add_public_dependency("iostreams", "type_traits", true);
        add_public_dependency("iostreams", "typeof", true);
        add_public_dependency("iostreams", "utility", true);
        add_public_dependency("iostreams", "winapi", true);
        add_public_dependency("iterator", "assert", true);
        add_public_dependency("iterator", "concept_check", true);
        add_public_dependency("iterator", "config", true);
        add_public_dependency("iterator", "container_hash", true);
        add_public_dependency("iterator", "conversion", true);
        add_public_dependency("iterator", "core", true);
        add_public_dependency("iterator", "detail", true);
        add_public_dependency("iterator", "function_types", true);
        add_public_dependency("iterator", "fusion", true);
        add_public_dependency("iterator", "integer", true);
        add_public_dependency("iterator", "io", true);
        add_public_dependency("iterator", "move", true);
        add_public_dependency("iterator", "mpl", true);
        add_public_dependency("iterator", "optional", true);
        add_public_dependency("iterator", "predef", true);
        add_public_dependency("iterator", "preprocessor", true);
        add_public_dependency("iterator", "smart_ptr", true);
        add_public_dependency("iterator", "static_assert", true);
        add_public_dependency("iterator", "throw_exception", true);
        add_public_dependency("iterator", "tuple", true);
        add_public_dependency("iterator", "type_traits", true);
        add_public_dependency("iterator", "typeof", true);
        add_public_dependency("iterator", "utility", true);
        add_public_dependency("lambda", "assert", true);
        add_public_dependency("lambda", "bind", true);
        add_public_dependency("lambda", "concept_check", true);
        add_public_dependency("lambda", "config", true);
        add_public_dependency("lambda", "container_hash", true);
        add_public_dependency("lambda", "conversion", true);
        add_public_dependency("lambda", "core", true);
        add_public_dependency("lambda", "detail", true);
        add_public_dependency("lambda", "function_types", true);
        add_public_dependency("lambda", "fusion", true);
        add_public_dependency("lambda", "integer", true);
        add_public_dependency("lambda", "io", true);
        add_public_dependency("lambda", "iterator", true);
        add_public_dependency("lambda", "move", true);
        add_public_dependency("lambda", "mpl", true);
        add_public_dependency("lambda", "optional", true);
        add_public_dependency("lambda", "predef", true);
        add_public_dependency("lambda", "preprocessor", true);
        add_public_dependency("lambda", "smart_ptr", true);
        add_public_dependency("lambda", "static_assert", true);
        add_public_dependency("lambda", "throw_exception", true);
        add_public_dependency("lambda", "tuple", true);
        add_public_dependency("lambda", "type_traits", true);
        add_public_dependency("lambda", "typeof", true);
        add_public_dependency("lambda", "utility", true);
        add_public_dependency("lexical_cast", "array", true);
        add_public_dependency("lexical_cast", "assert", true);
        add_public_dependency("lexical_cast", "atomic", true);
        add_public_dependency("lexical_cast", "bind", true);
        add_public_dependency("lexical_cast", "concept_check", true);
        add_public_dependency("lexical_cast", "config", true);
        add_public_dependency("lexical_cast", "container", true);
        add_public_dependency("lexical_cast", "container_hash", true);
        add_public_dependency("lexical_cast", "conversion", true);
        add_public_dependency("lexical_cast", "core", true);
        add_public_dependency("lexical_cast", "detail", true);
        add_public_dependency("lexical_cast", "function_types", true);
        add_public_dependency("lexical_cast", "fusion", true);
        add_public_dependency("lexical_cast", "integer", true);
        add_public_dependency("lexical_cast", "intrusive", true);
        add_public_dependency("lexical_cast", "io", true);
        add_public_dependency("lexical_cast", "iterator", true);
        add_public_dependency("lexical_cast", "lambda", true);
        add_public_dependency("lexical_cast", "math", true);
        add_public_dependency("lexical_cast", "move", true);
        add_public_dependency("lexical_cast", "mp11", true);
        add_public_dependency("lexical_cast", "mpl", true);
        add_public_dependency("lexical_cast", "numeric", true);
        add_public_dependency("lexical_cast", "optional", true);
        add_public_dependency("lexical_cast", "predef", true);
        add_public_dependency("lexical_cast", "preprocessor", true);
        add_public_dependency("lexical_cast", "range", true);
        add_public_dependency("lexical_cast", "regex", true);
        add_public_dependency("lexical_cast", "smart_ptr", true);
        add_public_dependency("lexical_cast", "static_assert", true);
        add_public_dependency("lexical_cast", "throw_exception", true);
        add_public_dependency("lexical_cast", "tuple", true);
        add_public_dependency("lexical_cast", "type_traits", true);
        add_public_dependency("lexical_cast", "typeof", true);
        add_public_dependency("lexical_cast", "utility", true);
        add_public_dependency("lexical_cast", "winapi", true);
        add_public_dependency("local_function", "assert", true);
        add_public_dependency("local_function", "bind", true);
        add_public_dependency("local_function", "config", true);
        add_public_dependency("local_function", "container_hash", true);
        add_public_dependency("local_function", "core", true);
        add_public_dependency("local_function", "detail", true);
        add_public_dependency("local_function", "function", true);
        add_public_dependency("local_function", "integer", true);
        add_public_dependency("local_function", "io", true);
        add_public_dependency("local_function", "move", true);
        add_public_dependency("local_function", "mpl", true);
        add_public_dependency("local_function", "predef", true);
        add_public_dependency("local_function", "preprocessor", true);
        add_public_dependency("local_function", "scope_exit", true);
        add_public_dependency("local_function", "smart_ptr", true);
        add_public_dependency("local_function", "static_assert", true);
        add_public_dependency("local_function", "throw_exception", true);
        add_public_dependency("local_function", "type_index", true);
        add_public_dependency("local_function", "type_traits", true);
        add_public_dependency("local_function", "typeof", true);
        add_public_dependency("local_function", "utility", true);
        add_public_dependency("locale", "system", false);
        add_public_dependency("locale", "thread", false);
        add_public_dependency("locale", "algorithm", true);
        add_public_dependency("locale", "array", true);
        add_public_dependency("locale", "assert", true);
        add_public_dependency("locale", "atomic", true);
        add_public_dependency("locale", "bind", true);
        add_public_dependency("locale", "chrono", true);
        add_public_dependency("locale", "concept_check", true);
        add_public_dependency("locale", "config", true);
        add_public_dependency("locale", "container", true);
        add_public_dependency("locale", "container_hash", true);
        add_public_dependency("locale", "conversion", true);
        add_public_dependency("locale", "core", true);
        add_public_dependency("locale", "date_time", true);
        add_public_dependency("locale", "detail", true);
        add_public_dependency("locale", "exception", true);
        add_public_dependency("locale", "function", true);
        add_public_dependency("locale", "function_types", true);
        add_public_dependency("locale", "fusion", true);
        add_public_dependency("locale", "integer", true);
        add_public_dependency("locale", "intrusive", true);
        add_public_dependency("locale", "io", true);
        add_public_dependency("locale", "iterator", true);
        add_public_dependency("locale", "lambda", true);
        add_public_dependency("locale", "lexical_cast", true);
        add_public_dependency("locale", "math", true);
        add_public_dependency("locale", "move", true);
        add_public_dependency("locale", "mp11", true);
        add_public_dependency("locale", "mpl", true);
        add_public_dependency("locale", "numeric", true);
        add_public_dependency("locale", "optional", true);
        add_public_dependency("locale", "predef", true);
        add_public_dependency("locale", "preprocessor", true);
        add_public_dependency("locale", "range", true);
        add_public_dependency("locale", "ratio", true);
        add_public_dependency("locale", "rational", true);
        add_public_dependency("locale", "regex", true);
        add_public_dependency("locale", "smart_ptr", true);
        add_public_dependency("locale", "static_assert", true);
        add_public_dependency("locale", "throw_exception", true);
        add_public_dependency("locale", "tokenizer", true);
        add_public_dependency("locale", "tuple", true);
        add_public_dependency("locale", "type_index", true);
        add_public_dependency("locale", "type_traits", true);
        add_public_dependency("locale", "typeof", true);
        add_public_dependency("locale", "unordered", true);
        add_public_dependency("locale", "utility", true);
        add_public_dependency("locale", "winapi", true);
        add_public_dependency("lockfree", "align", true);
        add_public_dependency("lockfree", "array", true);
        add_public_dependency("lockfree", "assert", true);
        add_public_dependency("lockfree", "atomic", true);
        add_public_dependency("lockfree", "bind", true);
        add_public_dependency("lockfree", "concept_check", true);
        add_public_dependency("lockfree", "config", true);
        add_public_dependency("lockfree", "container_hash", true);
        add_public_dependency("lockfree", "conversion", true);
        add_public_dependency("lockfree", "core", true);
        add_public_dependency("lockfree", "detail", true);
        add_public_dependency("lockfree", "function", true);
        add_public_dependency("lockfree", "function_types", true);
        add_public_dependency("lockfree", "fusion", true);
        add_public_dependency("lockfree", "integer", true);
        add_public_dependency("lockfree", "io", true);
        add_public_dependency("lockfree", "iterator", true);
        add_public_dependency("lockfree", "move", true);
        add_public_dependency("lockfree", "mp11", true);
        add_public_dependency("lockfree", "mpl", true);
        add_public_dependency("lockfree", "optional", true);
        add_public_dependency("lockfree", "parameter", true);
        add_public_dependency("lockfree", "predef", true);
        add_public_dependency("lockfree", "preprocessor", true);
        add_public_dependency("lockfree", "smart_ptr", true);
        add_public_dependency("lockfree", "static_assert", true);
        add_public_dependency("lockfree", "throw_exception", true);
        add_public_dependency("lockfree", "tuple", true);
        add_public_dependency("lockfree", "type_index", true);
        add_public_dependency("lockfree", "type_traits", true);
        add_public_dependency("lockfree", "typeof", true);
        add_public_dependency("lockfree", "utility", true);
        add_public_dependency("lockfree", "winapi", true);
        add_public_dependency("log", "atomic", false);
        add_public_dependency("log", "filesystem", false);
        add_public_dependency("log", "regex", false);
        add_public_dependency("log", "thread", false);
        add_public_dependency("log", "algorithm", true);
        add_public_dependency("log", "align", true);
        add_public_dependency("log", "any", true);
        add_public_dependency("log", "array", true);
        add_public_dependency("log", "asio", true);
        add_public_dependency("log", "assert", true);
        add_public_dependency("log", "bind", true);
        add_public_dependency("log", "chrono", true);
        add_public_dependency("log", "concept_check", true);
        add_public_dependency("log", "config", true);
        add_public_dependency("log", "container", true);
        add_public_dependency("log", "container_hash", true);
        add_public_dependency("log", "context", true);
        add_public_dependency("log", "conversion", true);
        add_public_dependency("log", "core", true);
        add_public_dependency("log", "coroutine", true);
        add_public_dependency("log", "date_time", true);
        add_public_dependency("log", "detail", true);
        add_public_dependency("log", "dynamic_bitset", true);
        add_public_dependency("log", "endian", true);
        add_public_dependency("log", "exception", true);
        add_public_dependency("log", "foreach", true);
        add_public_dependency("log", "format", true);
        add_public_dependency("log", "function", true);
        add_public_dependency("log", "function_types", true);
        add_public_dependency("log", "fusion", true);
        add_public_dependency("log", "integer", true);
        add_public_dependency("log", "interprocess", true);
        add_public_dependency("log", "intrusive", true);
        add_public_dependency("log", "io", true);
        add_public_dependency("log", "iterator", true);
        add_public_dependency("log", "lambda", true);
        add_public_dependency("log", "lexical_cast", true);
        add_public_dependency("log", "locale", true);
        add_public_dependency("log", "math", true);
        add_public_dependency("log", "move", true);
        add_public_dependency("log", "mp11", true);
        add_public_dependency("log", "mpl", true);
        add_public_dependency("log", "multi_index", true);
        add_public_dependency("log", "multiprecision", true);
        add_public_dependency("log", "numeric", true);
        add_public_dependency("log", "optional", true);
        add_public_dependency("log", "parameter", true);
        add_public_dependency("log", "phoenix", true);
        add_public_dependency("log", "pool", true);
        add_public_dependency("log", "predef", true);
        add_public_dependency("log", "preprocessor", true);
        add_public_dependency("log", "property_tree", true);
        add_public_dependency("log", "proto", true);
        add_public_dependency("log", "random", true);
        add_public_dependency("log", "range", true);
        add_public_dependency("log", "ratio", true);
        add_public_dependency("log", "rational", true);
        add_public_dependency("log", "serialization", true);
        add_public_dependency("log", "smart_ptr", true);
        add_public_dependency("log", "spirit", true);
        add_public_dependency("log", "static_assert", true);
        add_public_dependency("log", "system", true);
        add_public_dependency("log", "throw_exception", true);
        add_public_dependency("log", "tokenizer", true);
        add_public_dependency("log", "tuple", true);
        add_public_dependency("log", "type_index", true);
        add_public_dependency("log", "type_traits", true);
        add_public_dependency("log", "typeof", true);
        add_public_dependency("log", "unordered", true);
        add_public_dependency("log", "utility", true);
        add_public_dependency("log", "variant", true);
        add_public_dependency("log", "winapi", true);
        add_public_dependency("log", "xpressive", true);
        add_public_dependency("logic", "assert", true);
        add_public_dependency("logic", "config", true);
        add_public_dependency("logic", "core", true);
        add_public_dependency("math", "array", true);
        add_public_dependency("math", "assert", true);
        add_public_dependency("math", "atomic", true);
        add_public_dependency("math", "bind", true);
        add_public_dependency("math", "concept_check", true);
        add_public_dependency("math", "config", true);
        add_public_dependency("math", "container", true);
        add_public_dependency("math", "container_hash", true);
        add_public_dependency("math", "conversion", true);
        add_public_dependency("math", "core", true);
        add_public_dependency("math", "detail", true);
        add_public_dependency("math", "function_types", true);
        add_public_dependency("math", "fusion", true);
        add_public_dependency("math", "integer", true);
        add_public_dependency("math", "intrusive", true);
        add_public_dependency("math", "io", true);
        add_public_dependency("math", "iterator", true);
        add_public_dependency("math", "lambda", true);
        add_public_dependency("math", "lexical_cast", true);
        add_public_dependency("math", "move", true);
        add_public_dependency("math", "mp11", true);
        add_public_dependency("math", "mpl", true);
        add_public_dependency("math", "numeric", true);
        add_public_dependency("math", "optional", true);
        add_public_dependency("math", "predef", true);
        add_public_dependency("math", "preprocessor", true);
        add_public_dependency("math", "range", true);
        add_public_dependency("math", "regex", true);
        add_public_dependency("math", "smart_ptr", true);
        add_public_dependency("math", "static_assert", true);
        add_public_dependency("math", "throw_exception", true);
        add_public_dependency("math", "tuple", true);
        add_public_dependency("math", "type_traits", true);
        add_public_dependency("math", "typeof", true);
        add_public_dependency("math", "utility", true);
        add_public_dependency("math", "winapi", true);
        add_public_dependency("metaparse", "assert", true);
        add_public_dependency("metaparse", "config", true);
        add_public_dependency("metaparse", "container_hash", true);
        add_public_dependency("metaparse", "core", true);
        add_public_dependency("metaparse", "detail", true);
        add_public_dependency("metaparse", "integer", true);
        add_public_dependency("metaparse", "io", true);
        add_public_dependency("metaparse", "mpl", true);
        add_public_dependency("metaparse", "predef", true);
        add_public_dependency("metaparse", "preprocessor", true);
        add_public_dependency("metaparse", "static_assert", true);
        add_public_dependency("metaparse", "throw_exception", true);
        add_public_dependency("metaparse", "type_traits", true);
        add_public_dependency("metaparse", "utility", true);
        add_public_dependency("move", "assert", true);
        add_public_dependency("move", "config", true);
        add_public_dependency("move", "core", true);
        add_public_dependency("move", "static_assert", true);
        add_public_dependency("mpi", "python", false);
        add_public_dependency("mpi", "algorithm", true);
        add_public_dependency("mpi", "any", true);
        add_public_dependency("mpi", "array", true);
        add_public_dependency("mpi", "assert", true);
        add_public_dependency("mpi", "atomic", true);
        add_public_dependency("mpi", "bimap", true);
        add_public_dependency("mpi", "bind", true);
        add_public_dependency("mpi", "chrono", true);
        add_public_dependency("mpi", "concept_check", true);
        add_public_dependency("mpi", "config", true);
        add_public_dependency("mpi", "container", true);
        add_public_dependency("mpi", "container_hash", true);
        add_public_dependency("mpi", "conversion", true);
        add_public_dependency("mpi", "core", true);
        add_public_dependency("mpi", "date_time", true);
        add_public_dependency("mpi", "detail", true);
        add_public_dependency("mpi", "dynamic_bitset", true);
        add_public_dependency("mpi", "endian", true);
        add_public_dependency("mpi", "exception", true);
        add_public_dependency("mpi", "foreach", true);
        add_public_dependency("mpi", "format", true);
        add_public_dependency("mpi", "function", true);
        add_public_dependency("mpi", "function_types", true);
        add_public_dependency("mpi", "fusion", true);
        add_public_dependency("mpi", "graph", true);
        add_public_dependency("mpi", "integer", true);
        add_public_dependency("mpi", "intrusive", true);
        add_public_dependency("mpi", "io", true);
        add_public_dependency("mpi", "iterator", true);
        add_public_dependency("mpi", "lambda", true);
        add_public_dependency("mpi", "lexical_cast", true);
        add_public_dependency("mpi", "math", true);
        add_public_dependency("mpi", "move", true);
        add_public_dependency("mpi", "mp11", true);
        add_public_dependency("mpi", "mpl", true);
        add_public_dependency("mpi", "multi_index", true);
        add_public_dependency("mpi", "multiprecision", true);
        add_public_dependency("mpi", "numeric", true);
        add_public_dependency("mpi", "optional", true);
        add_public_dependency("mpi", "parameter", true);
        add_public_dependency("mpi", "phoenix", true);
        add_public_dependency("mpi", "pool", true);
        add_public_dependency("mpi", "predef", true);
        add_public_dependency("mpi", "preprocessor", true);
        add_public_dependency("mpi", "property_map", true);
        add_public_dependency("mpi", "property_tree", true);
        add_public_dependency("mpi", "proto", true);
        add_public_dependency("mpi", "random", true);
        add_public_dependency("mpi", "range", true);
        add_public_dependency("mpi", "ratio", true);
        add_public_dependency("mpi", "rational", true);
        add_public_dependency("mpi", "regex", true);
        add_public_dependency("mpi", "serialization", true);
        add_public_dependency("mpi", "smart_ptr", true);
        add_public_dependency("mpi", "spirit", true);
        add_public_dependency("mpi", "static_assert", true);
        add_public_dependency("mpi", "system", true);
        add_public_dependency("mpi", "test", true);
        add_public_dependency("mpi", "thread", true);
        add_public_dependency("mpi", "throw_exception", true);
        add_public_dependency("mpi", "tokenizer", true);
        add_public_dependency("mpi", "tti", true);
        add_public_dependency("mpi", "tuple", true);
        add_public_dependency("mpi", "type_index", true);
        add_public_dependency("mpi", "type_traits", true);
        add_public_dependency("mpi", "typeof", true);
        add_public_dependency("mpi", "unordered", true);
        add_public_dependency("mpi", "utility", true);
        add_public_dependency("mpi", "variant", true);
        add_public_dependency("mpi", "winapi", true);
        add_public_dependency("mpi", "xpressive", true);
        add_public_dependency("mpl", "assert", true);
        add_public_dependency("mpl", "config", true);
        add_public_dependency("mpl", "container_hash", true);
        add_public_dependency("mpl", "core", true);
        add_public_dependency("mpl", "detail", true);
        add_public_dependency("mpl", "integer", true);
        add_public_dependency("mpl", "io", true);
        add_public_dependency("mpl", "predef", true);
        add_public_dependency("mpl", "preprocessor", true);
        add_public_dependency("mpl", "static_assert", true);
        add_public_dependency("mpl", "throw_exception", true);
        add_public_dependency("mpl", "type_traits", true);
        add_public_dependency("mpl", "utility", true);
        add_public_dependency("msm", "algorithm", true);
        add_public_dependency("msm", "any", true);
        add_public_dependency("msm", "array", true);
        add_public_dependency("msm", "assert", true);
        add_public_dependency("msm", "atomic", true);
        add_public_dependency("msm", "bind", true);
        add_public_dependency("msm", "chrono", true);
        add_public_dependency("msm", "circular_buffer", true);
        add_public_dependency("msm", "concept_check", true);
        add_public_dependency("msm", "config", true);
        add_public_dependency("msm", "container", true);
        add_public_dependency("msm", "container_hash", true);
        add_public_dependency("msm", "conversion", true);
        add_public_dependency("msm", "core", true);
        add_public_dependency("msm", "date_time", true);
        add_public_dependency("msm", "detail", true);
        add_public_dependency("msm", "endian", true);
        add_public_dependency("msm", "exception", true);
        add_public_dependency("msm", "foreach", true);
        add_public_dependency("msm", "function", true);
        add_public_dependency("msm", "function_types", true);
        add_public_dependency("msm", "fusion", true);
        add_public_dependency("msm", "integer", true);
        add_public_dependency("msm", "intrusive", true);
        add_public_dependency("msm", "io", true);
        add_public_dependency("msm", "iterator", true);
        add_public_dependency("msm", "lambda", true);
        add_public_dependency("msm", "lexical_cast", true);
        add_public_dependency("msm", "math", true);
        add_public_dependency("msm", "move", true);
        add_public_dependency("msm", "mp11", true);
        add_public_dependency("msm", "mpl", true);
        add_public_dependency("msm", "numeric", true);
        add_public_dependency("msm", "optional", true);
        add_public_dependency("msm", "parameter", true);
        add_public_dependency("msm", "phoenix", true);
        add_public_dependency("msm", "pool", true);
        add_public_dependency("msm", "predef", true);
        add_public_dependency("msm", "preprocessor", true);
        add_public_dependency("msm", "proto", true);
        add_public_dependency("msm", "range", true);
        add_public_dependency("msm", "ratio", true);
        add_public_dependency("msm", "rational", true);
        add_public_dependency("msm", "regex", true);
        add_public_dependency("msm", "serialization", true);
        add_public_dependency("msm", "smart_ptr", true);
        add_public_dependency("msm", "spirit", true);
        add_public_dependency("msm", "static_assert", true);
        add_public_dependency("msm", "system", true);
        add_public_dependency("msm", "thread", true);
        add_public_dependency("msm", "throw_exception", true);
        add_public_dependency("msm", "tokenizer", true);
        add_public_dependency("msm", "tuple", true);
        add_public_dependency("msm", "type_index", true);
        add_public_dependency("msm", "type_traits", true);
        add_public_dependency("msm", "typeof", true);
        add_public_dependency("msm", "unordered", true);
        add_public_dependency("msm", "utility", true);
        add_public_dependency("msm", "variant", true);
        add_public_dependency("msm", "winapi", true);
        add_public_dependency("multi_array", "array", true);
        add_public_dependency("multi_array", "assert", true);
        add_public_dependency("multi_array", "bind", true);
        add_public_dependency("multi_array", "concept_check", true);
        add_public_dependency("multi_array", "config", true);
        add_public_dependency("multi_array", "container_hash", true);
        add_public_dependency("multi_array", "conversion", true);
        add_public_dependency("multi_array", "core", true);
        add_public_dependency("multi_array", "detail", true);
        add_public_dependency("multi_array", "function", true);
        add_public_dependency("multi_array", "function_types", true);
        add_public_dependency("multi_array", "functional", true);
        add_public_dependency("multi_array", "fusion", true);
        add_public_dependency("multi_array", "integer", true);
        add_public_dependency("multi_array", "io", true);
        add_public_dependency("multi_array", "iterator", true);
        add_public_dependency("multi_array", "move", true);
        add_public_dependency("multi_array", "mpl", true);
        add_public_dependency("multi_array", "optional", true);
        add_public_dependency("multi_array", "predef", true);
        add_public_dependency("multi_array", "preprocessor", true);
        add_public_dependency("multi_array", "smart_ptr", true);
        add_public_dependency("multi_array", "static_assert", true);
        add_public_dependency("multi_array", "throw_exception", true);
        add_public_dependency("multi_array", "tuple", true);
        add_public_dependency("multi_array", "type_index", true);
        add_public_dependency("multi_array", "type_traits", true);
        add_public_dependency("multi_array", "typeof", true);
        add_public_dependency("multi_array", "utility", true);
        add_public_dependency("multi_index", "algorithm", true);
        add_public_dependency("multi_index", "array", true);
        add_public_dependency("multi_index", "assert", true);
        add_public_dependency("multi_index", "atomic", true);
        add_public_dependency("multi_index", "bind", true);
        add_public_dependency("multi_index", "chrono", true);
        add_public_dependency("multi_index", "concept_check", true);
        add_public_dependency("multi_index", "config", true);
        add_public_dependency("multi_index", "container", true);
        add_public_dependency("multi_index", "container_hash", true);
        add_public_dependency("multi_index", "conversion", true);
        add_public_dependency("multi_index", "core", true);
        add_public_dependency("multi_index", "date_time", true);
        add_public_dependency("multi_index", "detail", true);
        add_public_dependency("multi_index", "endian", true);
        add_public_dependency("multi_index", "exception", true);
        add_public_dependency("multi_index", "foreach", true);
        add_public_dependency("multi_index", "function", true);
        add_public_dependency("multi_index", "function_types", true);
        add_public_dependency("multi_index", "fusion", true);
        add_public_dependency("multi_index", "integer", true);
        add_public_dependency("multi_index", "intrusive", true);
        add_public_dependency("multi_index", "io", true);
        add_public_dependency("multi_index", "iterator", true);
        add_public_dependency("multi_index", "lambda", true);
        add_public_dependency("multi_index", "lexical_cast", true);
        add_public_dependency("multi_index", "math", true);
        add_public_dependency("multi_index", "move", true);
        add_public_dependency("multi_index", "mp11", true);
        add_public_dependency("multi_index", "mpl", true);
        add_public_dependency("multi_index", "numeric", true);
        add_public_dependency("multi_index", "optional", true);
        add_public_dependency("multi_index", "phoenix", true);
        add_public_dependency("multi_index", "pool", true);
        add_public_dependency("multi_index", "predef", true);
        add_public_dependency("multi_index", "preprocessor", true);
        add_public_dependency("multi_index", "proto", true);
        add_public_dependency("multi_index", "range", true);
        add_public_dependency("multi_index", "ratio", true);
        add_public_dependency("multi_index", "rational", true);
        add_public_dependency("multi_index", "regex", true);
        add_public_dependency("multi_index", "serialization", true);
        add_public_dependency("multi_index", "smart_ptr", true);
        add_public_dependency("multi_index", "spirit", true);
        add_public_dependency("multi_index", "static_assert", true);
        add_public_dependency("multi_index", "system", true);
        add_public_dependency("multi_index", "thread", true);
        add_public_dependency("multi_index", "throw_exception", true);
        add_public_dependency("multi_index", "tokenizer", true);
        add_public_dependency("multi_index", "tuple", true);
        add_public_dependency("multi_index", "type_index", true);
        add_public_dependency("multi_index", "type_traits", true);
        add_public_dependency("multi_index", "typeof", true);
        add_public_dependency("multi_index", "unordered", true);
        add_public_dependency("multi_index", "utility", true);
        add_public_dependency("multi_index", "variant", true);
        add_public_dependency("multi_index", "winapi", true);
        add_public_dependency("multiprecision", "array", true);
        add_public_dependency("multiprecision", "assert", true);
        add_public_dependency("multiprecision", "atomic", true);
        add_public_dependency("multiprecision", "bind", true);
        add_public_dependency("multiprecision", "concept_check", true);
        add_public_dependency("multiprecision", "config", true);
        add_public_dependency("multiprecision", "container", true);
        add_public_dependency("multiprecision", "container_hash", true);
        add_public_dependency("multiprecision", "conversion", true);
        add_public_dependency("multiprecision", "core", true);
        add_public_dependency("multiprecision", "detail", true);
        add_public_dependency("multiprecision", "dynamic_bitset", true);
        add_public_dependency("multiprecision", "function_types", true);
        add_public_dependency("multiprecision", "fusion", true);
        add_public_dependency("multiprecision", "integer", true);
        add_public_dependency("multiprecision", "intrusive", true);
        add_public_dependency("multiprecision", "io", true);
        add_public_dependency("multiprecision", "iterator", true);
        add_public_dependency("multiprecision", "lambda", true);
        add_public_dependency("multiprecision", "lexical_cast", true);
        add_public_dependency("multiprecision", "math", true);
        add_public_dependency("multiprecision", "move", true);
        add_public_dependency("multiprecision", "mp11", true);
        add_public_dependency("multiprecision", "mpl", true);
        add_public_dependency("multiprecision", "numeric", true);
        add_public_dependency("multiprecision", "optional", true);
        add_public_dependency("multiprecision", "predef", true);
        add_public_dependency("multiprecision", "preprocessor", true);
        add_public_dependency("multiprecision", "random", true);
        add_public_dependency("multiprecision", "range", true);
        add_public_dependency("multiprecision", "rational", true);
        add_public_dependency("multiprecision", "regex", true);
        add_public_dependency("multiprecision", "smart_ptr", true);
        add_public_dependency("multiprecision", "static_assert", true);
        add_public_dependency("multiprecision", "system", true);
        add_public_dependency("multiprecision", "throw_exception", true);
        add_public_dependency("multiprecision", "tuple", true);
        add_public_dependency("multiprecision", "type_traits", true);
        add_public_dependency("multiprecision", "typeof", true);
        add_public_dependency("multiprecision", "utility", true);
        add_public_dependency("multiprecision", "winapi", true);
        add_public_dependency("nowide", "assert", true);
        add_public_dependency("nowide", "concept_check", true);
        add_public_dependency("nowide", "config", true);
        add_public_dependency("nowide", "container_hash", true);
        add_public_dependency("nowide", "conversion", true);
        add_public_dependency("nowide", "core", true);
        add_public_dependency("nowide", "detail", true);
        add_public_dependency("nowide", "filesystem", true);
        add_public_dependency("nowide", "function_types", true);
        add_public_dependency("nowide", "fusion", true);
        add_public_dependency("nowide", "integer", true);
        add_public_dependency("nowide", "io", true);
        add_public_dependency("nowide", "iterator", true);
        add_public_dependency("nowide", "move", true);
        add_public_dependency("nowide", "mpl", true);
        add_public_dependency("nowide", "optional", true);
        add_public_dependency("nowide", "predef", true);
        add_public_dependency("nowide", "preprocessor", true);
        add_public_dependency("nowide", "smart_ptr", true);
        add_public_dependency("nowide", "static_assert", true);
        add_public_dependency("nowide", "system", true);
        add_public_dependency("nowide", "throw_exception", true);
        add_public_dependency("nowide", "tuple", true);
        add_public_dependency("nowide", "type_traits", true);
        add_public_dependency("nowide", "typeof", true);
        add_public_dependency("nowide", "utility", true);
        add_public_dependency("nowide", "winapi", true);
        add_public_dependency("numeric", "assert", true);
        add_public_dependency("numeric", "config", true);
        add_public_dependency("numeric", "container_hash", true);
        add_public_dependency("numeric", "conversion", true);
        add_public_dependency("numeric", "core", true);
        add_public_dependency("numeric", "detail", true);
        add_public_dependency("numeric", "integer", true);
        add_public_dependency("numeric", "io", true);
        add_public_dependency("numeric", "move", true);
        add_public_dependency("numeric", "mpl", true);
        add_public_dependency("numeric", "predef", true);
        add_public_dependency("numeric", "preprocessor", true);
        add_public_dependency("numeric", "smart_ptr", true);
        add_public_dependency("numeric", "static_assert", true);
        add_public_dependency("numeric", "throw_exception", true);
        add_public_dependency("numeric", "type_traits", true);
        add_public_dependency("numeric", "typeof", true);
        add_public_dependency("numeric", "utility", true);
        add_public_dependency("interval", "assert", true);
        add_public_dependency("interval", "config", true);
        add_public_dependency("interval", "core", true);
        add_public_dependency("interval", "detail", true);
        add_public_dependency("interval", "logic", true);
        add_public_dependency("interval", "preprocessor", true);
        add_public_dependency("interval", "static_assert", true);
        add_public_dependency("interval", "type_traits", true);
        add_public_dependency("odeint", "algorithm", true);
        add_public_dependency("odeint", "any", true);
        add_public_dependency("odeint", "array", true);
        add_public_dependency("odeint", "assert", true);
        add_public_dependency("odeint", "atomic", true);
        add_public_dependency("odeint", "bimap", true);
        add_public_dependency("odeint", "bind", true);
        add_public_dependency("odeint", "chrono", true);
        add_public_dependency("odeint", "compute", true);
        add_public_dependency("odeint", "concept_check", true);
        add_public_dependency("odeint", "config", true);
        add_public_dependency("odeint", "container", true);
        add_public_dependency("odeint", "container_hash", true);
        add_public_dependency("odeint", "conversion", true);
        add_public_dependency("odeint", "core", true);
        add_public_dependency("odeint", "date_time", true);
        add_public_dependency("odeint", "detail", true);
        add_public_dependency("odeint", "dynamic_bitset", true);
        add_public_dependency("odeint", "endian", true);
        add_public_dependency("odeint", "exception", true);
        add_public_dependency("odeint", "filesystem", true);
        add_public_dependency("odeint", "foreach", true);
        add_public_dependency("odeint", "format", true);
        add_public_dependency("odeint", "function", true);
        add_public_dependency("odeint", "function_types", true);
        add_public_dependency("odeint", "functional", true);
        add_public_dependency("odeint", "fusion", true);
        add_public_dependency("odeint", "graph", true);
        add_public_dependency("odeint", "integer", true);
        add_public_dependency("odeint", "interval", true);
        add_public_dependency("odeint", "intrusive", true);
        add_public_dependency("odeint", "io", true);
        add_public_dependency("odeint", "iterator", true);
        add_public_dependency("odeint", "lambda", true);
        add_public_dependency("odeint", "lexical_cast", true);
        add_public_dependency("odeint", "logic", true);
        add_public_dependency("odeint", "math", true);
        add_public_dependency("odeint", "move", true);
        add_public_dependency("odeint", "mp11", true);
        add_public_dependency("odeint", "mpi", true);
        add_public_dependency("odeint", "mpl", true);
        add_public_dependency("odeint", "multi_array", true);
        add_public_dependency("odeint", "multi_index", true);
        add_public_dependency("odeint", "multiprecision", true);
        add_public_dependency("odeint", "numeric", true);
        add_public_dependency("odeint", "optional", true);
        add_public_dependency("odeint", "parameter", true);
        add_public_dependency("odeint", "phoenix", true);
        add_public_dependency("odeint", "pool", true);
        add_public_dependency("odeint", "predef", true);
        add_public_dependency("odeint", "preprocessor", true);
        add_public_dependency("odeint", "property_map", true);
        add_public_dependency("odeint", "property_tree", true);
        add_public_dependency("odeint", "proto", true);
        add_public_dependency("odeint", "python", true);
        add_public_dependency("odeint", "random", true);
        add_public_dependency("odeint", "range", true);
        add_public_dependency("odeint", "ratio", true);
        add_public_dependency("odeint", "rational", true);
        add_public_dependency("odeint", "regex", true);
        add_public_dependency("odeint", "serialization", true);
        add_public_dependency("odeint", "smart_ptr", true);
        add_public_dependency("odeint", "spirit", true);
        add_public_dependency("odeint", "static_assert", true);
        add_public_dependency("odeint", "system", true);
        add_public_dependency("odeint", "test", true);
        add_public_dependency("odeint", "thread", true);
        add_public_dependency("odeint", "throw_exception", true);
        add_public_dependency("odeint", "tokenizer", true);
        add_public_dependency("odeint", "tti", true);
        add_public_dependency("odeint", "tuple", true);
        add_public_dependency("odeint", "type_index", true);
        add_public_dependency("odeint", "type_traits", true);
        add_public_dependency("odeint", "typeof", true);
        add_public_dependency("odeint", "ublas", true);
        add_public_dependency("odeint", "units", true);
        add_public_dependency("odeint", "unordered", true);
        add_public_dependency("odeint", "utility", true);
        add_public_dependency("odeint", "uuid", true);
        add_public_dependency("odeint", "variant", true);
        add_public_dependency("odeint", "winapi", true);
        add_public_dependency("odeint", "xpressive", true);
        add_public_dependency("ublas", "algorithm", true);
        add_public_dependency("ublas", "any", true);
        add_public_dependency("ublas", "array", true);
        add_public_dependency("ublas", "assert", true);
        add_public_dependency("ublas", "atomic", true);
        add_public_dependency("ublas", "bind", true);
        add_public_dependency("ublas", "chrono", true);
        add_public_dependency("ublas", "compute", true);
        add_public_dependency("ublas", "concept_check", true);
        add_public_dependency("ublas", "config", true);
        add_public_dependency("ublas", "container", true);
        add_public_dependency("ublas", "container_hash", true);
        add_public_dependency("ublas", "conversion", true);
        add_public_dependency("ublas", "core", true);
        add_public_dependency("ublas", "date_time", true);
        add_public_dependency("ublas", "detail", true);
        add_public_dependency("ublas", "dynamic_bitset", true);
        add_public_dependency("ublas", "endian", true);
        add_public_dependency("ublas", "exception", true);
        add_public_dependency("ublas", "filesystem", true);
        add_public_dependency("ublas", "foreach", true);
        add_public_dependency("ublas", "format", true);
        add_public_dependency("ublas", "function", true);
        add_public_dependency("ublas", "function_types", true);
        add_public_dependency("ublas", "fusion", true);
        add_public_dependency("ublas", "integer", true);
        add_public_dependency("ublas", "interval", true);
        add_public_dependency("ublas", "intrusive", true);
        add_public_dependency("ublas", "io", true);
        add_public_dependency("ublas", "iterator", true);
        add_public_dependency("ublas", "lambda", true);
        add_public_dependency("ublas", "lexical_cast", true);
        add_public_dependency("ublas", "logic", true);
        add_public_dependency("ublas", "math", true);
        add_public_dependency("ublas", "move", true);
        add_public_dependency("ublas", "mp11", true);
        add_public_dependency("ublas", "mpl", true);
        add_public_dependency("ublas", "multi_index", true);
        add_public_dependency("ublas", "multiprecision", true);
        add_public_dependency("ublas", "numeric", true);
        add_public_dependency("ublas", "optional", true);
        add_public_dependency("ublas", "phoenix", true);
        add_public_dependency("ublas", "pool", true);
        add_public_dependency("ublas", "predef", true);
        add_public_dependency("ublas", "preprocessor", true);
        add_public_dependency("ublas", "property_tree", true);
        add_public_dependency("ublas", "proto", true);
        add_public_dependency("ublas", "random", true);
        add_public_dependency("ublas", "range", true);
        add_public_dependency("ublas", "ratio", true);
        add_public_dependency("ublas", "rational", true);
        add_public_dependency("ublas", "regex", true);
        add_public_dependency("ublas", "serialization", true);
        add_public_dependency("ublas", "smart_ptr", true);
        add_public_dependency("ublas", "spirit", true);
        add_public_dependency("ublas", "static_assert", true);
        add_public_dependency("ublas", "system", true);
        add_public_dependency("ublas", "thread", true);
        add_public_dependency("ublas", "throw_exception", true);
        add_public_dependency("ublas", "tokenizer", true);
        add_public_dependency("ublas", "tti", true);
        add_public_dependency("ublas", "tuple", true);
        add_public_dependency("ublas", "type_index", true);
        add_public_dependency("ublas", "type_traits", true);
        add_public_dependency("ublas", "typeof", true);
        add_public_dependency("ublas", "unordered", true);
        add_public_dependency("ublas", "utility", true);
        add_public_dependency("ublas", "uuid", true);
        add_public_dependency("ublas", "variant", true);
        add_public_dependency("ublas", "winapi", true);
        add_public_dependency("optional", "assert", true);
        add_public_dependency("optional", "config", true);
        add_public_dependency("optional", "container_hash", true);
        add_public_dependency("optional", "core", true);
        add_public_dependency("optional", "detail", true);
        add_public_dependency("optional", "integer", true);
        add_public_dependency("optional", "io", true);
        add_public_dependency("optional", "move", true);
        add_public_dependency("optional", "predef", true);
        add_public_dependency("optional", "preprocessor", true);
        add_public_dependency("optional", "static_assert", true);
        add_public_dependency("optional", "throw_exception", true);
        add_public_dependency("optional", "type_traits", true);
        add_public_dependency("optional", "utility", true);
        add_public_dependency("outcome", "assert", true);
        add_public_dependency("outcome", "config", true);
        add_public_dependency("outcome", "core", true);
        add_public_dependency("outcome", "exception", true);
        add_public_dependency("outcome", "move", true);
        add_public_dependency("outcome", "predef", true);
        add_public_dependency("outcome", "preprocessor", true);
        add_public_dependency("outcome", "smart_ptr", true);
        add_public_dependency("outcome", "static_assert", true);
        add_public_dependency("outcome", "system", true);
        add_public_dependency("outcome", "throw_exception", true);
        add_public_dependency("outcome", "tuple", true);
        add_public_dependency("outcome", "type_traits", true);
        add_public_dependency("outcome", "winapi", true);
        add_public_dependency("parameter", "assert", true);
        add_public_dependency("parameter", "bind", true);
        add_public_dependency("parameter", "config", true);
        add_public_dependency("parameter", "container_hash", true);
        add_public_dependency("parameter", "core", true);
        add_public_dependency("parameter", "detail", true);
        add_public_dependency("parameter", "function", true);
        add_public_dependency("parameter", "function_types", true);
        add_public_dependency("parameter", "fusion", true);
        add_public_dependency("parameter", "integer", true);
        add_public_dependency("parameter", "io", true);
        add_public_dependency("parameter", "move", true);
        add_public_dependency("parameter", "mp11", true);
        add_public_dependency("parameter", "mpl", true);
        add_public_dependency("parameter", "optional", true);
        add_public_dependency("parameter", "predef", true);
        add_public_dependency("parameter", "preprocessor", true);
        add_public_dependency("parameter", "smart_ptr", true);
        add_public_dependency("parameter", "static_assert", true);
        add_public_dependency("parameter", "throw_exception", true);
        add_public_dependency("parameter", "tuple", true);
        add_public_dependency("parameter", "type_index", true);
        add_public_dependency("parameter", "type_traits", true);
        add_public_dependency("parameter", "typeof", true);
        add_public_dependency("parameter", "utility", true);
        add_public_dependency("parameter_python", "algorithm", true);
        add_public_dependency("parameter_python", "any", true);
        add_public_dependency("parameter_python", "array", true);
        add_public_dependency("parameter_python", "assert", true);
        add_public_dependency("parameter_python", "atomic", true);
        add_public_dependency("parameter_python", "bimap", true);
        add_public_dependency("parameter_python", "bind", true);
        add_public_dependency("parameter_python", "chrono", true);
        add_public_dependency("parameter_python", "concept_check", true);
        add_public_dependency("parameter_python", "config", true);
        add_public_dependency("parameter_python", "container", true);
        add_public_dependency("parameter_python", "container_hash", true);
        add_public_dependency("parameter_python", "conversion", true);
        add_public_dependency("parameter_python", "core", true);
        add_public_dependency("parameter_python", "date_time", true);
        add_public_dependency("parameter_python", "detail", true);
        add_public_dependency("parameter_python", "dynamic_bitset", true);
        add_public_dependency("parameter_python", "endian", true);
        add_public_dependency("parameter_python", "exception", true);
        add_public_dependency("parameter_python", "foreach", true);
        add_public_dependency("parameter_python", "format", true);
        add_public_dependency("parameter_python", "function", true);
        add_public_dependency("parameter_python", "function_types", true);
        add_public_dependency("parameter_python", "fusion", true);
        add_public_dependency("parameter_python", "graph", true);
        add_public_dependency("parameter_python", "integer", true);
        add_public_dependency("parameter_python", "intrusive", true);
        add_public_dependency("parameter_python", "io", true);
        add_public_dependency("parameter_python", "iterator", true);
        add_public_dependency("parameter_python", "lambda", true);
        add_public_dependency("parameter_python", "lexical_cast", true);
        add_public_dependency("parameter_python", "math", true);
        add_public_dependency("parameter_python", "move", true);
        add_public_dependency("parameter_python", "mp11", true);
        add_public_dependency("parameter_python", "mpi", true);
        add_public_dependency("parameter_python", "mpl", true);
        add_public_dependency("parameter_python", "multi_index", true);
        add_public_dependency("parameter_python", "multiprecision", true);
        add_public_dependency("parameter_python", "numeric", true);
        add_public_dependency("parameter_python", "optional", true);
        add_public_dependency("parameter_python", "parameter", true);
        add_public_dependency("parameter_python", "phoenix", true);
        add_public_dependency("parameter_python", "pool", true);
        add_public_dependency("parameter_python", "predef", true);
        add_public_dependency("parameter_python", "preprocessor", true);
        add_public_dependency("parameter_python", "property_map", true);
        add_public_dependency("parameter_python", "property_tree", true);
        add_public_dependency("parameter_python", "proto", true);
        add_public_dependency("parameter_python", "python", true);
        add_public_dependency("parameter_python", "random", true);
        add_public_dependency("parameter_python", "range", true);
        add_public_dependency("parameter_python", "ratio", true);
        add_public_dependency("parameter_python", "rational", true);
        add_public_dependency("parameter_python", "regex", true);
        add_public_dependency("parameter_python", "serialization", true);
        add_public_dependency("parameter_python", "smart_ptr", true);
        add_public_dependency("parameter_python", "spirit", true);
        add_public_dependency("parameter_python", "static_assert", true);
        add_public_dependency("parameter_python", "system", true);
        add_public_dependency("parameter_python", "test", true);
        add_public_dependency("parameter_python", "thread", true);
        add_public_dependency("parameter_python", "throw_exception", true);
        add_public_dependency("parameter_python", "tokenizer", true);
        add_public_dependency("parameter_python", "tti", true);
        add_public_dependency("parameter_python", "tuple", true);
        add_public_dependency("parameter_python", "type_index", true);
        add_public_dependency("parameter_python", "type_traits", true);
        add_public_dependency("parameter_python", "typeof", true);
        add_public_dependency("parameter_python", "unordered", true);
        add_public_dependency("parameter_python", "utility", true);
        add_public_dependency("parameter_python", "variant", true);
        add_public_dependency("parameter_python", "winapi", true);
        add_public_dependency("parameter_python", "xpressive", true);
        add_public_dependency("phoenix", "array", true);
        add_public_dependency("phoenix", "assert", true);
        add_public_dependency("phoenix", "bind", true);
        add_public_dependency("phoenix", "concept_check", true);
        add_public_dependency("phoenix", "config", true);
        add_public_dependency("phoenix", "container_hash", true);
        add_public_dependency("phoenix", "conversion", true);
        add_public_dependency("phoenix", "core", true);
        add_public_dependency("phoenix", "detail", true);
        add_public_dependency("phoenix", "function", true);
        add_public_dependency("phoenix", "function_types", true);
        add_public_dependency("phoenix", "fusion", true);
        add_public_dependency("phoenix", "integer", true);
        add_public_dependency("phoenix", "io", true);
        add_public_dependency("phoenix", "iterator", true);
        add_public_dependency("phoenix", "move", true);
        add_public_dependency("phoenix", "mpl", true);
        add_public_dependency("phoenix", "numeric", true);
        add_public_dependency("phoenix", "optional", true);
        add_public_dependency("phoenix", "predef", true);
        add_public_dependency("phoenix", "preprocessor", true);
        add_public_dependency("phoenix", "proto", true);
        add_public_dependency("phoenix", "range", true);
        add_public_dependency("phoenix", "regex", true);
        add_public_dependency("phoenix", "smart_ptr", true);
        add_public_dependency("phoenix", "static_assert", true);
        add_public_dependency("phoenix", "throw_exception", true);
        add_public_dependency("phoenix", "tuple", true);
        add_public_dependency("phoenix", "type_index", true);
        add_public_dependency("phoenix", "type_traits", true);
        add_public_dependency("phoenix", "typeof", true);
        add_public_dependency("phoenix", "utility", true);
        add_public_dependency("poly_collection", "algorithm", true);
        add_public_dependency("poly_collection", "array", true);
        add_public_dependency("poly_collection", "assert", true);
        add_public_dependency("poly_collection", "atomic", true);
        add_public_dependency("poly_collection", "bind", true);
        add_public_dependency("poly_collection", "chrono", true);
        add_public_dependency("poly_collection", "concept_check", true);
        add_public_dependency("poly_collection", "config", true);
        add_public_dependency("poly_collection", "container", true);
        add_public_dependency("poly_collection", "container_hash", true);
        add_public_dependency("poly_collection", "conversion", true);
        add_public_dependency("poly_collection", "core", true);
        add_public_dependency("poly_collection", "date_time", true);
        add_public_dependency("poly_collection", "detail", true);
        add_public_dependency("poly_collection", "exception", true);
        add_public_dependency("poly_collection", "function", true);
        add_public_dependency("poly_collection", "function_types", true);
        add_public_dependency("poly_collection", "fusion", true);
        add_public_dependency("poly_collection", "integer", true);
        add_public_dependency("poly_collection", "intrusive", true);
        add_public_dependency("poly_collection", "io", true);
        add_public_dependency("poly_collection", "iterator", true);
        add_public_dependency("poly_collection", "lambda", true);
        add_public_dependency("poly_collection", "lexical_cast", true);
        add_public_dependency("poly_collection", "math", true);
        add_public_dependency("poly_collection", "move", true);
        add_public_dependency("poly_collection", "mp11", true);
        add_public_dependency("poly_collection", "mpl", true);
        add_public_dependency("poly_collection", "numeric", true);
        add_public_dependency("poly_collection", "optional", true);
        add_public_dependency("poly_collection", "predef", true);
        add_public_dependency("poly_collection", "preprocessor", true);
        add_public_dependency("poly_collection", "range", true);
        add_public_dependency("poly_collection", "ratio", true);
        add_public_dependency("poly_collection", "rational", true);
        add_public_dependency("poly_collection", "regex", true);
        add_public_dependency("poly_collection", "smart_ptr", true);
        add_public_dependency("poly_collection", "static_assert", true);
        add_public_dependency("poly_collection", "system", true);
        add_public_dependency("poly_collection", "thread", true);
        add_public_dependency("poly_collection", "throw_exception", true);
        add_public_dependency("poly_collection", "tokenizer", true);
        add_public_dependency("poly_collection", "tuple", true);
        add_public_dependency("poly_collection", "type_erasure", true);
        add_public_dependency("poly_collection", "type_index", true);
        add_public_dependency("poly_collection", "type_traits", true);
        add_public_dependency("poly_collection", "typeof", true);
        add_public_dependency("poly_collection", "unordered", true);
        add_public_dependency("poly_collection", "utility", true);
        add_public_dependency("poly_collection", "vmd", true);
        add_public_dependency("poly_collection", "winapi", true);
        add_public_dependency("polygon", "config", true);
        add_public_dependency("pool", "assert", true);
        add_public_dependency("pool", "config", true);
        add_public_dependency("pool", "core", true);
        add_public_dependency("pool", "integer", true);
        add_public_dependency("pool", "predef", true);
        add_public_dependency("pool", "preprocessor", true);
        add_public_dependency("pool", "static_assert", true);
        add_public_dependency("pool", "throw_exception", true);
        add_public_dependency("pool", "type_traits", true);
        add_public_dependency("pool", "winapi", true);
        add_public_dependency("process", "algorithm", true);
        add_public_dependency("process", "array", true);
        add_public_dependency("process", "asio", true);
        add_public_dependency("process", "assert", true);
        add_public_dependency("process", "atomic", true);
        add_public_dependency("process", "bind", true);
        add_public_dependency("process", "chrono", true);
        add_public_dependency("process", "concept_check", true);
        add_public_dependency("process", "config", true);
        add_public_dependency("process", "container", true);
        add_public_dependency("process", "container_hash", true);
        add_public_dependency("process", "context", true);
        add_public_dependency("process", "conversion", true);
        add_public_dependency("process", "core", true);
        add_public_dependency("process", "coroutine", true);
        add_public_dependency("process", "date_time", true);
        add_public_dependency("process", "detail", true);
        add_public_dependency("process", "exception", true);
        add_public_dependency("process", "filesystem", true);
        add_public_dependency("process", "function", true);
        add_public_dependency("process", "function_types", true);
        add_public_dependency("process", "fusion", true);
        add_public_dependency("process", "integer", true);
        add_public_dependency("process", "intrusive", true);
        add_public_dependency("process", "io", true);
        add_public_dependency("process", "iterator", true);
        add_public_dependency("process", "lambda", true);
        add_public_dependency("process", "lexical_cast", true);
        add_public_dependency("process", "math", true);
        add_public_dependency("process", "move", true);
        add_public_dependency("process", "mp11", true);
        add_public_dependency("process", "mpl", true);
        add_public_dependency("process", "numeric", true);
        add_public_dependency("process", "optional", true);
        add_public_dependency("process", "pool", true);
        add_public_dependency("process", "predef", true);
        add_public_dependency("process", "preprocessor", true);
        add_public_dependency("process", "range", true);
        add_public_dependency("process", "ratio", true);
        add_public_dependency("process", "rational", true);
        add_public_dependency("process", "regex", true);
        add_public_dependency("process", "smart_ptr", true);
        add_public_dependency("process", "static_assert", true);
        add_public_dependency("process", "system", true);
        add_public_dependency("process", "thread", true);
        add_public_dependency("process", "throw_exception", true);
        add_public_dependency("process", "tokenizer", true);
        add_public_dependency("process", "tuple", true);
        add_public_dependency("process", "type_index", true);
        add_public_dependency("process", "type_traits", true);
        add_public_dependency("process", "typeof", true);
        add_public_dependency("process", "unordered", true);
        add_public_dependency("process", "utility", true);
        add_public_dependency("process", "winapi", true);
        add_public_dependency("program_options", "any", true);
        add_public_dependency("program_options", "array", true);
        add_public_dependency("program_options", "assert", true);
        add_public_dependency("program_options", "atomic", true);
        add_public_dependency("program_options", "bind", true);
        add_public_dependency("program_options", "concept_check", true);
        add_public_dependency("program_options", "config", true);
        add_public_dependency("program_options", "container", true);
        add_public_dependency("program_options", "container_hash", true);
        add_public_dependency("program_options", "conversion", true);
        add_public_dependency("program_options", "core", true);
        add_public_dependency("program_options", "detail", true);
        add_public_dependency("program_options", "function", true);
        add_public_dependency("program_options", "function_types", true);
        add_public_dependency("program_options", "fusion", true);
        add_public_dependency("program_options", "integer", true);
        add_public_dependency("program_options", "intrusive", true);
        add_public_dependency("program_options", "io", true);
        add_public_dependency("program_options", "iterator", true);
        add_public_dependency("program_options", "lambda", true);
        add_public_dependency("program_options", "lexical_cast", true);
        add_public_dependency("program_options", "math", true);
        add_public_dependency("program_options", "move", true);
        add_public_dependency("program_options", "mp11", true);
        add_public_dependency("program_options", "mpl", true);
        add_public_dependency("program_options", "numeric", true);
        add_public_dependency("program_options", "optional", true);
        add_public_dependency("program_options", "predef", true);
        add_public_dependency("program_options", "preprocessor", true);
        add_public_dependency("program_options", "range", true);
        add_public_dependency("program_options", "regex", true);
        add_public_dependency("program_options", "smart_ptr", true);
        add_public_dependency("program_options", "static_assert", true);
        add_public_dependency("program_options", "throw_exception", true);
        add_public_dependency("program_options", "tokenizer", true);
        add_public_dependency("program_options", "tuple", true);
        add_public_dependency("program_options", "type_index", true);
        add_public_dependency("program_options", "type_traits", true);
        add_public_dependency("program_options", "typeof", true);
        add_public_dependency("program_options", "utility", true);
        add_public_dependency("program_options", "winapi", true);
        add_public_dependency("property_map", "algorithm", true);
        add_public_dependency("property_map", "any", true);
        add_public_dependency("property_map", "array", true);
        add_public_dependency("property_map", "assert", true);
        add_public_dependency("property_map", "atomic", true);
        add_public_dependency("property_map", "bimap", true);
        add_public_dependency("property_map", "bind", true);
        add_public_dependency("property_map", "chrono", true);
        add_public_dependency("property_map", "concept_check", true);
        add_public_dependency("property_map", "config", true);
        add_public_dependency("property_map", "container", true);
        add_public_dependency("property_map", "container_hash", true);
        add_public_dependency("property_map", "conversion", true);
        add_public_dependency("property_map", "core", true);
        add_public_dependency("property_map", "date_time", true);
        add_public_dependency("property_map", "detail", true);
        add_public_dependency("property_map", "dynamic_bitset", true);
        add_public_dependency("property_map", "endian", true);
        add_public_dependency("property_map", "exception", true);
        add_public_dependency("property_map", "foreach", true);
        add_public_dependency("property_map", "format", true);
        add_public_dependency("property_map", "function", true);
        add_public_dependency("property_map", "function_types", true);
        add_public_dependency("property_map", "fusion", true);
        add_public_dependency("property_map", "graph", true);
        add_public_dependency("property_map", "integer", true);
        add_public_dependency("property_map", "intrusive", true);
        add_public_dependency("property_map", "io", true);
        add_public_dependency("property_map", "iterator", true);
        add_public_dependency("property_map", "lambda", true);
        add_public_dependency("property_map", "lexical_cast", true);
        add_public_dependency("property_map", "math", true);
        add_public_dependency("property_map", "move", true);
        add_public_dependency("property_map", "mp11", true);
        add_public_dependency("property_map", "mpi", true);
        add_public_dependency("property_map", "mpl", true);
        add_public_dependency("property_map", "multi_index", true);
        add_public_dependency("property_map", "multiprecision", true);
        add_public_dependency("property_map", "numeric", true);
        add_public_dependency("property_map", "optional", true);
        add_public_dependency("property_map", "parameter", true);
        add_public_dependency("property_map", "phoenix", true);
        add_public_dependency("property_map", "pool", true);
        add_public_dependency("property_map", "predef", true);
        add_public_dependency("property_map", "preprocessor", true);
        add_public_dependency("property_map", "property_tree", true);
        add_public_dependency("property_map", "proto", true);
        add_public_dependency("property_map", "python", true);
        add_public_dependency("property_map", "random", true);
        add_public_dependency("property_map", "range", true);
        add_public_dependency("property_map", "ratio", true);
        add_public_dependency("property_map", "rational", true);
        add_public_dependency("property_map", "regex", true);
        add_public_dependency("property_map", "serialization", true);
        add_public_dependency("property_map", "smart_ptr", true);
        add_public_dependency("property_map", "spirit", true);
        add_public_dependency("property_map", "static_assert", true);
        add_public_dependency("property_map", "system", true);
        add_public_dependency("property_map", "test", true);
        add_public_dependency("property_map", "thread", true);
        add_public_dependency("property_map", "throw_exception", true);
        add_public_dependency("property_map", "tokenizer", true);
        add_public_dependency("property_map", "tti", true);
        add_public_dependency("property_map", "tuple", true);
        add_public_dependency("property_map", "type_index", true);
        add_public_dependency("property_map", "type_traits", true);
        add_public_dependency("property_map", "typeof", true);
        add_public_dependency("property_map", "unordered", true);
        add_public_dependency("property_map", "utility", true);
        add_public_dependency("property_map", "variant", true);
        add_public_dependency("property_map", "winapi", true);
        add_public_dependency("property_map", "xpressive", true);
        add_public_dependency("property_tree", "algorithm", true);
        add_public_dependency("property_tree", "any", true);
        add_public_dependency("property_tree", "array", true);
        add_public_dependency("property_tree", "assert", true);
        add_public_dependency("property_tree", "atomic", true);
        add_public_dependency("property_tree", "bind", true);
        add_public_dependency("property_tree", "chrono", true);
        add_public_dependency("property_tree", "concept_check", true);
        add_public_dependency("property_tree", "config", true);
        add_public_dependency("property_tree", "container", true);
        add_public_dependency("property_tree", "container_hash", true);
        add_public_dependency("property_tree", "conversion", true);
        add_public_dependency("property_tree", "core", true);
        add_public_dependency("property_tree", "date_time", true);
        add_public_dependency("property_tree", "detail", true);
        add_public_dependency("property_tree", "endian", true);
        add_public_dependency("property_tree", "exception", true);
        add_public_dependency("property_tree", "foreach", true);
        add_public_dependency("property_tree", "format", true);
        add_public_dependency("property_tree", "function", true);
        add_public_dependency("property_tree", "function_types", true);
        add_public_dependency("property_tree", "fusion", true);
        add_public_dependency("property_tree", "integer", true);
        add_public_dependency("property_tree", "intrusive", true);
        add_public_dependency("property_tree", "io", true);
        add_public_dependency("property_tree", "iterator", true);
        add_public_dependency("property_tree", "lambda", true);
        add_public_dependency("property_tree", "lexical_cast", true);
        add_public_dependency("property_tree", "math", true);
        add_public_dependency("property_tree", "move", true);
        add_public_dependency("property_tree", "mp11", true);
        add_public_dependency("property_tree", "mpl", true);
        add_public_dependency("property_tree", "multi_index", true);
        add_public_dependency("property_tree", "numeric", true);
        add_public_dependency("property_tree", "optional", true);
        add_public_dependency("property_tree", "phoenix", true);
        add_public_dependency("property_tree", "pool", true);
        add_public_dependency("property_tree", "predef", true);
        add_public_dependency("property_tree", "preprocessor", true);
        add_public_dependency("property_tree", "proto", true);
        add_public_dependency("property_tree", "range", true);
        add_public_dependency("property_tree", "ratio", true);
        add_public_dependency("property_tree", "rational", true);
        add_public_dependency("property_tree", "regex", true);
        add_public_dependency("property_tree", "serialization", true);
        add_public_dependency("property_tree", "smart_ptr", true);
        add_public_dependency("property_tree", "spirit", true);
        add_public_dependency("property_tree", "static_assert", true);
        add_public_dependency("property_tree", "system", true);
        add_public_dependency("property_tree", "thread", true);
        add_public_dependency("property_tree", "throw_exception", true);
        add_public_dependency("property_tree", "tokenizer", true);
        add_public_dependency("property_tree", "tuple", true);
        add_public_dependency("property_tree", "type_index", true);
        add_public_dependency("property_tree", "type_traits", true);
        add_public_dependency("property_tree", "typeof", true);
        add_public_dependency("property_tree", "unordered", true);
        add_public_dependency("property_tree", "utility", true);
        add_public_dependency("property_tree", "variant", true);
        add_public_dependency("property_tree", "winapi", true);
        add_public_dependency("proto", "array", true);
        add_public_dependency("proto", "assert", true);
        add_public_dependency("proto", "concept_check", true);
        add_public_dependency("proto", "config", true);
        add_public_dependency("proto", "container_hash", true);
        add_public_dependency("proto", "conversion", true);
        add_public_dependency("proto", "core", true);
        add_public_dependency("proto", "detail", true);
        add_public_dependency("proto", "function_types", true);
        add_public_dependency("proto", "fusion", true);
        add_public_dependency("proto", "integer", true);
        add_public_dependency("proto", "io", true);
        add_public_dependency("proto", "iterator", true);
        add_public_dependency("proto", "move", true);
        add_public_dependency("proto", "mpl", true);
        add_public_dependency("proto", "numeric", true);
        add_public_dependency("proto", "optional", true);
        add_public_dependency("proto", "predef", true);
        add_public_dependency("proto", "preprocessor", true);
        add_public_dependency("proto", "range", true);
        add_public_dependency("proto", "regex", true);
        add_public_dependency("proto", "smart_ptr", true);
        add_public_dependency("proto", "static_assert", true);
        add_public_dependency("proto", "throw_exception", true);
        add_public_dependency("proto", "tuple", true);
        add_public_dependency("proto", "type_traits", true);
        add_public_dependency("proto", "typeof", true);
        add_public_dependency("proto", "utility", true);
        add_public_dependency("ptr_container", "algorithm", true);
        add_public_dependency("ptr_container", "array", true);
        add_public_dependency("ptr_container", "assert", true);
        add_public_dependency("ptr_container", "atomic", true);
        add_public_dependency("ptr_container", "bind", true);
        add_public_dependency("ptr_container", "chrono", true);
        add_public_dependency("ptr_container", "circular_buffer", true);
        add_public_dependency("ptr_container", "concept_check", true);
        add_public_dependency("ptr_container", "config", true);
        add_public_dependency("ptr_container", "container", true);
        add_public_dependency("ptr_container", "container_hash", true);
        add_public_dependency("ptr_container", "conversion", true);
        add_public_dependency("ptr_container", "core", true);
        add_public_dependency("ptr_container", "date_time", true);
        add_public_dependency("ptr_container", "detail", true);
        add_public_dependency("ptr_container", "endian", true);
        add_public_dependency("ptr_container", "exception", true);
        add_public_dependency("ptr_container", "foreach", true);
        add_public_dependency("ptr_container", "function", true);
        add_public_dependency("ptr_container", "function_types", true);
        add_public_dependency("ptr_container", "fusion", true);
        add_public_dependency("ptr_container", "integer", true);
        add_public_dependency("ptr_container", "intrusive", true);
        add_public_dependency("ptr_container", "io", true);
        add_public_dependency("ptr_container", "iterator", true);
        add_public_dependency("ptr_container", "lambda", true);
        add_public_dependency("ptr_container", "lexical_cast", true);
        add_public_dependency("ptr_container", "math", true);
        add_public_dependency("ptr_container", "move", true);
        add_public_dependency("ptr_container", "mp11", true);
        add_public_dependency("ptr_container", "mpl", true);
        add_public_dependency("ptr_container", "numeric", true);
        add_public_dependency("ptr_container", "optional", true);
        add_public_dependency("ptr_container", "phoenix", true);
        add_public_dependency("ptr_container", "pool", true);
        add_public_dependency("ptr_container", "predef", true);
        add_public_dependency("ptr_container", "preprocessor", true);
        add_public_dependency("ptr_container", "proto", true);
        add_public_dependency("ptr_container", "range", true);
        add_public_dependency("ptr_container", "ratio", true);
        add_public_dependency("ptr_container", "rational", true);
        add_public_dependency("ptr_container", "regex", true);
        add_public_dependency("ptr_container", "serialization", true);
        add_public_dependency("ptr_container", "smart_ptr", true);
        add_public_dependency("ptr_container", "spirit", true);
        add_public_dependency("ptr_container", "static_assert", true);
        add_public_dependency("ptr_container", "system", true);
        add_public_dependency("ptr_container", "thread", true);
        add_public_dependency("ptr_container", "throw_exception", true);
        add_public_dependency("ptr_container", "tokenizer", true);
        add_public_dependency("ptr_container", "tuple", true);
        add_public_dependency("ptr_container", "type_index", true);
        add_public_dependency("ptr_container", "type_traits", true);
        add_public_dependency("ptr_container", "typeof", true);
        add_public_dependency("ptr_container", "unordered", true);
        add_public_dependency("ptr_container", "utility", true);
        add_public_dependency("ptr_container", "variant", true);
        add_public_dependency("ptr_container", "winapi", true);
        add_public_dependency("python", "algorithm", true);
        add_public_dependency("python", "any", true);
        add_public_dependency("python", "array", true);
        add_public_dependency("python", "assert", true);
        add_public_dependency("python", "atomic", true);
        add_public_dependency("python", "bimap", true);
        add_public_dependency("python", "bind", true);
        add_public_dependency("python", "chrono", true);
        add_public_dependency("python", "concept_check", true);
        add_public_dependency("python", "config", true);
        add_public_dependency("python", "container", true);
        add_public_dependency("python", "container_hash", true);
        add_public_dependency("python", "conversion", true);
        add_public_dependency("python", "core", true);
        add_public_dependency("python", "date_time", true);
        add_public_dependency("python", "detail", true);
        add_public_dependency("python", "dynamic_bitset", true);
        add_public_dependency("python", "endian", true);
        add_public_dependency("python", "exception", true);
        add_public_dependency("python", "foreach", true);
        add_public_dependency("python", "format", true);
        add_public_dependency("python", "function", true);
        add_public_dependency("python", "function_types", true);
        add_public_dependency("python", "fusion", true);
        add_public_dependency("python", "graph", true);
        add_public_dependency("python", "integer", true);
        add_public_dependency("python", "intrusive", true);
        add_public_dependency("python", "io", true);
        add_public_dependency("python", "iterator", true);
        add_public_dependency("python", "lambda", true);
        add_public_dependency("python", "lexical_cast", true);
        add_public_dependency("python", "math", true);
        add_public_dependency("python", "move", true);
        add_public_dependency("python", "mp11", true);
        add_public_dependency("python", "mpi", true);
        add_public_dependency("python", "mpl", true);
        add_public_dependency("python", "multi_index", true);
        add_public_dependency("python", "multiprecision", true);
        add_public_dependency("python", "numeric", true);
        add_public_dependency("python", "optional", true);
        add_public_dependency("python", "parameter", true);
        add_public_dependency("python", "phoenix", true);
        add_public_dependency("python", "pool", true);
        add_public_dependency("python", "predef", true);
        add_public_dependency("python", "preprocessor", true);
        add_public_dependency("python", "property_map", true);
        add_public_dependency("python", "property_tree", true);
        add_public_dependency("python", "proto", true);
        add_public_dependency("python", "random", true);
        add_public_dependency("python", "range", true);
        add_public_dependency("python", "ratio", true);
        add_public_dependency("python", "rational", true);
        add_public_dependency("python", "regex", true);
        add_public_dependency("python", "serialization", true);
        add_public_dependency("python", "smart_ptr", true);
        add_public_dependency("python", "spirit", true);
        add_public_dependency("python", "static_assert", true);
        add_public_dependency("python", "system", true);
        add_public_dependency("python", "test", true);
        add_public_dependency("python", "thread", true);
        add_public_dependency("python", "throw_exception", true);
        add_public_dependency("python", "tokenizer", true);
        add_public_dependency("python", "tti", true);
        add_public_dependency("python", "tuple", true);
        add_public_dependency("python", "type_index", true);
        add_public_dependency("python", "type_traits", true);
        add_public_dependency("python", "typeof", true);
        add_public_dependency("python", "unordered", true);
        add_public_dependency("python", "utility", true);
        add_public_dependency("python", "variant", true);
        add_public_dependency("python", "winapi", true);
        add_public_dependency("python", "xpressive", true);
        add_public_dependency("qvm", "assert", true);
        add_public_dependency("qvm", "config", true);
        add_public_dependency("qvm", "core", true);
        add_public_dependency("qvm", "exception", true);
        add_public_dependency("qvm", "move", true);
        add_public_dependency("qvm", "preprocessor", true);
        add_public_dependency("qvm", "smart_ptr", true);
        add_public_dependency("qvm", "static_assert", true);
        add_public_dependency("qvm", "throw_exception", true);
        add_public_dependency("qvm", "tuple", true);
        add_public_dependency("qvm", "type_traits", true);
        add_public_dependency("random", "system", false);
        add_public_dependency("random", "array", true);
        add_public_dependency("random", "assert", true);
        add_public_dependency("random", "atomic", true);
        add_public_dependency("random", "bind", true);
        add_public_dependency("random", "concept_check", true);
        add_public_dependency("random", "config", true);
        add_public_dependency("random", "container", true);
        add_public_dependency("random", "container_hash", true);
        add_public_dependency("random", "conversion", true);
        add_public_dependency("random", "core", true);
        add_public_dependency("random", "detail", true);
        add_public_dependency("random", "dynamic_bitset", true);
        add_public_dependency("random", "function_types", true);
        add_public_dependency("random", "fusion", true);
        add_public_dependency("random", "integer", true);
        add_public_dependency("random", "intrusive", true);
        add_public_dependency("random", "io", true);
        add_public_dependency("random", "iterator", true);
        add_public_dependency("random", "lambda", true);
        add_public_dependency("random", "lexical_cast", true);
        add_public_dependency("random", "math", true);
        add_public_dependency("random", "move", true);
        add_public_dependency("random", "mp11", true);
        add_public_dependency("random", "mpl", true);
        add_public_dependency("random", "multiprecision", true);
        add_public_dependency("random", "numeric", true);
        add_public_dependency("random", "optional", true);
        add_public_dependency("random", "predef", true);
        add_public_dependency("random", "preprocessor", true);
        add_public_dependency("random", "range", true);
        add_public_dependency("random", "rational", true);
        add_public_dependency("random", "regex", true);
        add_public_dependency("random", "smart_ptr", true);
        add_public_dependency("random", "static_assert", true);
        add_public_dependency("random", "throw_exception", true);
        add_public_dependency("random", "tuple", true);
        add_public_dependency("random", "type_traits", true);
        add_public_dependency("random", "typeof", true);
        add_public_dependency("random", "utility", true);
        add_public_dependency("random", "winapi", true);
        add_public_dependency("range", "array", true);
        add_public_dependency("range", "assert", true);
        add_public_dependency("range", "concept_check", true);
        add_public_dependency("range", "config", true);
        add_public_dependency("range", "container_hash", true);
        add_public_dependency("range", "conversion", true);
        add_public_dependency("range", "core", true);
        add_public_dependency("range", "detail", true);
        add_public_dependency("range", "function_types", true);
        add_public_dependency("range", "fusion", true);
        add_public_dependency("range", "integer", true);
        add_public_dependency("range", "io", true);
        add_public_dependency("range", "iterator", true);
        add_public_dependency("range", "move", true);
        add_public_dependency("range", "mpl", true);
        add_public_dependency("range", "numeric", true);
        add_public_dependency("range", "optional", true);
        add_public_dependency("range", "predef", true);
        add_public_dependency("range", "preprocessor", true);
        add_public_dependency("range", "regex", true);
        add_public_dependency("range", "smart_ptr", true);
        add_public_dependency("range", "static_assert", true);
        add_public_dependency("range", "throw_exception", true);
        add_public_dependency("range", "tuple", true);
        add_public_dependency("range", "type_traits", true);
        add_public_dependency("range", "typeof", true);
        add_public_dependency("range", "utility", true);
        add_public_dependency("ratio", "assert", true);
        add_public_dependency("ratio", "config", true);
        add_public_dependency("ratio", "container_hash", true);
        add_public_dependency("ratio", "core", true);
        add_public_dependency("ratio", "detail", true);
        add_public_dependency("ratio", "integer", true);
        add_public_dependency("ratio", "io", true);
        add_public_dependency("ratio", "mpl", true);
        add_public_dependency("ratio", "predef", true);
        add_public_dependency("ratio", "preprocessor", true);
        add_public_dependency("ratio", "rational", true);
        add_public_dependency("ratio", "static_assert", true);
        add_public_dependency("ratio", "throw_exception", true);
        add_public_dependency("ratio", "type_traits", true);
        add_public_dependency("ratio", "utility", true);
        add_public_dependency("rational", "assert", true);
        add_public_dependency("rational", "config", true);
        add_public_dependency("rational", "container_hash", true);
        add_public_dependency("rational", "core", true);
        add_public_dependency("rational", "detail", true);
        add_public_dependency("rational", "integer", true);
        add_public_dependency("rational", "io", true);
        add_public_dependency("rational", "preprocessor", true);
        add_public_dependency("rational", "static_assert", true);
        add_public_dependency("rational", "throw_exception", true);
        add_public_dependency("rational", "type_traits", true);
        add_public_dependency("rational", "utility", true);
        add_public_dependency("regex", "assert", true);
        add_public_dependency("regex", "concept_check", true);
        add_public_dependency("regex", "config", true);
        add_public_dependency("regex", "container_hash", true);
        add_public_dependency("regex", "conversion", true);
        add_public_dependency("regex", "core", true);
        add_public_dependency("regex", "detail", true);
        add_public_dependency("regex", "function_types", true);
        add_public_dependency("regex", "fusion", true);
        add_public_dependency("regex", "integer", true);
        add_public_dependency("regex", "io", true);
        add_public_dependency("regex", "iterator", true);
        add_public_dependency("regex", "move", true);
        add_public_dependency("regex", "mpl", true);
        add_public_dependency("regex", "optional", true);
        add_public_dependency("regex", "predef", true);
        add_public_dependency("regex", "preprocessor", true);
        add_public_dependency("regex", "smart_ptr", true);
        add_public_dependency("regex", "static_assert", true);
        add_public_dependency("regex", "throw_exception", true);
        add_public_dependency("regex", "tuple", true);
        add_public_dependency("regex", "type_traits", true);
        add_public_dependency("regex", "typeof", true);
        add_public_dependency("regex", "utility", true);
        add_public_dependency("safe_numerics", "assert", true);
        add_public_dependency("safe_numerics", "concept_check", true);
        add_public_dependency("safe_numerics", "config", true);
        add_public_dependency("safe_numerics", "core", true);
        add_public_dependency("safe_numerics", "integer", true);
        add_public_dependency("safe_numerics", "logic", true);
        add_public_dependency("safe_numerics", "mp11", true);
        add_public_dependency("safe_numerics", "preprocessor", true);
        add_public_dependency("safe_numerics", "static_assert", true);
        add_public_dependency("safe_numerics", "throw_exception", true);
        add_public_dependency("safe_numerics", "type_traits", true);
        add_public_dependency("scope_exit", "assert", true);
        add_public_dependency("scope_exit", "bind", true);
        add_public_dependency("scope_exit", "config", true);
        add_public_dependency("scope_exit", "container_hash", true);
        add_public_dependency("scope_exit", "core", true);
        add_public_dependency("scope_exit", "detail", true);
        add_public_dependency("scope_exit", "function", true);
        add_public_dependency("scope_exit", "integer", true);
        add_public_dependency("scope_exit", "move", true);
        add_public_dependency("scope_exit", "preprocessor", true);
        add_public_dependency("scope_exit", "smart_ptr", true);
        add_public_dependency("scope_exit", "static_assert", true);
        add_public_dependency("scope_exit", "throw_exception", true);
        add_public_dependency("scope_exit", "type_index", true);
        add_public_dependency("scope_exit", "type_traits", true);
        add_public_dependency("scope_exit", "typeof", true);
        add_public_dependency("serialization", "algorithm", true);
        add_public_dependency("serialization", "array", true);
        add_public_dependency("serialization", "assert", true);
        add_public_dependency("serialization", "atomic", true);
        add_public_dependency("serialization", "bind", true);
        add_public_dependency("serialization", "chrono", true);
        add_public_dependency("serialization", "concept_check", true);
        add_public_dependency("serialization", "config", true);
        add_public_dependency("serialization", "container", true);
        add_public_dependency("serialization", "container_hash", true);
        add_public_dependency("serialization", "conversion", true);
        add_public_dependency("serialization", "core", true);
        add_public_dependency("serialization", "date_time", true);
        add_public_dependency("serialization", "detail", true);
        add_public_dependency("serialization", "endian", true);
        add_public_dependency("serialization", "exception", true);
        add_public_dependency("serialization", "foreach", true);
        add_public_dependency("serialization", "function", true);
        add_public_dependency("serialization", "function_types", true);
        add_public_dependency("serialization", "fusion", true);
        add_public_dependency("serialization", "integer", true);
        add_public_dependency("serialization", "intrusive", true);
        add_public_dependency("serialization", "io", true);
        add_public_dependency("serialization", "iterator", true);
        add_public_dependency("serialization", "lambda", true);
        add_public_dependency("serialization", "lexical_cast", true);
        add_public_dependency("serialization", "math", true);
        add_public_dependency("serialization", "move", true);
        add_public_dependency("serialization", "mp11", true);
        add_public_dependency("serialization", "mpl", true);
        add_public_dependency("serialization", "numeric", true);
        add_public_dependency("serialization", "optional", true);
        add_public_dependency("serialization", "phoenix", true);
        add_public_dependency("serialization", "pool", true);
        add_public_dependency("serialization", "predef", true);
        add_public_dependency("serialization", "preprocessor", true);
        add_public_dependency("serialization", "proto", true);
        add_public_dependency("serialization", "range", true);
        add_public_dependency("serialization", "ratio", true);
        add_public_dependency("serialization", "rational", true);
        add_public_dependency("serialization", "regex", true);
        add_public_dependency("serialization", "smart_ptr", true);
        add_public_dependency("serialization", "spirit", true);
        add_public_dependency("serialization", "static_assert", true);
        add_public_dependency("serialization", "system", true);
        add_public_dependency("serialization", "thread", true);
        add_public_dependency("serialization", "throw_exception", true);
        add_public_dependency("serialization", "tokenizer", true);
        add_public_dependency("serialization", "tuple", true);
        add_public_dependency("serialization", "type_index", true);
        add_public_dependency("serialization", "type_traits", true);
        add_public_dependency("serialization", "typeof", true);
        add_public_dependency("serialization", "unordered", true);
        add_public_dependency("serialization", "utility", true);
        add_public_dependency("serialization", "variant", true);
        add_public_dependency("serialization", "winapi", true);
        add_public_dependency("signals2", "assert", true);
        add_public_dependency("signals2", "bind", true);
        add_public_dependency("signals2", "concept_check", true);
        add_public_dependency("signals2", "config", true);
        add_public_dependency("signals2", "container_hash", true);
        add_public_dependency("signals2", "conversion", true);
        add_public_dependency("signals2", "core", true);
        add_public_dependency("signals2", "detail", true);
        add_public_dependency("signals2", "function", true);
        add_public_dependency("signals2", "function_types", true);
        add_public_dependency("signals2", "fusion", true);
        add_public_dependency("signals2", "integer", true);
        add_public_dependency("signals2", "io", true);
        add_public_dependency("signals2", "iterator", true);
        add_public_dependency("signals2", "move", true);
        add_public_dependency("signals2", "mp11", true);
        add_public_dependency("signals2", "mpl", true);
        add_public_dependency("signals2", "optional", true);
        add_public_dependency("signals2", "parameter", true);
        add_public_dependency("signals2", "predef", true);
        add_public_dependency("signals2", "preprocessor", true);
        add_public_dependency("signals2", "smart_ptr", true);
        add_public_dependency("signals2", "static_assert", true);
        add_public_dependency("signals2", "throw_exception", true);
        add_public_dependency("signals2", "tuple", true);
        add_public_dependency("signals2", "type_index", true);
        add_public_dependency("signals2", "type_traits", true);
        add_public_dependency("signals2", "typeof", true);
        add_public_dependency("signals2", "utility", true);
        add_public_dependency("signals2", "variant", true);
        add_public_dependency("smart_ptr", "assert", true);
        add_public_dependency("smart_ptr", "config", true);
        add_public_dependency("smart_ptr", "core", true);
        add_public_dependency("smart_ptr", "move", true);
        add_public_dependency("smart_ptr", "preprocessor", true);
        add_public_dependency("smart_ptr", "static_assert", true);
        add_public_dependency("smart_ptr", "throw_exception", true);
        add_public_dependency("smart_ptr", "type_traits", true);
        add_public_dependency("sort", "array", true);
        add_public_dependency("sort", "assert", true);
        add_public_dependency("sort", "concept_check", true);
        add_public_dependency("sort", "config", true);
        add_public_dependency("sort", "container_hash", true);
        add_public_dependency("sort", "conversion", true);
        add_public_dependency("sort", "core", true);
        add_public_dependency("sort", "detail", true);
        add_public_dependency("sort", "function_types", true);
        add_public_dependency("sort", "fusion", true);
        add_public_dependency("sort", "integer", true);
        add_public_dependency("sort", "io", true);
        add_public_dependency("sort", "iterator", true);
        add_public_dependency("sort", "move", true);
        add_public_dependency("sort", "mpl", true);
        add_public_dependency("sort", "numeric", true);
        add_public_dependency("sort", "optional", true);
        add_public_dependency("sort", "predef", true);
        add_public_dependency("sort", "preprocessor", true);
        add_public_dependency("sort", "range", true);
        add_public_dependency("sort", "regex", true);
        add_public_dependency("sort", "smart_ptr", true);
        add_public_dependency("sort", "static_assert", true);
        add_public_dependency("sort", "throw_exception", true);
        add_public_dependency("sort", "tuple", true);
        add_public_dependency("sort", "type_traits", true);
        add_public_dependency("sort", "typeof", true);
        add_public_dependency("sort", "utility", true);
        add_public_dependency("spirit", "algorithm", true);
        add_public_dependency("spirit", "array", true);
        add_public_dependency("spirit", "assert", true);
        add_public_dependency("spirit", "atomic", true);
        add_public_dependency("spirit", "bind", true);
        add_public_dependency("spirit", "chrono", true);
        add_public_dependency("spirit", "concept_check", true);
        add_public_dependency("spirit", "config", true);
        add_public_dependency("spirit", "container", true);
        add_public_dependency("spirit", "container_hash", true);
        add_public_dependency("spirit", "conversion", true);
        add_public_dependency("spirit", "core", true);
        add_public_dependency("spirit", "date_time", true);
        add_public_dependency("spirit", "detail", true);
        add_public_dependency("spirit", "endian", true);
        add_public_dependency("spirit", "exception", true);
        add_public_dependency("spirit", "foreach", true);
        add_public_dependency("spirit", "function", true);
        add_public_dependency("spirit", "function_types", true);
        add_public_dependency("spirit", "fusion", true);
        add_public_dependency("spirit", "integer", true);
        add_public_dependency("spirit", "intrusive", true);
        add_public_dependency("spirit", "io", true);
        add_public_dependency("spirit", "iterator", true);
        add_public_dependency("spirit", "lambda", true);
        add_public_dependency("spirit", "lexical_cast", true);
        add_public_dependency("spirit", "math", true);
        add_public_dependency("spirit", "move", true);
        add_public_dependency("spirit", "mp11", true);
        add_public_dependency("spirit", "mpl", true);
        add_public_dependency("spirit", "numeric", true);
        add_public_dependency("spirit", "optional", true);
        add_public_dependency("spirit", "phoenix", true);
        add_public_dependency("spirit", "pool", true);
        add_public_dependency("spirit", "predef", true);
        add_public_dependency("spirit", "preprocessor", true);
        add_public_dependency("spirit", "proto", true);
        add_public_dependency("spirit", "range", true);
        add_public_dependency("spirit", "ratio", true);
        add_public_dependency("spirit", "rational", true);
        add_public_dependency("spirit", "regex", true);
        add_public_dependency("spirit", "smart_ptr", true);
        add_public_dependency("spirit", "static_assert", true);
        add_public_dependency("spirit", "system", true);
        add_public_dependency("spirit", "thread", true);
        add_public_dependency("spirit", "throw_exception", true);
        add_public_dependency("spirit", "tokenizer", true);
        add_public_dependency("spirit", "tuple", true);
        add_public_dependency("spirit", "type_index", true);
        add_public_dependency("spirit", "type_traits", true);
        add_public_dependency("spirit", "typeof", true);
        add_public_dependency("spirit", "unordered", true);
        add_public_dependency("spirit", "utility", true);
        add_public_dependency("spirit", "variant", true);
        add_public_dependency("spirit", "winapi", true);
        add_public_dependency("stacktrace", "array", true);
        add_public_dependency("stacktrace", "assert", true);
        add_public_dependency("stacktrace", "config", true);
        add_public_dependency("stacktrace", "container_hash", true);
        add_public_dependency("stacktrace", "core", true);
        add_public_dependency("stacktrace", "detail", true);
        add_public_dependency("stacktrace", "integer", true);
        add_public_dependency("stacktrace", "predef", true);
        add_public_dependency("stacktrace", "preprocessor", true);
        add_public_dependency("stacktrace", "static_assert", true);
        add_public_dependency("stacktrace", "throw_exception", true);
        add_public_dependency("stacktrace", "type_traits", true);
        add_public_dependency("stacktrace", "winapi", true);
        add_public_dependency("statechart", "algorithm", true);
        add_public_dependency("statechart", "array", true);
        add_public_dependency("statechart", "assert", true);
        add_public_dependency("statechart", "atomic", true);
        add_public_dependency("statechart", "bind", true);
        add_public_dependency("statechart", "chrono", true);
        add_public_dependency("statechart", "concept_check", true);
        add_public_dependency("statechart", "config", true);
        add_public_dependency("statechart", "container", true);
        add_public_dependency("statechart", "container_hash", true);
        add_public_dependency("statechart", "conversion", true);
        add_public_dependency("statechart", "core", true);
        add_public_dependency("statechart", "date_time", true);
        add_public_dependency("statechart", "detail", true);
        add_public_dependency("statechart", "exception", true);
        add_public_dependency("statechart", "function", true);
        add_public_dependency("statechart", "function_types", true);
        add_public_dependency("statechart", "fusion", true);
        add_public_dependency("statechart", "integer", true);
        add_public_dependency("statechart", "intrusive", true);
        add_public_dependency("statechart", "io", true);
        add_public_dependency("statechart", "iterator", true);
        add_public_dependency("statechart", "lambda", true);
        add_public_dependency("statechart", "lexical_cast", true);
        add_public_dependency("statechart", "math", true);
        add_public_dependency("statechart", "move", true);
        add_public_dependency("statechart", "mp11", true);
        add_public_dependency("statechart", "mpl", true);
        add_public_dependency("statechart", "numeric", true);
        add_public_dependency("statechart", "optional", true);
        add_public_dependency("statechart", "predef", true);
        add_public_dependency("statechart", "preprocessor", true);
        add_public_dependency("statechart", "range", true);
        add_public_dependency("statechart", "ratio", true);
        add_public_dependency("statechart", "rational", true);
        add_public_dependency("statechart", "regex", true);
        add_public_dependency("statechart", "smart_ptr", true);
        add_public_dependency("statechart", "static_assert", true);
        add_public_dependency("statechart", "system", true);
        add_public_dependency("statechart", "thread", true);
        add_public_dependency("statechart", "throw_exception", true);
        add_public_dependency("statechart", "tokenizer", true);
        add_public_dependency("statechart", "tuple", true);
        add_public_dependency("statechart", "type_index", true);
        add_public_dependency("statechart", "type_traits", true);
        add_public_dependency("statechart", "typeof", true);
        add_public_dependency("statechart", "unordered", true);
        add_public_dependency("statechart", "utility", true);
        add_public_dependency("statechart", "winapi", true);
        add_public_dependency("static_assert", "config", true);
        add_public_dependency("static_string", "assert", true);
        add_public_dependency("static_string", "config", true);
        add_public_dependency("static_string", "container_hash", true);
        add_public_dependency("static_string", "core", true);
        add_public_dependency("static_string", "detail", true);
        add_public_dependency("static_string", "integer", true);
        add_public_dependency("static_string", "io", true);
        add_public_dependency("static_string", "preprocessor", true);
        add_public_dependency("static_string", "static_assert", true);
        add_public_dependency("static_string", "throw_exception", true);
        add_public_dependency("static_string", "type_traits", true);
        add_public_dependency("static_string", "utility", true);
        add_public_dependency("stl_interfaces", "assert", true);
        add_public_dependency("stl_interfaces", "config", true);
        add_public_dependency("system", "config", true);
        add_public_dependency("system", "predef", true);
        add_public_dependency("system", "winapi", true);
        add_public_dependency("test", "algorithm", true);
        add_public_dependency("test", "array", true);
        add_public_dependency("test", "assert", true);
        add_public_dependency("test", "bind", true);
        add_public_dependency("test", "concept_check", true);
        add_public_dependency("test", "config", true);
        add_public_dependency("test", "container", true);
        add_public_dependency("test", "container_hash", true);
        add_public_dependency("test", "conversion", true);
        add_public_dependency("test", "core", true);
        add_public_dependency("test", "detail", true);
        add_public_dependency("test", "exception", true);
        add_public_dependency("test", "function", true);
        add_public_dependency("test", "function_types", true);
        add_public_dependency("test", "fusion", true);
        add_public_dependency("test", "integer", true);
        add_public_dependency("test", "intrusive", true);
        add_public_dependency("test", "io", true);
        add_public_dependency("test", "iterator", true);
        add_public_dependency("test", "move", true);
        add_public_dependency("test", "mpl", true);
        add_public_dependency("test", "numeric", true);
        add_public_dependency("test", "optional", true);
        add_public_dependency("test", "predef", true);
        add_public_dependency("test", "preprocessor", true);
        add_public_dependency("test", "range", true);
        add_public_dependency("test", "regex", true);
        add_public_dependency("test", "smart_ptr", true);
        add_public_dependency("test", "static_assert", true);
        add_public_dependency("test", "throw_exception", true);
        add_public_dependency("test", "tuple", true);
        add_public_dependency("test", "type_index", true);
        add_public_dependency("test", "type_traits", true);
        add_public_dependency("test", "typeof", true);
        add_public_dependency("test", "unordered", true);
        add_public_dependency("test", "utility", true);
        add_public_dependency("thread", "atomic", false);
        add_public_dependency("thread", "chrono", false);
        add_public_dependency("thread", "date_time", false);
        add_public_dependency("thread", "algorithm", true);
        add_public_dependency("thread", "array", true);
        add_public_dependency("thread", "assert", true);
        add_public_dependency("thread", "bind", true);
        add_public_dependency("thread", "concept_check", true);
        add_public_dependency("thread", "config", true);
        add_public_dependency("thread", "container", true);
        add_public_dependency("thread", "container_hash", true);
        add_public_dependency("thread", "conversion", true);
        add_public_dependency("thread", "core", true);
        add_public_dependency("thread", "detail", true);
        add_public_dependency("thread", "exception", true);
        add_public_dependency("thread", "function", true);
        add_public_dependency("thread", "function_types", true);
        add_public_dependency("thread", "fusion", true);
        add_public_dependency("thread", "integer", true);
        add_public_dependency("thread", "intrusive", true);
        add_public_dependency("thread", "io", true);
        add_public_dependency("thread", "iterator", true);
        add_public_dependency("thread", "lambda", true);
        add_public_dependency("thread", "lexical_cast", true);
        add_public_dependency("thread", "math", true);
        add_public_dependency("thread", "move", true);
        add_public_dependency("thread", "mp11", true);
        add_public_dependency("thread", "mpl", true);
        add_public_dependency("thread", "numeric", true);
        add_public_dependency("thread", "optional", true);
        add_public_dependency("thread", "predef", true);
        add_public_dependency("thread", "preprocessor", true);
        add_public_dependency("thread", "range", true);
        add_public_dependency("thread", "ratio", true);
        add_public_dependency("thread", "rational", true);
        add_public_dependency("thread", "regex", true);
        add_public_dependency("thread", "smart_ptr", true);
        add_public_dependency("thread", "static_assert", true);
        add_public_dependency("thread", "system", true);
        add_public_dependency("thread", "throw_exception", true);
        add_public_dependency("thread", "tokenizer", true);
        add_public_dependency("thread", "tuple", true);
        add_public_dependency("thread", "type_index", true);
        add_public_dependency("thread", "type_traits", true);
        add_public_dependency("thread", "typeof", true);
        add_public_dependency("thread", "unordered", true);
        add_public_dependency("thread", "utility", true);
        add_public_dependency("thread", "winapi", true);
        add_public_dependency("throw_exception", "assert", true);
        add_public_dependency("throw_exception", "config", true);
        add_public_dependency("timer", "chrono", false);
        add_public_dependency("timer", "assert", true);
        add_public_dependency("timer", "config", true);
        add_public_dependency("timer", "container_hash", true);
        add_public_dependency("timer", "core", true);
        add_public_dependency("timer", "detail", true);
        add_public_dependency("timer", "integer", true);
        add_public_dependency("timer", "io", true);
        add_public_dependency("timer", "move", true);
        add_public_dependency("timer", "mpl", true);
        add_public_dependency("timer", "predef", true);
        add_public_dependency("timer", "preprocessor", true);
        add_public_dependency("timer", "ratio", true);
        add_public_dependency("timer", "rational", true);
        add_public_dependency("timer", "static_assert", true);
        add_public_dependency("timer", "system", true);
        add_public_dependency("timer", "throw_exception", true);
        add_public_dependency("timer", "type_traits", true);
        add_public_dependency("timer", "typeof", true);
        add_public_dependency("timer", "utility", true);
        add_public_dependency("timer", "winapi", true);
        add_public_dependency("tokenizer", "assert", true);
        add_public_dependency("tokenizer", "concept_check", true);
        add_public_dependency("tokenizer", "config", true);
        add_public_dependency("tokenizer", "container_hash", true);
        add_public_dependency("tokenizer", "conversion", true);
        add_public_dependency("tokenizer", "core", true);
        add_public_dependency("tokenizer", "detail", true);
        add_public_dependency("tokenizer", "function_types", true);
        add_public_dependency("tokenizer", "fusion", true);
        add_public_dependency("tokenizer", "integer", true);
        add_public_dependency("tokenizer", "io", true);
        add_public_dependency("tokenizer", "iterator", true);
        add_public_dependency("tokenizer", "move", true);
        add_public_dependency("tokenizer", "mpl", true);
        add_public_dependency("tokenizer", "optional", true);
        add_public_dependency("tokenizer", "predef", true);
        add_public_dependency("tokenizer", "preprocessor", true);
        add_public_dependency("tokenizer", "smart_ptr", true);
        add_public_dependency("tokenizer", "static_assert", true);
        add_public_dependency("tokenizer", "throw_exception", true);
        add_public_dependency("tokenizer", "tuple", true);
        add_public_dependency("tokenizer", "type_traits", true);
        add_public_dependency("tokenizer", "typeof", true);
        add_public_dependency("tokenizer", "utility", true);
        add_public_dependency("tti", "assert", true);
        add_public_dependency("tti", "config", true);
        add_public_dependency("tti", "container_hash", true);
        add_public_dependency("tti", "core", true);
        add_public_dependency("tti", "detail", true);
        add_public_dependency("tti", "function_types", true);
        add_public_dependency("tti", "integer", true);
        add_public_dependency("tti", "io", true);
        add_public_dependency("tti", "mpl", true);
        add_public_dependency("tti", "predef", true);
        add_public_dependency("tti", "preprocessor", true);
        add_public_dependency("tti", "static_assert", true);
        add_public_dependency("tti", "throw_exception", true);
        add_public_dependency("tti", "type_traits", true);
        add_public_dependency("tti", "utility", true);
        add_public_dependency("tuple", "assert", true);
        add_public_dependency("tuple", "config", true);
        add_public_dependency("tuple", "core", true);
        add_public_dependency("tuple", "preprocessor", true);
        add_public_dependency("tuple", "static_assert", true);
        add_public_dependency("tuple", "type_traits", true);
        add_public_dependency("type_erasure", "system", false);
        add_public_dependency("type_erasure", "thread", false);
        add_public_dependency("type_erasure", "algorithm", true);
        add_public_dependency("type_erasure", "array", true);
        add_public_dependency("type_erasure", "assert", true);
        add_public_dependency("type_erasure", "atomic", true);
        add_public_dependency("type_erasure", "bind", true);
        add_public_dependency("type_erasure", "chrono", true);
        add_public_dependency("type_erasure", "concept_check", true);
        add_public_dependency("type_erasure", "config", true);
        add_public_dependency("type_erasure", "container", true);
        add_public_dependency("type_erasure", "container_hash", true);
        add_public_dependency("type_erasure", "conversion", true);
        add_public_dependency("type_erasure", "core", true);
        add_public_dependency("type_erasure", "date_time", true);
        add_public_dependency("type_erasure", "detail", true);
        add_public_dependency("type_erasure", "exception", true);
        add_public_dependency("type_erasure", "function", true);
        add_public_dependency("type_erasure", "function_types", true);
        add_public_dependency("type_erasure", "fusion", true);
        add_public_dependency("type_erasure", "integer", true);
        add_public_dependency("type_erasure", "intrusive", true);
        add_public_dependency("type_erasure", "io", true);
        add_public_dependency("type_erasure", "iterator", true);
        add_public_dependency("type_erasure", "lambda", true);
        add_public_dependency("type_erasure", "lexical_cast", true);
        add_public_dependency("type_erasure", "math", true);
        add_public_dependency("type_erasure", "move", true);
        add_public_dependency("type_erasure", "mp11", true);
        add_public_dependency("type_erasure", "mpl", true);
        add_public_dependency("type_erasure", "numeric", true);
        add_public_dependency("type_erasure", "optional", true);
        add_public_dependency("type_erasure", "predef", true);
        add_public_dependency("type_erasure", "preprocessor", true);
        add_public_dependency("type_erasure", "range", true);
        add_public_dependency("type_erasure", "ratio", true);
        add_public_dependency("type_erasure", "rational", true);
        add_public_dependency("type_erasure", "regex", true);
        add_public_dependency("type_erasure", "smart_ptr", true);
        add_public_dependency("type_erasure", "static_assert", true);
        add_public_dependency("type_erasure", "throw_exception", true);
        add_public_dependency("type_erasure", "tokenizer", true);
        add_public_dependency("type_erasure", "tuple", true);
        add_public_dependency("type_erasure", "type_index", true);
        add_public_dependency("type_erasure", "type_traits", true);
        add_public_dependency("type_erasure", "typeof", true);
        add_public_dependency("type_erasure", "unordered", true);
        add_public_dependency("type_erasure", "utility", true);
        add_public_dependency("type_erasure", "vmd", true);
        add_public_dependency("type_erasure", "winapi", true);
        add_public_dependency("type_index", "assert", true);
        add_public_dependency("type_index", "config", true);
        add_public_dependency("type_index", "container_hash", true);
        add_public_dependency("type_index", "core", true);
        add_public_dependency("type_index", "detail", true);
        add_public_dependency("type_index", "integer", true);
        add_public_dependency("type_index", "move", true);
        add_public_dependency("type_index", "preprocessor", true);
        add_public_dependency("type_index", "smart_ptr", true);
        add_public_dependency("type_index", "static_assert", true);
        add_public_dependency("type_index", "throw_exception", true);
        add_public_dependency("type_index", "type_traits", true);
        add_public_dependency("type_traits", "config", true);
        add_public_dependency("type_traits", "preprocessor", true);
        add_public_dependency("type_traits", "static_assert", true);
        add_public_dependency("typeof", "config", true);
        add_public_dependency("typeof", "preprocessor", true);
        add_public_dependency("typeof", "static_assert", true);
        add_public_dependency("typeof", "type_traits", true);
        add_public_dependency("units", "algorithm", true);
        add_public_dependency("units", "array", true);
        add_public_dependency("units", "assert", true);
        add_public_dependency("units", "atomic", true);
        add_public_dependency("units", "bind", true);
        add_public_dependency("units", "chrono", true);
        add_public_dependency("units", "concept_check", true);
        add_public_dependency("units", "config", true);
        add_public_dependency("units", "container", true);
        add_public_dependency("units", "container_hash", true);
        add_public_dependency("units", "conversion", true);
        add_public_dependency("units", "core", true);
        add_public_dependency("units", "date_time", true);
        add_public_dependency("units", "detail", true);
        add_public_dependency("units", "endian", true);
        add_public_dependency("units", "exception", true);
        add_public_dependency("units", "foreach", true);
        add_public_dependency("units", "function", true);
        add_public_dependency("units", "function_types", true);
        add_public_dependency("units", "fusion", true);
        add_public_dependency("units", "integer", true);
        add_public_dependency("units", "intrusive", true);
        add_public_dependency("units", "io", true);
        add_public_dependency("units", "iterator", true);
        add_public_dependency("units", "lambda", true);
        add_public_dependency("units", "lexical_cast", true);
        add_public_dependency("units", "math", true);
        add_public_dependency("units", "move", true);
        add_public_dependency("units", "mp11", true);
        add_public_dependency("units", "mpl", true);
        add_public_dependency("units", "numeric", true);
        add_public_dependency("units", "optional", true);
        add_public_dependency("units", "phoenix", true);
        add_public_dependency("units", "pool", true);
        add_public_dependency("units", "predef", true);
        add_public_dependency("units", "preprocessor", true);
        add_public_dependency("units", "proto", true);
        add_public_dependency("units", "range", true);
        add_public_dependency("units", "ratio", true);
        add_public_dependency("units", "rational", true);
        add_public_dependency("units", "regex", true);
        add_public_dependency("units", "serialization", true);
        add_public_dependency("units", "smart_ptr", true);
        add_public_dependency("units", "spirit", true);
        add_public_dependency("units", "static_assert", true);
        add_public_dependency("units", "system", true);
        add_public_dependency("units", "thread", true);
        add_public_dependency("units", "throw_exception", true);
        add_public_dependency("units", "tokenizer", true);
        add_public_dependency("units", "tuple", true);
        add_public_dependency("units", "type_index", true);
        add_public_dependency("units", "type_traits", true);
        add_public_dependency("units", "typeof", true);
        add_public_dependency("units", "unordered", true);
        add_public_dependency("units", "utility", true);
        add_public_dependency("units", "variant", true);
        add_public_dependency("units", "winapi", true);
        add_public_dependency("unordered", "assert", true);
        add_public_dependency("unordered", "config", true);
        add_public_dependency("unordered", "container", true);
        add_public_dependency("unordered", "container_hash", true);
        add_public_dependency("unordered", "core", true);
        add_public_dependency("unordered", "detail", true);
        add_public_dependency("unordered", "integer", true);
        add_public_dependency("unordered", "intrusive", true);
        add_public_dependency("unordered", "move", true);
        add_public_dependency("unordered", "predef", true);
        add_public_dependency("unordered", "preprocessor", true);
        add_public_dependency("unordered", "smart_ptr", true);
        add_public_dependency("unordered", "static_assert", true);
        add_public_dependency("unordered", "throw_exception", true);
        add_public_dependency("unordered", "tuple", true);
        add_public_dependency("unordered", "type_traits", true);
        add_public_dependency("utility", "assert", true);
        add_public_dependency("utility", "config", true);
        add_public_dependency("utility", "container_hash", true);
        add_public_dependency("utility", "core", true);
        add_public_dependency("utility", "detail", true);
        add_public_dependency("utility", "integer", true);
        add_public_dependency("utility", "io", true);
        add_public_dependency("utility", "preprocessor", true);
        add_public_dependency("utility", "static_assert", true);
        add_public_dependency("utility", "throw_exception", true);
        add_public_dependency("utility", "type_traits", true);
        add_public_dependency("uuid", "algorithm", true);
        add_public_dependency("uuid", "array", true);
        add_public_dependency("uuid", "assert", true);
        add_public_dependency("uuid", "atomic", true);
        add_public_dependency("uuid", "bind", true);
        add_public_dependency("uuid", "chrono", true);
        add_public_dependency("uuid", "concept_check", true);
        add_public_dependency("uuid", "config", true);
        add_public_dependency("uuid", "container", true);
        add_public_dependency("uuid", "container_hash", true);
        add_public_dependency("uuid", "conversion", true);
        add_public_dependency("uuid", "core", true);
        add_public_dependency("uuid", "date_time", true);
        add_public_dependency("uuid", "detail", true);
        add_public_dependency("uuid", "dynamic_bitset", true);
        add_public_dependency("uuid", "endian", true);
        add_public_dependency("uuid", "exception", true);
        add_public_dependency("uuid", "foreach", true);
        add_public_dependency("uuid", "function", true);
        add_public_dependency("uuid", "function_types", true);
        add_public_dependency("uuid", "fusion", true);
        add_public_dependency("uuid", "integer", true);
        add_public_dependency("uuid", "intrusive", true);
        add_public_dependency("uuid", "io", true);
        add_public_dependency("uuid", "iterator", true);
        add_public_dependency("uuid", "lambda", true);
        add_public_dependency("uuid", "lexical_cast", true);
        add_public_dependency("uuid", "math", true);
        add_public_dependency("uuid", "move", true);
        add_public_dependency("uuid", "mp11", true);
        add_public_dependency("uuid", "mpl", true);
        add_public_dependency("uuid", "multiprecision", true);
        add_public_dependency("uuid", "numeric", true);
        add_public_dependency("uuid", "optional", true);
        add_public_dependency("uuid", "phoenix", true);
        add_public_dependency("uuid", "pool", true);
        add_public_dependency("uuid", "predef", true);
        add_public_dependency("uuid", "preprocessor", true);
        add_public_dependency("uuid", "proto", true);
        add_public_dependency("uuid", "random", true);
        add_public_dependency("uuid", "range", true);
        add_public_dependency("uuid", "ratio", true);
        add_public_dependency("uuid", "rational", true);
        add_public_dependency("uuid", "regex", true);
        add_public_dependency("uuid", "serialization", true);
        add_public_dependency("uuid", "smart_ptr", true);
        add_public_dependency("uuid", "spirit", true);
        add_public_dependency("uuid", "static_assert", true);
        add_public_dependency("uuid", "system", true);
        add_public_dependency("uuid", "thread", true);
        add_public_dependency("uuid", "throw_exception", true);
        add_public_dependency("uuid", "tokenizer", true);
        add_public_dependency("uuid", "tti", true);
        add_public_dependency("uuid", "tuple", true);
        add_public_dependency("uuid", "type_index", true);
        add_public_dependency("uuid", "type_traits", true);
        add_public_dependency("uuid", "typeof", true);
        add_public_dependency("uuid", "unordered", true);
        add_public_dependency("uuid", "utility", true);
        add_public_dependency("uuid", "variant", true);
        add_public_dependency("uuid", "winapi", true);
        add_public_dependency("variant", "assert", true);
        add_public_dependency("variant", "bind", true);
        add_public_dependency("variant", "config", true);
        add_public_dependency("variant", "container_hash", true);
        add_public_dependency("variant", "core", true);
        add_public_dependency("variant", "detail", true);
        add_public_dependency("variant", "integer", true);
        add_public_dependency("variant", "io", true);
        add_public_dependency("variant", "move", true);
        add_public_dependency("variant", "mpl", true);
        add_public_dependency("variant", "predef", true);
        add_public_dependency("variant", "preprocessor", true);
        add_public_dependency("variant", "smart_ptr", true);
        add_public_dependency("variant", "static_assert", true);
        add_public_dependency("variant", "throw_exception", true);
        add_public_dependency("variant", "type_index", true);
        add_public_dependency("variant", "type_traits", true);
        add_public_dependency("variant", "utility", true);
        add_public_dependency("variant2", "config", true);
        add_public_dependency("variant2", "mp11", true);
        add_public_dependency("vmd", "preprocessor", true);
        add_public_dependency("wave", "filesystem", false);
        add_public_dependency("wave", "thread", false);
        add_public_dependency("wave", "algorithm", true);
        add_public_dependency("wave", "array", true);
        add_public_dependency("wave", "assert", true);
        add_public_dependency("wave", "atomic", true);
        add_public_dependency("wave", "bind", true);
        add_public_dependency("wave", "chrono", true);
        add_public_dependency("wave", "concept_check", true);
        add_public_dependency("wave", "config", true);
        add_public_dependency("wave", "container", true);
        add_public_dependency("wave", "container_hash", true);
        add_public_dependency("wave", "conversion", true);
        add_public_dependency("wave", "core", true);
        add_public_dependency("wave", "date_time", true);
        add_public_dependency("wave", "detail", true);
        add_public_dependency("wave", "endian", true);
        add_public_dependency("wave", "exception", true);
        add_public_dependency("wave", "foreach", true);
        add_public_dependency("wave", "function", true);
        add_public_dependency("wave", "function_types", true);
        add_public_dependency("wave", "fusion", true);
        add_public_dependency("wave", "integer", true);
        add_public_dependency("wave", "intrusive", true);
        add_public_dependency("wave", "io", true);
        add_public_dependency("wave", "iterator", true);
        add_public_dependency("wave", "lambda", true);
        add_public_dependency("wave", "lexical_cast", true);
        add_public_dependency("wave", "math", true);
        add_public_dependency("wave", "move", true);
        add_public_dependency("wave", "mp11", true);
        add_public_dependency("wave", "mpl", true);
        add_public_dependency("wave", "multi_index", true);
        add_public_dependency("wave", "numeric", true);
        add_public_dependency("wave", "optional", true);
        add_public_dependency("wave", "phoenix", true);
        add_public_dependency("wave", "pool", true);
        add_public_dependency("wave", "predef", true);
        add_public_dependency("wave", "preprocessor", true);
        add_public_dependency("wave", "proto", true);
        add_public_dependency("wave", "range", true);
        add_public_dependency("wave", "ratio", true);
        add_public_dependency("wave", "rational", true);
        add_public_dependency("wave", "regex", true);
        add_public_dependency("wave", "serialization", true);
        add_public_dependency("wave", "smart_ptr", true);
        add_public_dependency("wave", "spirit", true);
        add_public_dependency("wave", "static_assert", true);
        add_public_dependency("wave", "system", true);
        add_public_dependency("wave", "throw_exception", true);
        add_public_dependency("wave", "tokenizer", true);
        add_public_dependency("wave", "tuple", true);
        add_public_dependency("wave", "type_index", true);
        add_public_dependency("wave", "type_traits", true);
        add_public_dependency("wave", "typeof", true);
        add_public_dependency("wave", "unordered", true);
        add_public_dependency("wave", "utility", true);
        add_public_dependency("wave", "variant", true);
        add_public_dependency("wave", "winapi", true);
        add_public_dependency("winapi", "config", true);
        add_public_dependency("winapi", "predef", true);
        add_public_dependency("xpressive", "array", true);
        add_public_dependency("xpressive", "assert", true);
        add_public_dependency("xpressive", "atomic", true);
        add_public_dependency("xpressive", "bind", true);
        add_public_dependency("xpressive", "concept_check", true);
        add_public_dependency("xpressive", "config", true);
        add_public_dependency("xpressive", "container", true);
        add_public_dependency("xpressive", "container_hash", true);
        add_public_dependency("xpressive", "conversion", true);
        add_public_dependency("xpressive", "core", true);
        add_public_dependency("xpressive", "detail", true);
        add_public_dependency("xpressive", "exception", true);
        add_public_dependency("xpressive", "function_types", true);
        add_public_dependency("xpressive", "fusion", true);
        add_public_dependency("xpressive", "integer", true);
        add_public_dependency("xpressive", "intrusive", true);
        add_public_dependency("xpressive", "io", true);
        add_public_dependency("xpressive", "iterator", true);
        add_public_dependency("xpressive", "lambda", true);
        add_public_dependency("xpressive", "lexical_cast", true);
        add_public_dependency("xpressive", "math", true);
        add_public_dependency("xpressive", "move", true);
        add_public_dependency("xpressive", "mp11", true);
        add_public_dependency("xpressive", "mpl", true);
        add_public_dependency("xpressive", "numeric", true);
        add_public_dependency("xpressive", "optional", true);
        add_public_dependency("xpressive", "predef", true);
        add_public_dependency("xpressive", "preprocessor", true);
        add_public_dependency("xpressive", "proto", true);
        add_public_dependency("xpressive", "range", true);
        add_public_dependency("xpressive", "regex", true);
        add_public_dependency("xpressive", "smart_ptr", true);
        add_public_dependency("xpressive", "static_assert", true);
        add_public_dependency("xpressive", "throw_exception", true);
        add_public_dependency("xpressive", "tuple", true);
        add_public_dependency("xpressive", "type_traits", true);
        add_public_dependency("xpressive", "typeof", true);
        add_public_dependency("xpressive", "utility", true);
        add_public_dependency("xpressive", "winapi", true);
        add_public_dependency("yap", "assert", true);
        add_public_dependency("yap", "config", true);
        add_public_dependency("yap", "container_hash", true);
        add_public_dependency("yap", "core", true);
        add_public_dependency("yap", "detail", true);
        add_public_dependency("yap", "function_types", true);
        add_public_dependency("yap", "fusion", true);
        add_public_dependency("yap", "hana", true);
        add_public_dependency("yap", "integer", true);
        add_public_dependency("yap", "io", true);
        add_public_dependency("yap", "move", true);
        add_public_dependency("yap", "mpl", true);
        add_public_dependency("yap", "predef", true);
        add_public_dependency("yap", "preprocessor", true);
        add_public_dependency("yap", "smart_ptr", true);
        add_public_dependency("yap", "static_assert", true);
        add_public_dependency("yap", "throw_exception", true);
        add_public_dependency("yap", "tuple", true);
        add_public_dependency("yap", "type_index", true);
        add_public_dependency("yap", "type_traits", true);
        add_public_dependency("yap", "typeof", true);
        add_public_dependency("yap", "utility", true);
    }
}
