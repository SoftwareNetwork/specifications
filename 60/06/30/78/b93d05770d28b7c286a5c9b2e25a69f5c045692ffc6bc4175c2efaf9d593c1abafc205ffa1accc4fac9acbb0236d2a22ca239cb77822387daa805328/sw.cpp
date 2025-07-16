template <class T = LibraryTarget>
auto add_library(Project &p, const String &name) -> decltype(auto)
{
    auto &t = p.addTarget<T>(name);
    t.setRootDirectory("libs/core/" + name);
    t += cpp17;
    t.Public += "include"_idir;
    if (fs::exists(t.SourceDir / "include_compatibility"))
        t.Public += IncludeDirectory("include_compatibility"s);
    t.writeFileOnce("hpx/modules/" + name + ".hpp");
    return t;
};

void build(Solution &s)
{
    auto &p = s.addProject("StellarGroup.hpx", "1.11.0");
    p += Git("https://github.com/STEllAR-GROUP/hpx", "v{v}");

#define ADD_LIBRARY(x) \
    auto &x = add_library(p, #x)

    ADD_LIBRARY(preprocessor);

    ADD_LIBRARY(config);
    {
        config.Protected += sw::Shared, "HPX_EXPORTS"_d;

        config.Public += "org.sw.demo.boost.config"_dep;
        config.Public += preprocessor;

        config.writeFileOnce("hpx/config/libs_enabled.hpp");

        {
            String defines;

            auto add_def = [&defines](const String &d, const String &v = {})
            {
                defines += "#define "s + d;
                if (!v.empty())
                    defines += " " + v;
                defines += "\n";
            };
            auto add_def1 = [&add_def](const String &d)
            {
                add_def(d, "1");
            };
            auto add_defi = [&add_def](const String &d, int i)
            {
                add_def(d, std::to_string(i));
            };

            add_def1("HPX_HAVE_LOCAL_SCHEDULER");
            add_def1("HPX_HAVE_STATIC_SCHEDULER");
            add_def1("HPX_HAVE_STATIC_PRIORITY_SCHEDULER");
            add_def1("HPX_HAVE_ABP_SCHEDULER");
            add_def1("HPX_HAVE_SHARED_PRIORITY_SCHEDULER");

            add_def1("HPX_HAVE_CXX11_NSDMI");
            add_def1("HPX_HAVE_CXX11_EXTENDED_FRIEND_DECLARATIONS");
            add_def1("HPX_HAVE_CXX11_CONSTEXPR");
            add_def1("HPX_HAVE_CXX14_CONSTEXPR");

            //add_def1("BOOST_HAS_INT128");
            add_def1("HPX_HAVE_ABP_SCHEDULER");
            add_def1("HPX_HAVE_AGAS_DUMP_REFCNT_ENTRIES");
            add_def1("HPX_HAVE_ALGORITHM_INPUT_ITERATOR_SUPPORT");
            //add_def1("HPX_HAVE_APEX");
            add_def1("HPX_HAVE_ATTACH_DEBUGGER_ON_TEST_FAILURE");
            add_def1("HPX_HAVE_AUTOMATIC_SERIALIZATION_REGISTRATION");
            add_def1("HPX_HAVE_AWAIT");
            add_def1("HPX_HAVE_BACKGROUND_THREAD_COUNTERS");
            add_def1("HPX_HAVE_COMPRESSION_BZIP2");
            add_def1("HPX_HAVE_COMPRESSION_SNAPPY");
            add_def1("HPX_HAVE_COMPRESSION_ZLIB");
            add_def1("HPX_HAVE_COMPUTE");
            add_def1("HPX_HAVE_COROUTINE_GCC_HIDDEN_VISIBILITY");
            //add_def1("HPX_HAVE_CUDA");
            add_def1("HPX_HAVE_CXX14_RETURN_TYPE_DEDUCTION");
            add_def1("HPX_HAVE_DEPRECATION_WARNINGS");
            add_def1("HPX_HAVE_DISABLED_SIGNAL_EXCEPTION_HANDLERS");
            add_def1("HPX_HAVE_ELF_HIDDEN_VISIBILITY");
            //add_def1("HPX_HAVE_EMULATE_COROUTINE_SUPPORT_LIBRARY");
            add_def1("HPX_HAVE_FAULT_TOLERANCE");
            add_def1("HPX_HAVE_FIBER_BASED_COROUTINES");
            //add_def1("HPX_HAVE_GENERIC_CONTEXT_COROUTINES");
            add_def("HPX_HAVE_GIT_COMMIT", "\"\"");
            add_def1("HPX_HAVE_GOOGLE_PERFTOOLS");
            add_def1("HPX_HAVE_INCLUSIVE_SCAN_COMPATIBILITY");
            //add_def("HPX_HAVE_INTERNAL_ALLOCATOR");
            add_def1("HPX_HAVE_IO_COUNTERS");
            add_def1("HPX_HAVE_IO_POOL");
            add_defi("HPX_HAVE_ITTNOTIFY", 0);
            //add_def1("HPX_HAVE_JEMALLOC_PREFIX ${HPX_WITH_JEMALLOC_PREFIX}");
            add_def1("HPX_HAVE_LOCAL_SCHEDULER");
            add_def1("HPX_HAVE_LOGGING");
            //add_def("HPX_HAVE_MALLOC", "\"\"");
            add_defi("HPX_HAVE_MAX_CPU_COUNT", 64);
            add_defi("HPX_HAVE_MAX_NUMA_DOMAIN_COUNT", 4);
            add_def1("HPX_HAVE_MORE_THAN_64_THREADS");
            add_def1("HPX_HAVE_NATIVE_TLS");
            add_def1("HPX_HAVE_CXX11_THREAD_LOCAL");
            add_def1("HPX_HAVE_NICE_THREADLEVEL");
            add_def1("HPX_HAVE_PAPI");
            add_def1("HPX_HAVE_PARCELPORT_ACTION_COUNTERS");
            add_def1("HPX_HAVE_PARCEL_COALESCING");
            add_def1("HPX_HAVE_PARCEL_PROFILING");
            add_def1("HPX_HAVE_PLUGIN_GCC_HIDDEN_VISIBILITY");
            add_def1("HPX_HAVE_QUEUE_COMPATIBILITY");
            add_def1("HPX_HAVE_RUN_MAIN_EVERYWHERE");
            //add_def1("HPX_HAVE_SCHEDULER_LOCAL_STORAGE");
            add_def1("HPX_HAVE_SCOPED_UNLOCK_COMPATIBILITY");
            add_def1("HPX_HAVE_SHARED_PRIORITY_SCHEDULER");
            add_def1("HPX_HAVE_SPINLOCK_DEADLOCK_DETECTION");
            add_defi("HPX_HAVE_SPINLOCK_POOL_NUM", 128);
            add_def1("HPX_HAVE_STACKOVERFLOW_DETECTION");
            add_def1("HPX_HAVE_STACKTRACES");
            //add_def1("HPX_HAVE_STATIC_LINKING");
            add_def1("HPX_HAVE_STATIC_PRIORITY_SCHEDULER");
            add_def1("HPX_HAVE_STATIC_SCHEDULER");
            add_def1("HPX_HAVE_SWAP_CONTEXT_EMULATION");
            add_def1("HPX_HAVE_THREAD_BACKTRACE_DEPTH");
            add_def1("HPX_HAVE_THREAD_BACKTRACE_ON_SUSPENSION");
            add_def1("HPX_HAVE_THREAD_COMPATIBILITY");
            add_def1("HPX_HAVE_THREAD_CREATION_AND_CLEANUP_RATES");
            add_def1("HPX_HAVE_THREAD_CUMULATIVE_COUNTS");
            add_def1("HPX_HAVE_THREAD_DEADLOCK_DETECTION");
            add_def1("HPX_HAVE_THREAD_DESCRIPTION");
            add_def1("HPX_HAVE_THREAD_DESCRIPTION_FULL");
            add_def1("HPX_HAVE_THREAD_FULLBACKTRACE_ON_SUSPENSION");
            add_def1("HPX_HAVE_THREAD_GUARD_PAGE");
            add_def1("HPX_HAVE_THREAD_IDLE_RATES");
            add_def1("HPX_HAVE_THREAD_LOCAL_STORAGE");
            //add_def1("HPX_HAVE_THREAD_MANAGER_IDLE_BACKOFF");
            add_def1("HPX_HAVE_THREAD_PARENT_REFERENCE");
            add_def1("HPX_HAVE_THREAD_PHASE_INFORMATION");
            add_def1("HPX_HAVE_THREAD_QUEUE_WAITTIME");
            add_def1("HPX_HAVE_THREAD_STACK_MMAP");
            add_def1("HPX_HAVE_THREAD_STEALING_COUNTS");
            add_def1("HPX_HAVE_THREAD_TARGET_ADDRESS");
            add_def1("HPX_HAVE_TIMER_POOL");
            add_def1("HPX_HAVE_TUPLE_RVALUE_SWAP");
            add_def1("HPX_HAVE_UNWRAPPED_COMPATIBILITY");
            add_def1("HPX_HAVE_VALGRIND");
            add_def1("HPX_HAVE_VCPKG");
            add_def1("HPX_HAVE_VERIFY_LOCKS");
            //add_def1("HPX_HAVE_VERIFY_LOCKS_BACKTRACE");
            add_def1("HPX_HAVE_VERIFY_LOCKS_GLOBALLY");
            add_defi("HPX_SCHEDULER_MAX_TERMINATED_THREADS", 100);
            add_def1("HPX_ZERO_COPY_SERIALIZATION_THRESHOLD");

            config.Variables["hpx_config_defines"] = defines;
            //hpx.Variables["hpx_config_information"] = "\"\",";

            config.configureFile("cmake/templates/config_defines.hpp.in", "hpx/config/defines.hpp");

            config.Variables["HPX_VERSION_MAJOR"] = config.Variables["PACKAGE_VERSION_MAJOR"];
            config.Variables["HPX_VERSION_MINOR"] = config.Variables["PACKAGE_VERSION_MINOR"];
            config.Variables["HPX_VERSION_SUBMINOR"] = config.Variables["PACKAGE_VERSION_PATCH"];
            config.Variables["HPX_VERSION_DATE"] = 0;
            config.configureFile("cmake/templates/config_version.hpp.in", "hpx/config/version.hpp");
        }
    }

#define ADD_LIBRARY_WITH_CONFIG(x) ADD_LIBRARY(x); x.Public += config

    ADD_LIBRARY_WITH_CONFIG(assertion);
    ADD_LIBRARY_WITH_CONFIG(timing);
    ADD_LIBRARY_WITH_CONFIG(hashing);
    ADD_LIBRARY_WITH_CONFIG(hardware);

    ADD_LIBRARY(type_support);
    type_support.Public += assertion;

    ADD_LIBRARY(concepts);
    concepts.Public += type_support;

    auto &thread_support = add_library<StaticLibrary>(p, "thread_support");
    thread_support.Public += concepts;

    ADD_LIBRARY(datastructures);
    datastructures.Public += type_support;
    datastructures.Public += "org.sw.demo.boost.array"_dep;
    datastructures.writeFileOnce("hpx/datastructures/config/defines.hpp");

    ADD_LIBRARY(filesystem);
    filesystem.Public += config;
    //filesystem.Public += "org.sw.demo.boost.filesystem"_dep;
    filesystem.writeFileOnce("hpx/filesystem/config/defines.hpp");

    ADD_LIBRARY(format);
    format.Public += assertion;
    format.Public += "org.sw.demo.boost.utility"_dep;

    ADD_LIBRARY(logging);
    logging.Public += assertion, filesystem, format, timing;
    logging += "org.sw.demo.boost.lexical_cast"_dep;
    logging.Public += "org.sw.demo.boost.smart_ptr"_dep;
    logging += "org.sw.demo.boost.utility"_dep;

    ADD_LIBRARY(errors);
    errors.Public += datastructures, format, logging, thread_support;
    errors.Public += "org.sw.demo.boost.system"_dep;
    /*errors.writeFileOnce("hpx/errors.hpp", R"(
#include <hpx/errors/error.hpp>
#include <hpx/errors/error_code.hpp>
#include <hpx/errors/exception.hpp>
#include <hpx/errors/exception_fwd.hpp>
#include <hpx/errors/exception_info.hpp>
#include <hpx/errors/exception_list.hpp>
#include <hpx/errors/throw_exception.hpp>
)");*/

    ADD_LIBRARY_WITH_CONFIG(concurrency);
    ADD_LIBRARY_WITH_CONFIG(itt_notify);

    ADD_LIBRARY(functional);
    functional.Public += concurrency, errors, itt_notify;

    ADD_LIBRARY(execution_base);
    execution_base.Public += functional;

    ADD_LIBRARY(topology);
    topology.Public += assertion, concurrency, execution_base;
    topology.Public += "org.sw.demo.boost.io"_dep;
    topology.Public += "org.sw.demo.open_mpi.hwloc"_dep;

    //ADD_LIBRARY(serialization);
    //serialization.Public += errors;

    return;

    //
    /*{
    hpx +=
    "cmake/.*"_rr,
    "hpx/.*"_rr,
    "plugins/parcelport/tcp/.*"_rr,
    "src/.*"_rr;

    hpx -=
    "cmake/.*"_rr,
    "src/hpx_main.*"_rr,
    "src/hpx_user.*"_rr,
    "src/main.cpp";

    hpx.Public += "."_idir;
    hpx.Public += "hpx/include"_idir;
    hpx.Public += "src"_idir;

    hpx.Public += pp;

    hpx.Private += "HPX_APPLICATION_EXPORTS"_d;
    hpx.Private += "HPX_COMPONENT_EXPORTS"_d;
    hpx.Private += "HPX_COROUTINE_EXPORTS"_d;
    hpx.Private += "HPX_EXPORTS"_d;
    hpx.Private += "HPX_EXCEPTION_EXPORTS"_d;
    hpx.Private += "HPX_LIBRARY_EXPORTS"_d;
    hpx.Private += "HPX_SERIALIZATION_EXPORTS"_d;
    if (hpx.getCompilerType() == CompilerType::MSVC)
    {
    hpx.CompileOptions.push_back("-bigobj");

    hpx.Public += "Dbghelp.lib"_slib;
    hpx.Public += "Psapi.lib"_slib;
    hpx.Public += "Shlwapi.lib"_slib;
    }
    hpx.Public += sw::Static, "HPX_HAVE_STATIC_LINKING"_d;

    hpx.Public += "org.sw.demo.boost.any-1"_dep;
    hpx.Public += "org.sw.demo.boost.thread-1"_dep;
    hpx.Public += "org.sw.demo.boost.format-1"_dep;
    hpx.Public += "org.sw.demo.boost.regex-1"_dep;
    hpx.Public += "org.sw.demo.boost.assign-1"_dep;
    hpx.Public += "org.sw.demo.boost.lockfree-1"_dep;
    hpx.Public += "org.sw.demo.boost.date_time-1"_dep;
    hpx.Public += "org.sw.demo.boost.bimap-1"_dep;
    hpx.Public += "org.sw.demo.boost.accumulators-1"_dep;
    hpx.Public += "org.sw.demo.boost.program_options-1"_dep;
    hpx.Public += "org.sw.demo.boost.dynamic_bitset-1"_dep;
    hpx.Public += "org.sw.demo.boost.iostreams-1"_dep;
    hpx.Public += "org.sw.demo.boost.icl-1"_dep;
    hpx.Public += "org.sw.demo.boost.asio-1"_dep;
    hpx.Public += "org.sw.demo.boost.signals2-1"_dep;

    String defines;

    auto add_def = [&defines](const String &d, const String &v = {})
    {
    defines += "#define "s + d;
    if (!v.empty())
    defines += " " + v;
    defines += "\n";
    };
    auto add_def1 = [&add_def](const String &d)
    {
    add_def(d, "1");
    };
    auto add_defi = [&add_def](const String &d, int i)
    {
    add_def(d, std::to_string(i));
    };

    add_def1("HPX_HAVE_LOCAL_SCHEDULER");
    add_def1("HPX_HAVE_STATIC_SCHEDULER");
    add_def1("HPX_HAVE_STATIC_PRIORITY_SCHEDULER");
    add_def1("HPX_HAVE_ABP_SCHEDULER");
    add_def1("HPX_HAVE_SHARED_PRIORITY_SCHEDULER");

    add_def1("HPX_HAVE_CXX11_NSDMI");
    add_def1("HPX_HAVE_CXX11_EXTENDED_FRIEND_DECLARATIONS");
    add_def1("HPX_HAVE_CXX11_CONSTEXPR");
    add_def1("HPX_HAVE_CXX14_CONSTEXPR");

    //add_def1("BOOST_HAS_INT128");
    add_def1("HPX_HAVE_ABP_SCHEDULER");
    add_def1("HPX_HAVE_AGAS_DUMP_REFCNT_ENTRIES");
    add_def1("HPX_HAVE_ALGORITHM_INPUT_ITERATOR_SUPPORT");
    //add_def1("HPX_HAVE_APEX");
    add_def1("HPX_HAVE_ATTACH_DEBUGGER_ON_TEST_FAILURE");
    add_def1("HPX_HAVE_AUTOMATIC_SERIALIZATION_REGISTRATION");
    add_def1("HPX_HAVE_AWAIT");
    add_def1("HPX_HAVE_BACKGROUND_THREAD_COUNTERS");
    add_def1("HPX_HAVE_COMPRESSION_BZIP2");
    add_def1("HPX_HAVE_COMPRESSION_SNAPPY");
    add_def1("HPX_HAVE_COMPRESSION_ZLIB");
    add_def1("HPX_HAVE_COMPUTE");
    add_def1("HPX_HAVE_COROUTINE_GCC_HIDDEN_VISIBILITY");
    //add_def1("HPX_HAVE_CUDA");
    add_def1("HPX_HAVE_CXX14_RETURN_TYPE_DEDUCTION");
    add_def1("HPX_HAVE_DEPRECATION_WARNINGS");
    add_def1("HPX_HAVE_DISABLED_SIGNAL_EXCEPTION_HANDLERS");
    add_def1("HPX_HAVE_ELF_HIDDEN_VISIBILITY");
    //add_def1("HPX_HAVE_EMULATE_COROUTINE_SUPPORT_LIBRARY");
    add_def1("HPX_HAVE_FAULT_TOLERANCE");
    add_def1("HPX_HAVE_FIBER_BASED_COROUTINES");
    //add_def1("HPX_HAVE_GENERIC_CONTEXT_COROUTINES");
    add_def("HPX_HAVE_GIT_COMMIT", "\"\"");
    add_def1("HPX_HAVE_GOOGLE_PERFTOOLS");
    add_def1("HPX_HAVE_INCLUSIVE_SCAN_COMPATIBILITY");
    //add_def("HPX_HAVE_INTERNAL_ALLOCATOR");
    add_def1("HPX_HAVE_IO_COUNTERS");
    add_def1("HPX_HAVE_IO_POOL");
    add_defi("HPX_HAVE_ITTNOTIFY", 0);
    //add_def1("HPX_HAVE_JEMALLOC_PREFIX ${HPX_WITH_JEMALLOC_PREFIX}");
    add_def1("HPX_HAVE_LOCAL_SCHEDULER");
    add_def1("HPX_HAVE_LOGGING");
    //add_def("HPX_HAVE_MALLOC", "\"\"");
    add_defi("HPX_HAVE_MAX_CPU_COUNT", 64);
    add_defi("HPX_HAVE_MAX_NUMA_DOMAIN_COUNT", 4);
    add_def1("HPX_HAVE_MORE_THAN_64_THREADS");
    add_def1("HPX_HAVE_NATIVE_TLS");
    add_def1("HPX_HAVE_CXX11_THREAD_LOCAL");
    add_def1("HPX_HAVE_NICE_THREADLEVEL");
    add_def1("HPX_HAVE_PAPI");
    add_def1("HPX_HAVE_PARCELPORT_ACTION_COUNTERS");
    add_def1("HPX_HAVE_PARCEL_COALESCING");
    add_def1("HPX_HAVE_PARCEL_PROFILING");
    add_def1("HPX_HAVE_PLUGIN_GCC_HIDDEN_VISIBILITY");
    add_def1("HPX_HAVE_QUEUE_COMPATIBILITY");
    add_def1("HPX_HAVE_RUN_MAIN_EVERYWHERE");
    //add_def1("HPX_HAVE_SCHEDULER_LOCAL_STORAGE");
    add_def1("HPX_HAVE_SCOPED_UNLOCK_COMPATIBILITY");
    add_def1("HPX_HAVE_SHARED_PRIORITY_SCHEDULER");
    add_def1("HPX_HAVE_SPINLOCK_DEADLOCK_DETECTION");
    add_defi("HPX_HAVE_SPINLOCK_POOL_NUM", 128);
    add_def1("HPX_HAVE_STACKOVERFLOW_DETECTION");
    add_def1("HPX_HAVE_STACKTRACES");
    //add_def1("HPX_HAVE_STATIC_LINKING");
    add_def1("HPX_HAVE_STATIC_PRIORITY_SCHEDULER");
    add_def1("HPX_HAVE_STATIC_SCHEDULER");
    add_def1("HPX_HAVE_SWAP_CONTEXT_EMULATION");
    add_def1("HPX_HAVE_THREAD_BACKTRACE_DEPTH");
    add_def1("HPX_HAVE_THREAD_BACKTRACE_ON_SUSPENSION");
    add_def1("HPX_HAVE_THREAD_COMPATIBILITY");
    add_def1("HPX_HAVE_THREAD_CREATION_AND_CLEANUP_RATES");
    add_def1("HPX_HAVE_THREAD_CUMULATIVE_COUNTS");
    add_def1("HPX_HAVE_THREAD_DEADLOCK_DETECTION");
    add_def1("HPX_HAVE_THREAD_DESCRIPTION");
    add_def1("HPX_HAVE_THREAD_DESCRIPTION_FULL");
    add_def1("HPX_HAVE_THREAD_FULLBACKTRACE_ON_SUSPENSION");
    add_def1("HPX_HAVE_THREAD_GUARD_PAGE");
    add_def1("HPX_HAVE_THREAD_IDLE_RATES");
    add_def1("HPX_HAVE_THREAD_LOCAL_STORAGE");
    //add_def1("HPX_HAVE_THREAD_MANAGER_IDLE_BACKOFF");
    add_def1("HPX_HAVE_THREAD_PARENT_REFERENCE");
    add_def1("HPX_HAVE_THREAD_PHASE_INFORMATION");
    add_def1("HPX_HAVE_THREAD_QUEUE_WAITTIME");
    add_def1("HPX_HAVE_THREAD_STACK_MMAP");
    add_def1("HPX_HAVE_THREAD_STEALING_COUNTS");
    add_def1("HPX_HAVE_THREAD_TARGET_ADDRESS");
    add_def1("HPX_HAVE_TIMER_POOL");
    add_def1("HPX_HAVE_TUPLE_RVALUE_SWAP");
    add_def1("HPX_HAVE_UNWRAPPED_COMPATIBILITY");
    add_def1("HPX_HAVE_VALGRIND");
    add_def1("HPX_HAVE_VCPKG");
    add_def1("HPX_HAVE_VERIFY_LOCKS");
    add_def1("HPX_HAVE_VERIFY_LOCKS_BACKTRACE");
    add_def1("HPX_HAVE_VERIFY_LOCKS_GLOBALLY");
    add_defi("HPX_SCHEDULER_MAX_TERMINATED_THREADS", 100);
    add_def1("HPX_ZERO_COPY_SERIALIZATION_THRESHOLD");

    if (1)
    {
    auto pp_plugins = { "tcp",
    //"verbs","mpi",
    };
    add_def1("HPX_HAVE_NETWORKING");

    String i, e;
    for (String p : pp_plugins)
    {
    add_def1("HPX_HAVE_PARCELPORT_" + boost::to_upper_copy(p));

    e += "HPX_EXPORT hpx::plugins::parcelport_factory_base *parcelport_" + p +
    "_factory_init(std::vector<hpx::plugins::parcelport_factory_base *>& factories);\n";
    i += "        parcelport_" + p + "_factory_init(factories);\n";
    }
    hpx.Variables["_parcelport_init"] = i;
    hpx.Variables["_parcelport_export"] = e;
    }

    hpx.Variables["hpx_config_defines"] = defines;
    //hpx.Variables["hpx_config_information"] = "\"\",";

    hpx.configureFile("cmake/templates/static_parcelports.hpp.in", "hpx/runtime/parcelset/static_parcelports.hpp");
    //hpx.configureFile("cmake/templates/config_defines.hpp.in", "hpx/config/defines.hpp");
    hpx.configureFile("cmake/templates/config_defines_strings.hpp.in", "hpx/config/config_strings.hpp");

    hpx.Variables["HPX_VERSION_MAJOR"] = hpx.Variables["PACKAGE_VERSION_MAJOR"];
    hpx.Variables["HPX_VERSION_MINOR"] = hpx.Variables["PACKAGE_VERSION_MINOR"];
    hpx.Variables["HPX_VERSION_SUBMINOR"] = hpx.Variables["PACKAGE_VERSION_PATCH"];
    hpx.Variables["HPX_VERSION_DATE"] = 0;
    //hpx.configureFile("cmake/templates/config_version.hpp.in", "hpx/config/version.hpp");
    }

    return;

    auto add_plugin = [&hpx, &s](const String &name, const String &dir) -> decltype(auto)
    {
    auto &t = hpx.addTarget<LibraryTarget>(name);
    t += FileRegex(dir, ".*", true);
    if (t.getCompilerType() == CompilerType::MSVC)
    t.CompileOptions.push_back("-bigobj");
    t.Private += sw::Shared, "HPX_LIBRARY_EXPORTS"_d;
    t.Public += hpx;
    return t;
    };

    add_plugin("plugins.parcel.coalescing", "plugins/parcel/coalescing");
    //coalescing.Private += sw::Shared, "HPX_PARCEL_COALESCING_MODULE_EXPORTS"_d;

    auto add_bf = [&add_plugin](const String &name) -> decltype(auto)
    {
    return add_plugin("plugins.binary_filter." + name, "plugins/binary_filter/" + name);
    };

    add_bf("zlib");
    add_bf("bzip2");
    add_bf("snappy").Public += "org.sw.demo.google.snappy"_dep;

    auto add_comp = [&add_plugin](const String &name) -> decltype(auto)
    {
    auto d = name;
    boost::replace_all(d, ".", "/");
    return add_plugin("components." + name, "components/" + d);
    };*/

    /*add_comp("containers.partitioned_vector");
    add_comp("containers.unordered");

    add_comp("iostreams");
    add_comp("process");
    add_comp("performance_counters.memory");
    add_comp("component_storage");*/
}
