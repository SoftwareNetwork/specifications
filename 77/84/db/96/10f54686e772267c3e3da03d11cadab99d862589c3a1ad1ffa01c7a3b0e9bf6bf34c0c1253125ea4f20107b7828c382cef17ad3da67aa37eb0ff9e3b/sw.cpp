#pragma sw header on

struct ProtocData
{
    path input;
    path outdir;
    String generator;
    Strings exts;
    FilesOrdered idirs;
    DependencyPtr plugin;

    void addIncludeDirectory(const path &p) { idirs.push_back(p); }

    String getHash() const
    {
        size_t h = 0;
        hash_combine(h, input);
        hash_combine(h, outdir);
        hash_combine(h, generator);
        hash_combine(h, exts);
        hash_combine(h, idirs);
        if (plugin)
#if SW_CPP_DRIVER_API_VERSION > 1
            hash_combine(h, plugin->getUnresolvedPackageId().getName().toString());
#else
            hash_combine(h, plugin->getPackage().toString());
#endif
        return std::to_string(h);
    }

    auto generate(const DependencyPtr &protoc, NativeExecutedTarget &t)
    {
        if (idirs.empty())
        {
            addIncludeDirectory(t.SourceDir);
            //throw SW_RUNTIME_ERROR("protoc: empty include dirs, provide atleast one");
        }
        path rel_input = input;
        if (input.is_absolute())
        {
            for (auto &i : idirs)
            {
                if (is_under_root(input, i))
                {
                    rel_input = input.lexically_relative(i);
                    break;
                }
            }
        }
        if (generator.empty())
            throw SW_LOGIC_ERROR("protoc: empty generator");
        if (exts.empty())
            throw SW_LOGIC_ERROR("protoc: empty exts");
        if (outdir.empty())
            outdir = t.BinaryDir;

        // append protoc idir
        addIncludeDirectory(t.getFile(protoc, "src"));

        auto deps_file = t.BinaryDir.parent_path() / "obj" / (input.filename() += "." + getHash() + ".d");
#if SW_CPP_DRIVER_API_VERSION > 1
        auto gc = std::make_shared<::sw::driver::Command>();
#else
        auto gc = std::make_shared<::sw::driver::Command>(t.getMainBuild());
#endif
        gc->deps_processor = Command::DepsProcessor::Gnu;
        gc->deps_file = deps_file;

        auto c = t.addCommand(gc);
        c << cmd::prog(protoc);
        c << cmd::wdir(outdir);

        // deps file
        c << ("--dependency_out=" + to_string(normalize_path(deps_file)));

        // idirs first
        for (auto &i : idirs)
            c << "-I" << normalize_path(i);

        // generator
        c << ("--" + generator + "_out=" + to_string(normalize_path(outdir)));

        // plugin
        if (plugin)
        {
#if SW_CPP_DRIVER_API_VERSION > 1
            c << [c = c.getCommand().get(), plugin = plugin, generator = generator, &ctx = t.getSolution().getContext()]()
#else
            c << [c = c.getCommand().get(), plugin = plugin, generator = generator, &ctx = t.getContext()]()
#endif
            {
                path p;
#if SW_CPP_DRIVER_API_VERSION > 1
                p = plugin->transform->get_properties()["output_file"].getPathValue(ctx.getLocalStorage());
#else
                if (auto t = plugin->getTarget().as<NativeExecutedTarget *>())
                {
                    p = t->getOutputFile();
                }
                else if (auto t = plugin->getTarget().as<PredefinedTarget *>())
                {
                    p = t->getInterfaceSettings()["output_file"].getPathValue(ctx.getLocalStorage());
                }
                else
                    throw SW_RUNTIME_ERROR("no grpc_cpp_plugin resolved (missing target code)");
#endif
                c->addInput(p);
                return "--plugin=protoc-gen-" + generator + "=" + to_string(normalize_path(p));
            }
            ;

            t.addDummyDependency(plugin);
            std::dynamic_pointer_cast<::sw::driver::Command>(c.getCommand())->addProgramDependency(plugin);
        }

        // input
        c << cmd::in(normalize_path(input)); // must be normalized
        c << cmd::end();

        // create deps output dir!
        c->output_dirs.insert(deps_file.parent_path());

        auto o = outdir / rel_input.parent_path() / rel_input.stem();
        for (auto &e : exts)
            c << cmd::out(path(o) += e);
        return c.getCommand();
    }
};

