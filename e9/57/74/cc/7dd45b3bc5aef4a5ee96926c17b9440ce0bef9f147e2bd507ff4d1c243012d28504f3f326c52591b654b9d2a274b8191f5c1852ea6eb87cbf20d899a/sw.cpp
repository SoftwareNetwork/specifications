void build(Solution &s)
{
    auto &flatbuffers = s.addTarget<StaticLibraryTarget>("google.flatbuffers", "22.12.06");
    flatbuffers += Git("https://github.com/google/flatbuffers", "v{M}.{m:02}.{p:02}");
    {
        flatbuffers +=
            "include/.*"_rr,
            "src/code_generators.cpp",
            "src/idl_gen_fbs.cpp",
            "src/idl_gen_text.cpp",
            "src/idl_parser.cpp",
            "src/reflection.cpp",
            "src/util.cpp";
    }

    auto &flatc = flatbuffers.addTarget<ExecutableTarget>("flatc");
    {
        flatc +=
            "grpc/src/compiler/.*"_rr,
            "src/.*"_rr;
        flatc -= "src/flathash.cpp";
        flatc.Public +=
            "grpc"_id;
        (flatc + flatbuffers)->IncludeDirectoriesOnly = true;
    }
}
