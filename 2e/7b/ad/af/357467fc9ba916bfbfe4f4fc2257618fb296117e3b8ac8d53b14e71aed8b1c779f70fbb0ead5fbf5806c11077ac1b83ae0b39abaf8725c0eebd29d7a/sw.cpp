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
    auto &p = s.addProject("google.protobuf", "3.20.1");
    p += Git("https://github.com/protocolbuffers/protobuf", "v{v}");

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

    auto cppstd = cpp11;

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
            "src/google/protobuf/map.cc",
            "src/google/protobuf/message_lite.cc",
            "src/google/protobuf/parse_context.cc",
            "src/google/protobuf/repeated_field.cc",
            "src/google/protobuf/repeated_ptr_field.cc",
            "src/google/protobuf/stubs/bytestream.cc",
            "src/google/protobuf/stubs/common.cc",
            "src/google/protobuf/stubs/int128.cc",
            "src/google/protobuf/stubs/status.cc",
            "src/google/protobuf/stubs/statusor.cc",
            "src/google/protobuf/stubs/stringpiece.cc",
            "src/google/protobuf/stubs/stringprintf.cc",
            "src/google/protobuf/stubs/structurally_valid.cc",
            "src/google/protobuf/stubs/strutil.cc",
            "src/google/protobuf/stubs/time.cc",
            "src/google/protobuf/wire_format_lite.cc",
        }) {
            f(fn);
        }
    };

    auto &protobuf_lite = p.addTarget<LibraryTarget>("protobuf_lite");
    {
        protobuf_lite += cppstd;
        //protobuf_lite.ImportFromBazel = true;
        if (!win_or_mingw(protobuf_lite))
            protobuf_lite.ExportAllSymbols = true;
        protobuf_lite += "src/google/protobuf/.*\\.h"_rr;
        protobuf_lite += "src/google/protobuf/.*\\.inc"_rr;
        lite_sources([&](auto &&fn) { protobuf_lite += fn; });
        protobuf_lite += sw::Shared, "LIBPROTOBUF_EXPORTS"_d;
        protobuf_lite.Public += sw::Shared, "PROTOBUF_USE_DLLS"_d;
        if (!win_or_mingw(protobuf_lite))
        {
            protobuf_lite.Public += "HAVE_PTHREAD"_d;
            protobuf_lite += "pthread"_slib;
        }
        protobuf_lite.patch("src/google/protobuf/port_def.inc", "[[nodiscard]]", ""); // does not work on clang+shared build
        protobuf_lite.patch("src/google/protobuf/parse_context.h", "PROTOBUF_EXPORT_TEMPLATE_DEFINE", "//PROTOBUF_EXPORT_TEMPLATE_DEFINE");
    }

    auto &protobuf = p.addTarget<LibraryTarget>("protobuf");
    {
        protobuf += cppstd;
        //protobuf.ImportFromBazel = true;
        //protobuf.BazelNames.insert("protobuf_lite");
        protobuf +=
            "src/google/protobuf/any.cc",
            "src/google/protobuf/any.pb.cc",
            "src/google/protobuf/api.pb.cc",
            "src/google/protobuf/compiler/importer.cc",
            "src/google/protobuf/compiler/parser.cc",
            "src/google/protobuf/descriptor.cc",
            "src/google/protobuf/descriptor.pb.cc",
            "src/google/protobuf/descriptor_database.cc",
            "src/google/protobuf/duration.pb.cc",
            "src/google/protobuf/dynamic_message.cc",
            "src/google/protobuf/empty.pb.cc",
            "src/google/protobuf/extension_set_heavy.cc",
            "src/google/protobuf/field_mask.pb.cc",
            "src/google/protobuf/generated_message_bases.cc",
            "src/google/protobuf/generated_message_reflection.cc",
            "src/google/protobuf/generated_message_tctable_full.cc",
            "src/google/protobuf/io/gzip_stream.cc",
            "src/google/protobuf/io/printer.cc",
            "src/google/protobuf/io/tokenizer.cc",
            "src/google/protobuf/map_field.cc",
            "src/google/protobuf/message.cc",
            "src/google/protobuf/reflection_ops.cc",
            "src/google/protobuf/service.cc",
            "src/google/protobuf/source_context.pb.cc",
            "src/google/protobuf/struct.pb.cc",
            "src/google/protobuf/stubs/substitute.cc",
            "src/google/protobuf/text_format.cc",
            "src/google/protobuf/timestamp.pb.cc",
            "src/google/protobuf/type.pb.cc",
            "src/google/protobuf/unknown_field_set.cc",
            "src/google/protobuf/util/delimited_message_util.cc",
            "src/google/protobuf/util/field_comparator.cc",
            "src/google/protobuf/util/field_mask_util.cc",
            "src/google/protobuf/util/internal/datapiece.cc",
            "src/google/protobuf/util/internal/default_value_objectwriter.cc",
            "src/google/protobuf/util/internal/error_listener.cc",
            "src/google/protobuf/util/internal/field_mask_utility.cc",
            "src/google/protobuf/util/internal/json_escaping.cc",
            "src/google/protobuf/util/internal/json_objectwriter.cc",
            "src/google/protobuf/util/internal/json_stream_parser.cc",
            "src/google/protobuf/util/internal/object_writer.cc",
            "src/google/protobuf/util/internal/proto_writer.cc",
            "src/google/protobuf/util/internal/protostream_objectsource.cc",
            "src/google/protobuf/util/internal/protostream_objectwriter.cc",
            "src/google/protobuf/util/internal/type_info.cc",
            "src/google/protobuf/util/internal/utility.cc",
            "src/google/protobuf/util/json_util.cc",
            "src/google/protobuf/util/message_differencer.cc",
            "src/google/protobuf/util/time_util.cc",
            "src/google/protobuf/util/type_resolver_util.cc",
            "src/google/protobuf/wire_format.cc",
            "src/google/protobuf/wrappers.pb.cc"
            ;
        lite_sources([&](auto &&fn) { protobuf -= fn; });
        if (!win_or_mingw(protobuf))
            protobuf.ExportAllSymbols = true;
        else
            lite_sources([&](auto &&fn) { protobuf += fn; });
        protobuf.Private += sw::Shared, "LIBPROTOBUF_EXPORTS"_d;
        protobuf.Public += sw::Shared, "PROTOBUF_USE_DLLS"_d;
        protobuf.Public += protobuf_lite;
        protobuf.Public += "org.sw.demo.madler.zlib"_dep;
        if (!win_or_mingw(protobuf))
        {
            protobuf.Public += "HAVE_PTHREAD"_d;
            protobuf += "pthread"_slib;
        }
        protobuf.patch("src/google/protobuf/port_def.inc", "[[nodiscard]]", ""); // does not work on clang+shared build
        protobuf.patch("src/google/protobuf/parse_context.h", "PROTOBUF_EXPORT_TEMPLATE_DEFINE", "//PROTOBUF_EXPORT_TEMPLATE_DEFINE");
    }

    auto &protoc_lib = p.addTarget<LibraryTarget>("protoc_lib");
    {
        protoc_lib += cppstd;
        //protoc_lib.ImportFromBazel = true;
        protoc_lib +=
            "src/google/protobuf/compiler/code_generator.cc",
            "src/google/protobuf/compiler/command_line_interface.cc",
            "src/google/protobuf/compiler/cpp/cpp_enum.cc",
            "src/google/protobuf/compiler/cpp/cpp_enum_field.cc",
            "src/google/protobuf/compiler/cpp/cpp_extension.cc",
            "src/google/protobuf/compiler/cpp/cpp_field.cc",
            "src/google/protobuf/compiler/cpp/cpp_file.cc",
            "src/google/protobuf/compiler/cpp/cpp_generator.cc",
            "src/google/protobuf/compiler/cpp/cpp_helpers.cc",
            "src/google/protobuf/compiler/cpp/cpp_map_field.cc",
            "src/google/protobuf/compiler/cpp/cpp_message.cc",
            "src/google/protobuf/compiler/cpp/cpp_message_field.cc",
            "src/google/protobuf/compiler/cpp/cpp_padding_optimizer.cc",
            "src/google/protobuf/compiler/cpp/cpp_parse_function_generator.cc",
            "src/google/protobuf/compiler/cpp/cpp_primitive_field.cc",
            "src/google/protobuf/compiler/cpp/cpp_service.cc",
            "src/google/protobuf/compiler/cpp/cpp_string_field.cc",
            "src/google/protobuf/compiler/csharp/csharp_doc_comment.cc",
            "src/google/protobuf/compiler/csharp/csharp_enum.cc",
            "src/google/protobuf/compiler/csharp/csharp_enum_field.cc",
            "src/google/protobuf/compiler/csharp/csharp_field_base.cc",
            "src/google/protobuf/compiler/csharp/csharp_generator.cc",
            "src/google/protobuf/compiler/csharp/csharp_helpers.cc",
            "src/google/protobuf/compiler/csharp/csharp_map_field.cc",
            "src/google/protobuf/compiler/csharp/csharp_message.cc",
            "src/google/protobuf/compiler/csharp/csharp_message_field.cc",
            "src/google/protobuf/compiler/csharp/csharp_primitive_field.cc",
            "src/google/protobuf/compiler/csharp/csharp_reflection_class.cc",
            "src/google/protobuf/compiler/csharp/csharp_repeated_enum_field.cc",
            "src/google/protobuf/compiler/csharp/csharp_repeated_message_field.cc",
            "src/google/protobuf/compiler/csharp/csharp_repeated_primitive_field.cc",
            "src/google/protobuf/compiler/csharp/csharp_source_generator_base.cc",
            "src/google/protobuf/compiler/csharp/csharp_wrapper_field.cc",
            "src/google/protobuf/compiler/java/java_context.cc",
            "src/google/protobuf/compiler/java/java_doc_comment.cc",
            "src/google/protobuf/compiler/java/java_enum.cc",
            "src/google/protobuf/compiler/java/java_enum_field.cc",
            "src/google/protobuf/compiler/java/java_enum_field_lite.cc",
            "src/google/protobuf/compiler/java/java_enum_lite.cc",
            "src/google/protobuf/compiler/java/java_extension.cc",
            "src/google/protobuf/compiler/java/java_extension_lite.cc",
            "src/google/protobuf/compiler/java/java_field.cc",
            "src/google/protobuf/compiler/java/java_file.cc",
            "src/google/protobuf/compiler/java/java_generator.cc",
            "src/google/protobuf/compiler/java/java_generator_factory.cc",
            "src/google/protobuf/compiler/java/java_helpers.cc",
            "src/google/protobuf/compiler/java/java_kotlin_generator.cc",
            "src/google/protobuf/compiler/java/java_map_field.cc",
            "src/google/protobuf/compiler/java/java_map_field_lite.cc",
            "src/google/protobuf/compiler/java/java_message.cc",
            "src/google/protobuf/compiler/java/java_message_builder.cc",
            "src/google/protobuf/compiler/java/java_message_builder_lite.cc",
            "src/google/protobuf/compiler/java/java_message_field.cc",
            "src/google/protobuf/compiler/java/java_message_field_lite.cc",
            "src/google/protobuf/compiler/java/java_message_lite.cc",
            "src/google/protobuf/compiler/java/java_name_resolver.cc",
            "src/google/protobuf/compiler/java/java_primitive_field.cc",
            "src/google/protobuf/compiler/java/java_primitive_field_lite.cc",
            "src/google/protobuf/compiler/java/java_service.cc",
            "src/google/protobuf/compiler/java/java_shared_code_generator.cc",
            "src/google/protobuf/compiler/java/java_string_field.cc",
            "src/google/protobuf/compiler/java/java_string_field_lite.cc",
            "src/google/protobuf/compiler/js/js_generator.cc",
            "src/google/protobuf/compiler/js/well_known_types_embed.cc",
            "src/google/protobuf/compiler/objectivec/objectivec_enum.cc",
            "src/google/protobuf/compiler/objectivec/objectivec_enum_field.cc",
            "src/google/protobuf/compiler/objectivec/objectivec_extension.cc",
            "src/google/protobuf/compiler/objectivec/objectivec_field.cc",
            "src/google/protobuf/compiler/objectivec/objectivec_file.cc",
            "src/google/protobuf/compiler/objectivec/objectivec_generator.cc",
            "src/google/protobuf/compiler/objectivec/objectivec_helpers.cc",
            "src/google/protobuf/compiler/objectivec/objectivec_map_field.cc",
            "src/google/protobuf/compiler/objectivec/objectivec_message.cc",
            "src/google/protobuf/compiler/objectivec/objectivec_message_field.cc",
            "src/google/protobuf/compiler/objectivec/objectivec_oneof.cc",
            "src/google/protobuf/compiler/objectivec/objectivec_primitive_field.cc",
            "src/google/protobuf/compiler/php/php_generator.cc",
            "src/google/protobuf/compiler/plugin.cc",
            "src/google/protobuf/compiler/plugin.pb.cc",
            "src/google/protobuf/compiler/python/python_generator.cc",
            "src/google/protobuf/compiler/python/python_helpers.cc",
            "src/google/protobuf/compiler/python/python_pyi_generator.cc",
            "src/google/protobuf/compiler/ruby/ruby_generator.cc",
            "src/google/protobuf/compiler/subprocess.cc",
            "src/google/protobuf/compiler/zip_writer.cc"
            ;
        if (!win_or_mingw(protoc_lib))
            protoc_lib.ExportAllSymbols = true;
        protoc_lib.Private += sw::Shared, "LIBPROTOC_EXPORTS"_d;
        protoc_lib.Public += sw::Shared, "PROTOBUF_USE_DLLS"_d;
        protoc_lib.Public += protobuf;
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
        protoc.Public += protoc_lib;
    }
}