struct ProtobufData : ProtocData
{
    bool public_protobuf = false;
};

static auto gen_protobuf_cpp(const DependencyPtr &protobuf_base, NativeExecutedTarget &t,
    const path &f, const ProtobufData &data = {})
{
#if SW_CPP_DRIVER_API_VERSION > 1
    auto protoc = std::make_shared<Dependency>(sw::UnresolvedPackageName{
        protobuf_base->getUnresolvedPackageId().getName().getPath() / "protoc",
        protobuf_base->getUnresolvedPackageId().getName().getRange()});
    auto protobuf = std::make_shared<Dependency>(sw::UnresolvedPackageName{
        protobuf_base->getUnresolvedPackageId().getName().getPath() / "protobuf",
        protobuf_base->getUnresolvedPackageId().getName().getRange()});
#else
    auto protoc = std::make_shared<Dependency>(sw::UnresolvedPackage{protobuf_base->package.getPath() / "protoc", protobuf_base->package.getRange()});
    auto protobuf = std::make_shared<Dependency>(sw::UnresolvedPackage{protobuf_base->package.getPath() / "protobuf", protobuf_base->package.getRange()});
#endif

    ProtocData d = data;
    d.input = f;
    d.generator = "cpp";
    d.exts = {".pb.cc", ".pb.h"};
    auto c = d.generate(protoc, t);

    t += protobuf;
    if (data.public_protobuf)
        t.Public += protobuf;

    return std::tuple{ protoc, c };
}

#pragma sw header off

void build(Solution &s)
{
    auto &p = s.addProject("google.protobuf", "33.5.0");
    p += Git("https://github.com/protocolbuffers/protobuf", "v{M}.{m}");

    auto setup = [](auto &t)
    {
        t.patch("BUILD", "COPIED_WELL_KNOWN_PROTOS =", "#COPIED_WELL_KNOWN_PROTOS  =");
    };
    auto win_or_mingw = [](auto &t)
    {
        return
            t.getBuildSettings().TargetOS.Type == OSType::Windows/* ||
                                                                 t.getBuildSettings().TargetOS.Type == OSType::Mingw*/
            ;
    };

    auto cppstd = cpp20;

    auto &utf8_range = p.addStaticLibrary("utf8_range");
    {
        utf8_range += cppstd;
        utf8_range.setRootDirectory("third_party/utf8_range");
        utf8_range +=
            //"naive.c",
            //"range2-neon.c",
            //"range2-sse.c",
            "utf8_range.c",
            "utf8_range.h",
            "utf8_range_.*inc"_r
            ;

        auto &utf8_validity = p.addStaticLibrary("utf8_validity");
        utf8_validity.setRootDirectory("third_party/utf8_range");
        utf8_validity += cppstd;
        utf8_validity +=
            //"utf8_validity.cc",
            "utf8_validity.h"
            ;
        utf8_validity.Public += "org.sw.demo.google.abseil"_dep;
        //utf8_validity.CompileOptions.push_back("/arch:AVX");
    }

    auto &upb = p.addStaticLibrary("upb");
    //upb.ExportAllSymbols = true;
    {
        //upb += "cmake/google/.*"_rr;
        upb += "upb/.*"_rr;
        /*upb += "upb/cmake"_idir;
        if (!upb.DryRun && !fs::exists(upb.BinaryDir / "upb")) {
            fs::create_directory_symlink(upb.SourceDir, upb.BinaryDir / "upb");
        }*/
        //upb -= "upb/reflection/.*"_rr;
        //upb -= "upb/reflection/cmake/.*"_rr;
        upb -= "upb/reflection/stage0/.*"_rr;
        upb -= "upb/wire/decode_fast/.*"_rr;

        upb -= ".*test.*"_rr;
        upb -= "upb/conform.*"_rr;
        upb -= "upb/.*_benchmark.cc"_rr;

        //upb -= "upb/reflection/field_def.c";
        //upb -= "upb/reflection/internal/def_builder.c";

        //upb.Public += "cmake"_idir;
        //if (upb.getCompilerType() != CompilerType::GNU)
        //upb.Public += "upb/reflection/stage0"_idir;
        upb.Public += "upb/reflection/cmake"_idir;
        upb.Public += "."_idir;

        //upb += "UPB_API"_api;
        upb += sw::Shared, "UPB_BUILD_API"_def;

        //upb.Public += "org.sw.demo.google.protocolbuffers.utf8_range-main"_dep;
        upb.Public += utf8_range;
    }

    auto lite_sources = [](auto &&f) {
        for (auto &&fn : {
            "src/google/protobuf/any_lite.cc",
            "src/google/protobuf/arena.cc",
            "src/google/protobuf/arenastring.cc",
            "src/google/protobuf/arenaz_sampler.cc",
            "src/google/protobuf/extension_set.cc",
            "src/google/protobuf/generated_enum_util.cc",
            "src/google/protobuf/generated_message_tctable_lite.cc",
            "src/google/protobuf/generated_message_util.cc",
            "src/google/protobuf/implicit_weak_message.cc",
            "src/google/protobuf/inlined_string_field.cc",
            "src/google/protobuf/io/coded_stream.cc",
            "src/google/protobuf/io/io_win32.cc",
            "src/google/protobuf/io/strtod.cc",
            "src/google/protobuf/io/zero_copy_stream.cc",
            "src/google/protobuf/io/zero_copy_stream_impl.cc",
            "src/google/protobuf/io/zero_copy_stream_impl_lite.cc",
            "src/google/protobuf/io/zero_copy_sink.cc",
            "src/google/protobuf/map.cc",
            "src/google/protobuf/message_lite.cc",
            "src/google/protobuf/port.cc",
            "src/google/protobuf/parse_context.cc",
            "src/google/protobuf/raw_ptr.cc",
            "src/google/protobuf/repeated_field.cc",
            "src/google/protobuf/repeated_ptr_field.cc",
            "src/google/protobuf/wire_format_lite.cc",
            "src/google/protobuf/stubs/common.cc",
        }) {
            f(fn);
        }
    };

    auto &protobuf_lite = p.addTarget<StaticLibraryTarget>("protobuf_lite");
    {
        protobuf_lite += cppstd;
        //protobuf_lite.ImportFromBazel = true;
        //if (!win_or_mingw(protobuf_lite))
            //protobuf_lite.ExportAllSymbols = true;
        protobuf_lite += "src/google/protobuf/.*\\.h"_rr;
        protobuf_lite += "src/google/protobuf/.*\\.inc"_rr;
        lite_sources([&](auto &&fn) { protobuf_lite += fn; });
        protobuf_lite += sw::Shared, "LIBPROTOBUF_EXPORTS"_d;
        protobuf_lite.Public += sw::Shared, "PROTOBUF_USE_DLLS"_d;
        if (!win_or_mingw(protobuf_lite))
        {
            protobuf_lite.Public += "HAVE_PTHREAD"_d;
            if (!protobuf_lite.getBuildSettings().TargetOS.Android)
                protobuf_lite += "pthread"_slib;
        }
        protobuf_lite.patch("src/google/protobuf/port_def.inc", "[[nodiscard]]", ""); // does not work on clang+shared build
        protobuf_lite.patch("src/google/protobuf/port_def.inc", " && !defined(__i386__)", " &&  !defined(__i386__) && !defined(__GNUC__)"); // does not work on gcc-15
        protobuf_lite.patch("src/google/protobuf/parse_context.h", "PROTOBUF_EXPORT_TEMPLATE_DEFINE", "//PROTOBUF_EXPORT_TEMPLATE_DEFINE");
        protobuf_lite.Public += "org.sw.demo.google.abseil"_dep;
        protobuf_lite.Public += "org.sw.demo.google.protocolbuffers.utf8_validity-main"_dep;
        protobuf_lite.patch("src/google/protobuf/map_field.h", "constexpr MapFieldBase::VTable", "const MapFieldBase::VTable");
        protobuf_lite.patch("src/google/protobuf/port.h",
            "true_type HasConstexprDefaultConstructor",
            "false_type HasConstexprDefaultConstructor"
        );
    }

    auto &protobuf = p.addTarget<StaticLibraryTarget>("protobuf");
    {
        protobuf += cppstd;
        //protobuf.ImportFromBazel = true;
        //protobuf.BazelNames.insert("protobuf_lite");
        protobuf +=
            "src/google/protobuf/any.cc",
            "src/google/protobuf/any.pb.cc",
            "src/google/protobuf/api.pb.cc",
            "src/google/protobuf/cpp_features.pb.cc",
            "src/google/protobuf/compiler/importer.cc",
            "src/google/protobuf/compiler/parser.cc",
            "src/google/protobuf/descriptor.cc",
            "src/google/protobuf/descriptor.pb.cc",
            "src/google/protobuf/descriptor_database.cc",
            "src/google/protobuf/duration.pb.cc",
            "src/google/protobuf/dynamic_message.cc",
            "src/google/protobuf/empty.pb.cc",
            "src/google/protobuf/extension_set_heavy.cc",
            "src/google/protobuf/feature_resolver.cc",
            "src/google/protobuf/field_mask.pb.cc",
            "src/google/protobuf/generated_message_bases.cc",
            "src/google/protobuf/generated_message_reflection.cc",
            "src/google/protobuf/generated_message_tctable_full.cc",
            "src/google/protobuf/generated_message_tctable_gen.cc",
            //"src/google/protobuf/internal_message_util.cc",
            "src/google/protobuf/io/gzip_stream.cc",
            "src/google/protobuf/io/printer.cc",
            "src/google/protobuf/io/tokenizer.cc",
            "src/google/protobuf/internal_feature_helper.cc",
            "src/google/protobuf/map_field.cc",
            "src/google/protobuf/message.cc",
            "src/google/protobuf/micro_string.cc",
            "src/google/protobuf/reflection_mode.cc",
            "src/google/protobuf/reflection_ops.cc",
            "src/google/protobuf/service.cc",
            "src/google/protobuf/source_context.pb.cc",
            "src/google/protobuf/struct.pb.cc",
            "src/google/protobuf/text_format.cc",
            "src/google/protobuf/timestamp.pb.cc",
            "src/google/protobuf/type.pb.cc",
            "src/google/protobuf/unknown_field_set.cc",
            "src/google/protobuf/util/delimited_message_util.cc",
            "src/google/protobuf/util/field_comparator.cc",
            "src/google/protobuf/util/field_mask_util.cc",
            "src/google/protobuf/util/message_differencer.cc",
            "src/google/protobuf/util/time_util.cc",
            "src/google/protobuf/util/type_resolver_util.cc",
            "src/google/protobuf/wire_format.cc",
            "src/google/protobuf/wrappers.pb.cc"
            ;
        lite_sources([&](auto &&fn) { protobuf -= fn; });
        //if (!win_or_mingw(protobuf))
            //protobuf.ExportAllSymbols = true;
        //else
            lite_sources([&](auto &&fn) { protobuf += fn; });
        protobuf.Private += sw::Shared, "LIBPROTOBUF_EXPORTS"_d;
        protobuf.Public += sw::Shared, "PROTOBUF_USE_DLLS"_d;
        protobuf.Public += protobuf_lite;
        protobuf.Public += "org.sw.demo.madler.zlib"_dep;
        if (!win_or_mingw(protobuf))
        {
            protobuf.Public += "HAVE_PTHREAD"_d;
            if (!protobuf.getBuildSettings().TargetOS.Android)
                protobuf += "pthread"_slib;
        }
        protobuf.patch("src/google/protobuf/port_def.inc", "[[nodiscard]]", ""); // does not work on clang+shared build
        protobuf.patch("src/google/protobuf/port_def.inc", " && !defined(__i386__)", " &&  !defined(__i386__) && !defined(__GNUC__)"); // does not work on gcc-15
        protobuf.patch("src/google/protobuf/parse_context.h", "PROTOBUF_EXPORT_TEMPLATE_DEFINE", "//PROTOBUF_EXPORT_TEMPLATE_DEFINE");
    }

    auto &mangle = p.addTarget<StaticLibraryTarget>("upb_generator.mangle");
    {
        mangle += cppstd;
        mangle += "upb_generator/common/.*"_r;
        mangle += "upb_generator/minitable/generator.*"_r;
        //mangle += "upb_generator/minitable/fasttable.*"_r;
        mangle += "upb_generator/minitable/names.*"_r;
        mangle += "upb_generator/common.*"_r;
        mangle += "upb_generator/file_la.*"_r;
        mangle += "upb_generator/plugin.*"_r;
        mangle.Public += "org.sw.demo.google.abseil"_dep;
        mangle.Public += "."_idir;
        mangle.Public += upb, protobuf;
    }

    auto &protoc_lib = p.addTarget<StaticLibraryTarget>("protoc_lib");
    {
        protoc_lib += cppstd;
        //protoc_lib.ImportFromBazel = true;
        protoc_lib +=
            "src/google/protobuf/compiler/code_generator.cc",
            "src/google/protobuf/compiler/code_generator_lite.cc",
            "src/google/protobuf/compiler/command_line_interface.cc",
            "src/google/protobuf/compiler/retention.cc",
            "src/google/protobuf/compiler/plugin.cc",
            "src/google/protobuf/compiler/plugin.pb.cc",
            "src/google/protobuf/compiler/python/generator.cc",
            "src/google/protobuf/compiler/python/helpers.cc",
            "src/google/protobuf/compiler/python/pyi_generator.cc",
            "src/google/protobuf/compiler/ruby/ruby_generator.cc",
            "src/google/protobuf/compiler/subprocess.cc",
            "src/google/protobuf/compiler/versions.cc",
            "src/google/protobuf/compiler/zip_writer.cc",
            "src/google/protobuf/testing/file.*"_rr
            ;
        for (auto &&l : {"cpp", "csharp", "objectivec", "php", "java", "kotlin", "rust"}) {
            protoc_lib += FileRegex{"src/google/protobuf/compiler/"s + l + "/", ".*", true};
            protoc_lib.AllowEmptyRegexes = true;
            protoc_lib -= FileRegex{"src/google/protobuf/compiler/"s + l + "/", ".*unittest.cc", true};
            protoc_lib -= FileRegex{"src/google/protobuf/compiler/"s + l + "/", ".*test.cc", true};
            protoc_lib -= FileRegex{"src/google/protobuf/compiler/"s + l + "/tools", ".*", true};
            protoc_lib.AllowEmptyRegexes = false;
        }
        //if (!win_or_mingw(protoc_lib))
            //protoc_lib.ExportAllSymbols = true;
        protoc_lib.Private += sw::Shared, "LIBPROTOC_EXPORTS"_d;
        protoc_lib.Public += sw::Shared, "PROTOBUF_USE_DLLS"_d;
        protoc_lib.Public += protobuf;
        protoc_lib.Public += mangle;
    }

    auto &protoc = p.addTarget<ExecutableTarget>("protoc");
    {
        protoc += cppstd;
        //protoc.ImportFromBazel = true;
        protoc += "src/google/protobuf/compiler/main.cc";
        if (!win_or_mingw(protoc))
            protoc.ExportAllSymbols = true;
        protoc +=
            "src/google/protobuf/any.proto",
            "src/google/protobuf/api.proto",
            "src/google/protobuf/compiler/plugin.proto",
            "src/google/protobuf/descriptor.proto",
            "src/google/protobuf/duration.proto",
            "src/google/protobuf/empty.proto",
            "src/google/protobuf/field_mask.proto",
            "src/google/protobuf/source_context.proto",
            "src/google/protobuf/struct.proto",
            "src/google/protobuf/timestamp.proto",
            "src/google/protobuf/type.proto",
            "src/google/protobuf/wrappers.proto";
        protoc += protoc_lib;
        if (win_or_mingw(protoc))
            protoc += "Shell32.lib"_slib;
    }
}
