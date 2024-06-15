struct LlvmTargets
{
    using Targets = std::map<std::string, std::vector<StaticLibraryTarget*>>;

    StaticLibraryTarget *demangle;
    StaticLibraryTarget *support;
    Targets arch_targets;

    auto tablegen(const DependencyPtr &UTablegen, NativeExecutedTarget &t, const std::string &in,
        const std::string &out, const Strings &args,
        bool add_target_idir = true, const Files &idirs = {})
    {
        auto c = t.addCommand();
        c << cmd::prog(UTablegen);
        c << "--write-if-changed";
        if (t.getCompilerType() == CompilerType::MSVC)
            c << "--long-string-literals=0";
        // args
        for (auto &a : args)
            c << a;
        // add this target includes
        c
            << "-I" << t.SourceDir
            << "-I" << t.SourceDir / "include";
        // passed target dir
        if (add_target_idir)
            c << "-I" << (t.SourceDir / in).parent_path();
        // passed idirs
        for (auto &i : idirs)
            c << "-I" << i;
        // add base llvm includes
        c
            << "-I" << demangle->SourceDir
            << "-I" << demangle->SourceDir / "include";
        //
        c << cmd::in(in)
            << "-o" << cmd::out(out);
        return c;
    };
};

void build_llvm(auto &&llvm_project, LlvmTargets &targets)
{
    /*
    // Disabled warnings.
4141 // Suppress ''modifier' : used more than once' (because of __forceinline combined with inline)
4146 // Suppress 'unary minus operator applied to unsigned type, result still unsigned'
4180 // Suppress 'qualifier applied to function type has no meaning; ignored'
4244 // Suppress ''argument' : conversion from 'type1' to 'type2', possible loss of data'
4258 // Suppress ''var' : definition from the for loop is ignored; the definition from the enclosing scope is used'
4267 // Suppress ''var' : conversion from 'size_t' to 'type', possible loss of data'
4291 // Suppress ''declaration' : no matching operator delete found; memory will not be freed if initialization throws an exception'
4345 // Suppress 'behavior change: an object of POD type constructed with an initializer of the form () will be default-initialized'
4351 // Suppress 'new behavior: elements of array 'array' will be default initialized'
4355 // Suppress ''this' : used in base member initializer list'
4456 // Suppress 'declaration of 'var' hides local variable'
4457 // Suppress 'declaration of 'var' hides function parameter'
4458 // Suppress 'declaration of 'var' hides class member'
4459 // Suppress 'declaration of 'var' hides global declaration'
4503 // Suppress ''identifier' : decorated name length exceeded, name was truncated'
4624 // Suppress ''derived class' : destructor could not be generated because a base class destructor is inaccessible'
4722 // Suppress 'function' : destructor never returns, potential memory leak
4800 // Suppress ''type' : forcing value to bool 'true' or 'false' (performance warning)'
4100 // Suppress 'unreferenced formal parameter'
4127 // Suppress 'conditional expression is constant'
4512 // Suppress 'assignment operator could not be generated'
4505 // Suppress 'unreferenced local function has been removed'
4610 // Suppress '<class> can never be instantiated'
4510 // Suppress 'default constructor could not be generated'
4702 // Suppress 'unreachable code'
4245 // Suppress 'signed/unsigned mismatch'
4706 // Suppress 'assignment within conditional expression'
4310 // Suppress 'cast truncates constant value'
4701 // Suppress 'potentially uninitialized local variable'
4703 // Suppress 'potentially uninitialized local pointer variable'
4389 // Suppress 'signed/unsigned mismatch'
4611 // Suppress 'interaction between '_setjmp' and C++ object destruction is non-portable'
4805 // Suppress 'unsafe mix of type <type> and type <type> in operation'
4204 // Suppress 'nonstandard extension used : non-constant aggregate initializer'
4577 // Suppress 'noexcept used with no exception handling mode specified; termination on exception is not guaranteed'
4091 // Suppress 'typedef: ignored on left of '' when no variable is declared'
    // C4592 is disabled because of false positives in Visual Studio 2015
    // Update 1. Re-evaluate the usefulness of this diagnostic with Update 2.
4592 // Suppress ''var': symbol will be dynamically initialized (implementation limitation)
4319 // Suppress ''operator' : zero extending 'type' to 'type' of greater size'

// Ideally, we'd like this warning to be enabled, but MSVC 2013 doesn't
// support the 'aligned' attribute in the way that clang sources requires (for
// any code that uses the LLVM_ALIGNAS macro), so this is must be disabled to
// avoid unwanted alignment warnings.
// When we switch to requiring a version of MSVC that supports the 'alignas'
// specifier (MSVC 2015?) this warning can be re-enabled.
4324 // Suppress 'structure was padded due to __declspec(align())'

// Promoted warnings.
-w14062 // Promote 'enumerator in switch of enum is not handled' to level 1 warning.

// Promoted warnings to errors.
-we4238 // Promote 'nonstandard extension used : class rvalue used as lvalue' to error.

    */

    auto &llvm = llvm_project.addDirectory("llvm");

    const StringSet all_targets
    {
        "AArch64",
        "AMDGPU",
        "ARM",
        "BPF",
        "Hexagon",
        "Lanai",
        //"LoongArch",
        "Mips",
        "MSP430",
        "NVPTX",
        "PowerPC",
        "RISCV",
        "Sparc",
        "SystemZ",
        "WebAssembly",
        "X86",
        "XCore",
    };
    const auto selected_targets = all_targets;

    auto &demangle = llvm.template addTarget<StaticLibraryTarget>("Demangle");
    targets.demangle = &demangle;
    {
        demangle.setChecks("support");
        demangle += "include/.*"_rr;
        demangle += "lib/Demangle/.*"_rr;
        demangle.Public +=
            "include"_id;

        demangle.Public += "utils/LLVMVisualizers/llvm.natvis";

        if (demangle.getCompilerType() == CompilerType::MSVC)
        {
            demangle.Protected.CompileOptions.push_back("-bigobj");
            demangle.Protected.CompileOptions.push_back("/Zc:__cplusplus");
            demangle.Protected.CompileOptions.push_back("/Zc:inline");
            demangle.Protected.CompileOptions.push_back("/Zc:preprocessor");
            //demangle.Protected.CompileOptions.push_back("/permissive-");
        }
        //else if (s.Settings.Native.CompilerType == CompilerType::GNU)
            //t.CompileOptions.push_back("-Wa,-mbig-obj");

        demangle.Variables["LLVM_ENABLE_ZLIB"] = "1";
        demangle.Variables["LLVM_LIBXML2_ENABLED"] = "1";

        for (auto &t : selected_targets)
        {
            demangle.Variables["LLVM_ENUM_ASM_PRINTERS"] += "LLVM_ASM_PRINTER(" + t + ")\n";
            if (!(t == "MSP430" || t == "NVPTX" || t == "XCore"))
                demangle.Variables["LLVM_ENUM_ASM_PARSERS"] += "LLVM_ASM_PARSER(" + t + ")\n";
            demangle.Variables["LLVM_ENUM_DISASSEMBLERS"] += "LLVM_DISASSEMBLER(" + t + ")\n";
            demangle.Variables["LLVM_ENUM_TARGETS"] += "LLVM_TARGET(" + t + ")\n";
            if (t == "X86" || t == "AMDGPU")
                demangle.Variables["LLVM_ENUM_TARGETMCAS"] += "LLVM_TARGETMCA(" + t + ")\n";
        }

        demangle.Variables["LLVM_DEFAULT_TARGET_TRIPLE"] = "x86_64-pc-win32";
        demangle.Variables["LLVM_ENABLE_THREADS"] = "1";
        demangle.Variables["LLVM_HAS_ATOMICS"] = "1";
        demangle.Variables["LLVM_HOST_TRIPLE"] = "x86_64-pc-win32";
        demangle.Variables["LLVM_NATIVE_ARCH"] = "X86";

        if (demangle.getBuildSettings().TargetOS.Type == OSType::Windows)
            demangle.Variables["LLVM_ON_WIN32"] = "1";
        else
            demangle.Variables["LLVM_ON_UNIX"] = "1";

        demangle.Variables["LLVM_VERSION_MAJOR"] = demangle.Variables["PACKAGE_VERSION_MAJOR"];
        demangle.Variables["LLVM_VERSION_MINOR"] = demangle.Variables["PACKAGE_VERSION_MINOR"];
        demangle.Variables["LLVM_VERSION_PATCH"] = demangle.Variables["PACKAGE_VERSION_PATCH"];
        //demangle.replaceInFileOnce("include/llvm/Config/llvm-config.h.cmake", "\"${PACKAGE_VERSION}\"", "${PACKAGE_VERSION}");
        //demangle.replaceInFileOnce("include/llvm/Config/config.h.cmake", "\"${PACKAGE_VERSION}\"", "${PACKAGE_VERSION}");

        demangle.Variables["BUG_REPORT_URL"] = "http://llvm.org/bugs/";
        demangle.Variables["PACKAGE_BUGREPORT"] = "http://llvm.org/bugs/";
        demangle.Variables["ENABLE_BACKTRACES"] = "1";
        demangle.Variables["ENABLE_CRASH_OVERRIDES"] = "1";
        demangle.Variables["BACKTRACE_HEADER"] = "execinfo.h";

        demangle.Variables["HAVE_DECL_ARC4RANDOM"] = "1";
        demangle.Variables["HAVE_DECL_FE_ALL_EXCEPT"] = "1";
        demangle.Variables["HAVE_DECL_FE_INEXACT"] = "1";
        demangle.Variables["HAVE_DECL_STRERROR_S"] = "1";
        demangle.Variables["LLVM_ENABLE_DIA_SDK"] = "0"; // todo
        demangle.Variables["LLVM_HAVE_TF_API"] = "0";
        demangle.Variables["LLVM_HAVE_TF_AOT"] = "0";

        demangle.Variables["LLVM_VERSION_PRINTER_SHOW_HOST_TARGET_INFO"] = "1";
        demangle.Variables["LTDL_SHLIB_EXT"] = ".dll";
        demangle.Variables["RETSIGTYPE"] = "void";
        demangle.Variables["stricmp"] = "_stricmp";
        demangle.Variables["strdup"] = "_strdup";

        demangle.configureFile("include/llvm/Config/AsmPrinters.def.in", "llvm/Config/AsmPrinters.def");
        demangle.configureFile("include/llvm/Config/AsmParsers.def.in", "llvm/Config/AsmParsers.def");
        demangle.configureFile("include/llvm/Config/Disassemblers.def.in", "llvm/Config/Disassemblers.def");
        demangle.configureFile("include/llvm/Config/Targets.def.in", "llvm/Config/Targets.def");
        demangle.configureFile("include/llvm/Config/TargetMCAs.def.in", "llvm/Config/TargetMCAs.def");

        demangle.configureFile("include/llvm/Config/config.h.cmake", "llvm/Config/config.h");
        demangle.configureFile("include/llvm/Config/llvm-config.h.cmake", "llvm/Config/llvm-config.h");
        demangle.configureFile("include/llvm/Config/abi-breaking.h.cmake", "llvm/Config/abi-breaking.h");
    }

    //
    auto &Support = llvm.template addTarget<StaticLibraryTarget>("Support");
    targets.support = &Support;
    {
        Support +=
            "cmake/config.guess",
            "lib/Support/.*"_rr;

        Support.Private += "lib"_id;
        Support.Public += "include"_id;

        Support.Public += demangle;
        Support.Public += "org.sw.demo.madler.zlib"_dep;
        Support.Public += "org.sw.demo.xmlsoft.libxml2"_dep;

        //Support.configureFile("include/llvm/Support/DataTypes.h.cmake", "llvm/Support/DataTypes.h");
        Support.writeFileOnce("llvm/Support/VCSRevision.h");
        Support.writeFileOnce("llvm/Support/Extension.def", R"(//extension handlers
#undef HANDLE_EXTENSION)");

        if (Support.getBuildSettings().TargetOS.Type == OSType::Windows)
        {
            Support += "advapi32.lib"_slib;
            Support += "ole32.lib"_slib;
            Support += "shell32.lib"_slib;
        }

        //Support.patch("lib/Support/AArch64TargetParser.cpp", "../../include/", "");
    }

    //
    auto &Tablegen = llvm.template addTarget<StaticLibraryTarget>("Tablegen");
    Tablegen += "lib/TableGen/.*"_rr;
    Tablegen.Public += Support;

    //
    auto &UMinTablegen = llvm.template addTarget<ExecutableTarget>("Utils.MinTablegen");
    UMinTablegen +=
        "utils/TableGen/Attributes.cpp",
        "utils/TableGen/CodeGenIntrinsics.cpp",
        "utils/TableGen/DirectiveEmitter.cpp",
        "utils/TableGen/IntrinsicEmitter.cpp",
        "utils/TableGen/RISCVTargetDefEmitter.cpp",
        "utils/TableGen/SDNodeProperties.cpp",
        "utils/TableGen/VTEmitter.cpp",
        "utils/TableGen/TableGen.cpp"
        ;
    UMinTablegen.Public += Tablegen;
    auto tablegen0 = [&targets](auto &UTablegen, auto &t, const std::string &in,
        const std::string &out, const Strings &args,
        bool add_target_idir = true, const Files &idirs = {})
    {
        targets.tablegen(std::make_shared<Dependency>(UTablegen), t, in, out, args, add_target_idir, idirs);
    };

    auto &min_gen = llvm.template addTarget<StaticLibraryTarget>("min_gen");
    min_gen.HeaderOnly = true;
    tablegen0(UMinTablegen, min_gen, "include/llvm/CodeGen/ValueTypes.td", "llvm/CodeGen/GenVT.inc", { "-gen-vt" });

    //
    auto &UTablegen = llvm.template addTarget<ExecutableTarget>("Utils.Tablegen");
    UTablegen += "utils/TableGen/.*"_rr;
    //UTablegen += "lib/Target/X86/Disassembler/X86DisassemblerDecoderCommon.h";
    UTablegen.Public += Tablegen, min_gen;

    auto tablegen = [&](auto &t, const std::string &in,
        const std::string &out, const Strings &args,
        bool add_target_idir = true, const Files &idirs = {})
    {
        tablegen0(UTablegen, t, in, out, args, add_target_idir, idirs);
    };

    //
    auto &BinaryFormat = llvm.template addTarget<StaticLibraryTarget>("BinaryFormat");
    BinaryFormat += "lib/BinaryFormat/.*"_rr;
    BinaryFormat.Public += Support;

    //
    auto &TextAPI = llvm.template addTarget<StaticLibraryTarget>("TextAPI");
    TextAPI += "lib/TextAPI/.*"_rr;
    TextAPI += BinaryFormat;

    //
    auto &MC = llvm.template addTarget<StaticLibraryTarget>("MC");
    MC += "lib/MC/[^/]*"_rr;
    MC.Public += Support;

    //
    auto &MCParser = MC.template addTarget<StaticLibraryTarget>("Parser");
    MCParser += "lib/MC/MCParser/.*"_rr;
    MCParser.Public += MC;

    //
    auto &MCDisassembler = MC.template addTarget<StaticLibraryTarget>("Disassembler");
    MCDisassembler += "lib/MC/MCDisassembler/.*"_rr;
    MCDisassembler.Public += MC;

    auto &intrinsics_gen = llvm.template addTarget<StaticLibraryTarget>("intrinsics_gen");
    {
        intrinsics_gen.HeaderOnly = true;
        intrinsics_gen.Public += min_gen;
        auto tablegen2 = [&intrinsics_gen, &tablegen](const String &in, const String &out, const Strings &args)
        {
            tablegen(intrinsics_gen, "include/llvm/IR/" + in, "llvm/IR/" + out, args, true);
        };

        // main
        tablegen2("Attributes.td", "Attributes.inc", { "-gen-attrs" });
        tablegen2("Intrinsics.td", "IntrinsicImpl.inc", { "-gen-intrinsic-impl" });
        tablegen2("Intrinsics.td", "IntrinsicEnums.inc", { "-gen-intrinsic-enums" });

        // arch
        tablegen2("Intrinsics.td", "IntrinsicsAArch64.h", { "-gen-intrinsic-enums", "-intrinsic-prefix=aarch64" });
        tablegen2("Intrinsics.td", "IntrinsicsAMDGPU.h", { "-gen-intrinsic-enums", "-intrinsic-prefix=amdgcn" });
        tablegen2("Intrinsics.td", "IntrinsicsARM.h", { "-gen-intrinsic-enums", "-intrinsic-prefix=arm" });
        tablegen2("Intrinsics.td", "IntrinsicsBPF.h", { "-gen-intrinsic-enums", "-intrinsic-prefix=bpf" });
        tablegen2("Intrinsics.td", "IntrinsicsDirectX.h", { "-gen-intrinsic-enums", "-intrinsic-prefix=dx" });
        tablegen2("Intrinsics.td", "IntrinsicsHexagon.h", { "-gen-intrinsic-enums", "-intrinsic-prefix=hexagon" });
        tablegen2("Intrinsics.td", "IntrinsicsLoongArch.h", { "-gen-intrinsic-enums", "-intrinsic-prefix=loongarch" });
        tablegen2("Intrinsics.td", "IntrinsicsMips.h", { "-gen-intrinsic-enums", "-intrinsic-prefix=mips" });
        tablegen2("Intrinsics.td", "IntrinsicsNVPTX.h", { "-gen-intrinsic-enums", "-intrinsic-prefix=nvvm" });
        tablegen2("Intrinsics.td", "IntrinsicsPowerPC.h", { "-gen-intrinsic-enums", "-intrinsic-prefix=ppc" });
        tablegen2("Intrinsics.td", "IntrinsicsR600.h", { "-gen-intrinsic-enums", "-intrinsic-prefix=r600" });
        tablegen2("Intrinsics.td", "IntrinsicsRISCV.h", { "-gen-intrinsic-enums", "-intrinsic-prefix=riscv" });
        tablegen2("Intrinsics.td", "IntrinsicsS390.h", { "-gen-intrinsic-enums", "-intrinsic-prefix=s390" });
        tablegen2("Intrinsics.td", "IntrinsicsWebAssembly.h", { "-gen-intrinsic-enums", "-intrinsic-prefix=wasm" });
        tablegen2("Intrinsics.td", "IntrinsicsX86.h", { "-gen-intrinsic-enums", "-intrinsic-prefix=x86" });
        tablegen2("Intrinsics.td", "IntrinsicsXCore.h", { "-gen-intrinsic-enums", "-intrinsic-prefix=xcore" });
        tablegen2("Intrinsics.td", "IntrinsicsVE.h", { "-gen-intrinsic-enums", "-intrinsic-prefix=ve" });
    }

    //
    auto &BitstreamReader = llvm.template addTarget<StaticLibraryTarget>("Bitstream.Reader");
    BitstreamReader += "lib/Bitstream/Reader/.*"_rr;
    BitstreamReader.Public += Support;

    //
    auto &Remarks = llvm.template addTarget<StaticLibraryTarget>("Remarks");
    Remarks += "lib/Remarks/.*"_rr;
    Remarks.Public += BitstreamReader;

    //
    auto &Core = llvm.template addTarget<StaticLibraryTarget>("Core");
    Core += "include/llvm/IR/.*\\.td"_rr, "lib/IR/.*"_rr;
    Core.Public += Support, BinaryFormat, intrinsics_gen, Remarks;

    //
    auto &AsmParser = llvm.template addTarget<StaticLibraryTarget>("AsmParser");
    AsmParser += "lib/AsmParser/.*"_rr;
    AsmParser.Public += Core;

    //
    auto &BitReader = llvm.template addTarget<StaticLibraryTarget>("BitReader");
    BitReader += "lib/Bitcode/Reader/.*"_rr;
    BitReader += BitstreamReader;
    BitReader.Public += Core;

    //
    auto &LineEditor = llvm.template addTarget<StaticLibraryTarget>("LineEditor");
    LineEditor += "lib/LineEditor/.*"_rr;
    LineEditor.Public += Support;

    //
    auto &Option = llvm.template addTarget<StaticLibraryTarget>("Option");
    Option += "lib/Option/.*"_rr;
    Option.Public += Support;

    //
    auto &ProfileData = llvm.template addTarget<StaticLibraryTarget>("ProfileData");
    ProfileData += "lib/ProfileData/[^/]*"_rr;
    ProfileData.Public += Core;

    //
    auto &DebugInfoMSF = llvm.template addTarget<StaticLibraryTarget>("DebugInfo.MSF");
    DebugInfoMSF += "lib/DebugInfo/MSF/.*"_rr;
    DebugInfoMSF.Public += Support;

    //
    auto &DebugInfoCodeView = llvm.template addTarget<StaticLibraryTarget>("DebugInfo.CodeView");
    DebugInfoCodeView += "lib/DebugInfo/CodeView/.*"_rr;
    DebugInfoCodeView.Public += DebugInfoMSF;

    //
    auto &DebugInfoPDB = llvm.template addTarget<StaticLibraryTarget>("DebugInfo.PDB");
    DebugInfoPDB += "lib/DebugInfo/PDB/.*"_rr;
    DebugInfoPDB -= "lib/DebugInfo/PDB/DIA/.*"_rr;
    DebugInfoPDB.Public += DebugInfoMSF, DebugInfoCodeView;

    //
    auto &Object = llvm.template addTarget<StaticLibraryTarget>("Object");
    Object += "lib/Object/.*"_rr;
    Object += TextAPI;
    Object.Public += BitReader, Core, MCParser;

    //
    auto &DebugInfoDWARF = llvm.template addTarget<StaticLibraryTarget>("DebugInfo.DWARF");
    DebugInfoDWARF += "lib/DebugInfo/DWARF/.*"_rr;
    DebugInfoDWARF.Public += Object;

    //
    auto &Analysis = llvm.template addTarget<StaticLibraryTarget>("Analysis");
    Analysis += "lib/Analysis/.*"_rr;
    //Analysis -= "lib/Analysis/TFUtils.cpp";
    Analysis.Public += Object, ProfileData;

    //
    auto &IRPrinter = llvm.template addTarget<StaticLibraryTarget>("IRPrinter");
    IRPrinter += "lib/IRPrinter/.*"_rr;
    IRPrinter.Public += Analysis, Core;

    //
    auto &BitWriter = llvm.template addTarget<StaticLibraryTarget>("BitWriter");
    BitWriter += "lib/Bitcode/Writer/.*"_rr;
    BitWriter.Public += Analysis;

    //
    auto &TransformUtils = llvm.template addTarget<StaticLibraryTarget>("TransformUtils");
    TransformUtils += "lib/Transforms/Utils/.*"_rr;
    TransformUtils.Public += Analysis;

    //
    auto &FrontendOpenMP = llvm.template addTarget<StaticLibraryTarget>("Frontend.OpenMP");
    FrontendOpenMP += "lib/Frontend/OpenMP/.*"_rr;
    FrontendOpenMP.Public += TransformUtils;
    tablegen(FrontendOpenMP, "include/llvm/Frontend/OpenMP/OMP.td", "llvm/Frontend/OpenMP/OMP.inc", { "-gen-directive-impl" });
    tablegen(FrontendOpenMP, "include/llvm/Frontend/OpenMP/OMP.td", "llvm/Frontend/OpenMP/OMP.h.inc", { "-gen-directive-decl" });
    tablegen(FrontendOpenMP, "include/llvm/Frontend/OpenMP/OMP.td", "OMP.cpp", { "-gen-directive-impl" });

    auto &FrontendHLSL = llvm.template addTarget<StaticLibraryTarget>("Frontend.HLSL");
    FrontendHLSL += "lib/Frontend/HLSL/.*"_rr;
    FrontendHLSL.Public += Core;

    //
    auto &Linker = llvm.template addTarget<StaticLibraryTarget>("Linker");
    Linker += "lib/Linker/.*"_rr;
    Linker.Public += TransformUtils;

    //
    auto &AggressiveInstCombine = llvm.template addTarget<StaticLibraryTarget>("AggressiveInstCombine");
    AggressiveInstCombine += "lib/Transforms/AggressiveInstCombine/.*"_rr;
    AggressiveInstCombine.Public += TransformUtils;

    //
    auto &InstCombine = llvm.template addTarget<StaticLibraryTarget>("InstCombine");
    InstCombine += "lib/Transforms/InstCombine/.*"_rr;
    InstCombine.Public += TransformUtils;
    //tablegen(InstCombine, "lib/Transforms/InstCombine/InstCombineTables.td", "InstCombineTables.inc", { "-gen-searchable-tables" });

    //
    auto &Instrumentation = llvm.template addTarget<StaticLibraryTarget>("Instrumentation");
    Instrumentation += "lib/Transforms/Instrumentation/.*"_rr;
    Instrumentation.Public += TransformUtils;

    //
    auto &ScalarOpts = llvm.template addTarget<StaticLibraryTarget>("ScalarOpts");
    ScalarOpts += "lib/Transforms/Scalar/.*"_rr;
    ScalarOpts.Public += InstCombine;

    //
    auto &Vectorize = llvm.template addTarget<StaticLibraryTarget>("Vectorize");
    Vectorize += "lib/Transforms/Vectorize/.*"_rr;
    Vectorize.Public += TransformUtils;

    //
    auto &ObjCARCOpts = llvm.template addTarget<StaticLibraryTarget>("ObjCARCOpts");
    ObjCARCOpts += "lib/Transforms/ObjCARC/.*"_rr;
    ObjCARCOpts.Public += TransformUtils;

    //
    auto &CFGuard = llvm.template addTarget<StaticLibraryTarget>("CFGuard");
    CFGuard += "lib/Transforms/CFGuard/.*"_rr;
    CFGuard.Public += Core;

    //
    auto &HipStdPar = llvm.template addTarget<StaticLibraryTarget>("HipStdPar");
    HipStdPar += "lib/Transforms/HipStdPar/.*"_rr;
    HipStdPar.Public += Analysis, Core, TransformUtils;

    //
    auto &ObjectYAML = llvm.template addTarget<StaticLibraryTarget>("ObjectYAML");
    ObjectYAML += "lib/ObjectYAML/.*"_rr;
    ObjectYAML.Public += DebugInfoCodeView;

    //
    auto &LibDriver = llvm.template addTarget<StaticLibraryTarget>("LibDriver");
    LibDriver += "lib/ToolDrivers/llvm-lib/.*"_rr;
    LibDriver.Public += Object, Option;
    tablegen(LibDriver, "lib/ToolDrivers/llvm-lib/Options.td", "Options.inc", { "-gen-opt-parser-defs" });

    //
    auto &IRReader = llvm.template addTarget<StaticLibraryTarget>("IRReader");
    IRReader += "lib/IRReader/.*"_rr;
    IRReader.Public += AsmParser, BitReader;

    //
    auto &ipo = llvm.template addTarget<StaticLibraryTarget>("ipo");
    ipo += "lib/Transforms/IPO/.*"_rr;
    ipo.Public += BitWriter, FrontendOpenMP, IRReader, InstCombine, Instrumentation, Linker,
        Object, ScalarOpts, Vectorize;

    //
    auto &Coroutines = llvm.template addTarget<StaticLibraryTarget>("Coroutines");
    Coroutines += "lib/Transforms/Coroutines/.*"_rr;
    Coroutines.Public += ipo;

    //
    auto &HelloWorldPass = llvm.template addTarget<StaticLibraryTarget>("Hello");
    HelloWorldPass += "lib/Transforms/Hello/.*"_rr;
    HelloWorldPass.Public += Core;

    //
    auto &Coverage = llvm.template addTarget<StaticLibraryTarget>("Coverage");
    Coverage += "lib/ProfileData/Coverage/.*"_rr;
    Coverage.Public += Object, ProfileData;

    //
    auto &Symbolize = llvm.template addTarget<StaticLibraryTarget>("Symbolize");
    Symbolize += "lib/DebugInfo/Symbolize/.*"_rr;
    Symbolize.Public += Object, DebugInfoDWARF, DebugInfoPDB;

    //
    auto &RuntimeDyld = llvm.template addTarget<StaticLibraryTarget>("RuntimeDyld");
    RuntimeDyld += "lib/ExecutionEngine/RuntimeDyld/.*"_rr;
    RuntimeDyld.Public += Object;

    //
    auto &Target = llvm.template addTarget<StaticLibraryTarget>("Target");
    Target += "lib/Target/[^/]*"_rr;
    Target.Public += Analysis;

    //
    auto &CodeGen = llvm.template addTarget<StaticLibraryTarget>("CodeGen");
    CodeGen += "lib/CodeGen/[^/]*"_rr;
    CodeGen += "lib/CodeGen/LiveDebugValues/.*"_rr;
    CodeGen.Public += BitReader, BitWriter, ScalarOpts, Target;

    //
    auto &AsmPrinter = llvm.template addTarget<StaticLibraryTarget>("AsmPrinter");
    AsmPrinter += "lib/CodeGen/AsmPrinter/[^/]*"_rr;
    AsmPrinter -= "lib/CodeGen/AsmPrinter/[^/]*\\.def"_rr;
    AsmPrinter += DebugInfoDWARF;
    AsmPrinter.Public += Analysis, CodeGen, DebugInfoCodeView, DebugInfoMSF,
        MCParser;

    //
    auto &GlobalISel = llvm.template addTarget<StaticLibraryTarget>("GlobalISel");
    GlobalISel += "lib/CodeGen/GlobalISel/.*"_rr;
    GlobalISel.Public += TransformUtils, CodeGen;

    //
    auto &MIRParser = llvm.template addTarget<StaticLibraryTarget>("MIRParser");
    MIRParser += "lib/CodeGen/MIRParser/.*"_rr;
    MIRParser.Public += AsmParser, CodeGen, Core, MC;

    //
    auto &SelectionDAG = llvm.template addTarget<StaticLibraryTarget>("SelectionDAG");
    SelectionDAG += "lib/CodeGen/SelectionDAG/.*"_rr;
    SelectionDAG.Public += TransformUtils, CodeGen;

    //
    auto &ExecutionEngine = llvm.template addTarget<StaticLibraryTarget>("ExecutionEngine");
    ExecutionEngine += "lib/ExecutionEngine/[^/]*"_rr;
    ExecutionEngine.Public += Object, RuntimeDyld, Target, Core;

    //
    auto &Interpreter = llvm.template addTarget<StaticLibraryTarget>("Interpreter");
    Interpreter += "lib/ExecutionEngine/Interpreter/.*"_rr;
    Interpreter.Public += CodeGen, ExecutionEngine;

    //
    auto &MCJIT = llvm.template addTarget<StaticLibraryTarget>("MCJIT");
    MCJIT += "lib/ExecutionEngine/MCJIT/.*"_rr;
    MCJIT.Public += Object, ExecutionEngine, RuntimeDyld, Target;

    //
    auto &TargetParser = llvm.template addTarget<StaticLibraryTarget>("TargetParser");
    TargetParser += "lib/TargetParser/[^/]*"_rr;
    TargetParser.Public += Support;
    tablegen(TargetParser, "lib/Target/RISCV/RISCV.td", "llvm/TargetParser/RISCVTargetParserDef.inc", {"-gen-riscv-target-def"}, true);

    auto &FrontendOffloading = llvm.template addTarget<StaticLibraryTarget>("Frontend.Offloading");
    FrontendOffloading += "lib/Frontend/Offloading/.*"_rr;
    FrontendOffloading.Public += Core, TransformUtils, TargetParser;

    auto &FrontendDriver = llvm.template addTarget<StaticLibraryTarget>("Frontend.Driver");
    FrontendDriver += "lib/Frontend/Driver/.*"_rr;
    FrontendDriver.Public += Core, Analysis;

    //
    auto &WindowsDriver = llvm.template addTarget<StaticLibraryTarget>("WindowsDriver");
    WindowsDriver += "lib/WindowsDriver/[^/]*"_rr;
    WindowsDriver.Public += TargetParser, Option;

    //
    auto &OrcJIT = llvm.template addTarget<StaticLibraryTarget>("OrcJIT");
    OrcJIT += "lib/ExecutionEngine/Orc/.*"_rr;
    OrcJIT.Public += Object, ExecutionEngine, RuntimeDyld, TransformUtils, WindowsDriver;

    //
    auto &Passes = llvm.template addTarget<StaticLibraryTarget>("Passes");
    Passes += "lib/Passes/.*"_rr;
    Passes -= "lib/Passes/.*\\.def"_rr;
    Passes.Public += AggressiveInstCombine, Analysis, CodeGen, HelloWorldPass,
        InstCombine, Instrumentation, IRPrinter, Vectorize, ipo;

    //
    auto &LTO = llvm.template addTarget<StaticLibraryTarget>("LTO");
    LTO += "lib/LTO/.*"_rr;
    LTO.Public += Linker, ObjCARCOpts, Object, Passes;

    // Targets
    auto add_arch = [&](const std::string &dir)
    {
        auto &d = llvm.addDirectory(dir);

        auto &ATablegen = d.template addTarget<StaticLibraryTarget>("CommonTableGen");
        if (!ATablegen.DryRun)
        {
            ATablegen.HeaderOnly = true;
            //ATablegen += "include/.*\\.td"_rr;
            auto cmake = path{"lib"} / "Target" / dir / "CMakeLists.txt";
            ATablegen += cmake;
            ATablegen += FileRegex("lib/Target/" + dir, std::regex(".*\\.td"), true);

            auto tdefs = "LLVM_TARGET_DEFINITIONS"s;
            auto tblgen_string = "tablegen(LLVM "s;
            std::map<String, String> tablegens;

            auto s = read_file(ATablegen.SourceDir / cmake);
            size_t p = 0;
            while (1)
            {
                p = s.find(tdefs, p);
                if (p == s.npos)
                    break;
                auto b = p + tdefs.size();
                auto tdname = boost::trim_copy(s.substr(b, s.find(")", b) - b));
                //if (tdname.find("X86GenFoldTables") == -1) {
                    tablegens[tdname] = s.substr(b, s.find(tdefs, b) - b);
                //}
                p++; // prevents inf loops
            }
            for (auto &[f, s] : tablegens)
            {
                p = 0;
                while (1)
                {
                    p = s.find(tblgen_string, p);
                    if (p == s.npos)
                        break;
                    auto b = p + tblgen_string.size();
                    auto e = s.find(")", b);
                    auto e_fn = s.find(" ", b);
                    auto out = boost::trim_copy(s.substr(b, e_fn - b));
                    Strings args, args2;
                    boost::split(args, boost::trim_copy(s.substr(e_fn, e - e_fn)), boost::is_any_of(" "));
                    for (auto &a : args)
                    {
                        if (a.empty())
                            continue;
                        boost::replace_all(a, "\"", "");
                        args2.push_back(boost::trim_copy(a));
                    }

                    tablegen(ATablegen, "lib/Target/" + dir + "/" + f, out, args2, true, { demangle.SourceDir / "include" });

                    p++; // prevents inf loops
                }
            }
        }

        //
        auto &AInfo = d.template addTarget<StaticLibraryTarget>("Info");
        {
            AInfo += FileRegex("lib/Target/" + dir + "/TargetInfo", std::regex(".*"), true);
            AInfo += FileRegex("lib/Target/" + dir, std::regex(".*\\.h"), true);
            AInfo.AllowEmptyRegexes = true;
            AInfo -= FileRegex("lib/Target/" + dir, std::regex(".*\\.def"), true);
            AInfo -= FileRegex("lib/Target/" + dir, std::regex(".*\\.inc"), true); // Hexagon
            AInfo.AllowEmptyRegexes = false;
            AInfo.Public += IncludeDirectory{ "lib/Target/" + dir };
            AInfo.Public += Support, ATablegen, Core, TargetParser;
            targets.arch_targets[dir].push_back(&AInfo);
        }

        auto check_target = [](auto &t, const String &dir)
        {
            if (fs::exists(t.SourceDir / dir))
                return true;
            t.AutoDetectOptions = false;
            t.HeaderOnly = true;
            t.Empty = true;
            return false;
        };

        //
        auto &AUtils = d.template addTarget<StaticLibraryTarget>("Utils");
        if (check_target(AUtils, "lib/Target/" + dir + "/Utils"))
        {
            AUtils += FileRegex("lib/Target/" + dir + "/Utils", std::regex(".*"), true);
            AUtils.Public += IncludeDirectory{ "lib/Target/" + dir };
            AUtils.Public += AInfo;
            targets.arch_targets[dir].push_back(&AUtils);
        }

        //
        auto &AAsmPrinter = d.template addTarget<StaticLibraryTarget>("AsmPrinter");
        if (check_target(AAsmPrinter, "lib/Target/" + dir + "/InstPrinter"))
        {
            AAsmPrinter += FileRegex("lib/Target/" + dir + "/InstPrinter", std::regex(".*"), true);
            AAsmPrinter.Public += IncludeDirectory{ "lib/Target/" + dir };
            AAsmPrinter.Public += AInfo, MC;
            AAsmPrinter.Public += AUtils;
            targets.arch_targets[dir].push_back(&AAsmPrinter);
        }

        //
        auto &ADisassembler = d.template addTarget<StaticLibraryTarget>("Disassembler");
        if (check_target(ADisassembler, "lib/Target/" + dir + "/Disassembler"))
        {
            ADisassembler += FileRegex("lib/Target/" + dir + "/Disassembler", std::regex(".*"), true);
            ADisassembler.Public += IncludeDirectory{ "lib/Target/" + dir };
            ADisassembler.Public += AInfo, MCDisassembler;
            targets.arch_targets[dir].push_back(&ADisassembler);
        }

        //
        auto &AAsmParser = d.template addTarget<StaticLibraryTarget>("AsmParser");
        if (check_target(AAsmParser, "lib/Target/" + dir + "/AsmParser"))
        {
            AAsmParser += FileRegex("lib/Target/" + dir + "/AsmParser", std::regex(".*"), true);
            AAsmParser.Public += IncludeDirectory{ "lib/Target/" + dir };
            AAsmParser.Public += AInfo, MCParser;
            targets.arch_targets[dir].push_back(&AAsmParser);
        }

        //
        auto &ADesc = d.template addTarget<StaticLibraryTarget>("Desc");
        {
            ADesc += FileRegex("lib/Target/" + dir + "/MCTargetDesc", std::regex(".*"), true);
            ADesc.Public += IncludeDirectory{ "lib/Target/" + dir };
            ADesc.Public += MCDisassembler, Object, AInfo, AUtils, AAsmPrinter;
            targets.arch_targets[dir].push_back(&ADesc);
            if (dir == "AArch64")
                ADesc.patch("lib/Target/AArch64/MCTargetDesc/AArch64InstPrinter.h", "../", "");
            if (dir == "WebAssembly")
                ADesc.patch("lib/Target/WebAssembly/MCTargetDesc/WebAssemblyMCTargetDesc.h", "../", "");
        }

        //
        auto &ACodeGen = d.template addTarget<StaticLibraryTarget>("CodeGen");
        {
            ACodeGen += FileRegex("lib/Target/" + dir, std::regex(".*"), false);
            if (dir == "AArch64" || dir == "PowerPC" || dir == "RISCV" || dir == "BPF" || dir == "X86") {
                ACodeGen += FileRegex("lib/Target/" + dir + "/GISel", std::regex(".*"), false);
            }
            ACodeGen.AllowEmptyRegexes = true;
            ACodeGen -= FileRegex("lib/Target/" + dir, std::regex(".*\\.def"), false);
            ACodeGen.AllowEmptyRegexes = true;
            ACodeGen.Public += IncludeDirectory{ "lib/Target/" + dir };
            ACodeGen.Public += Analysis, AsmPrinter, CodeGen, Core, GlobalISel,
                SelectionDAG, Target, ADesc;
            ACodeGen += CFGuard, MIRParser;
            ACodeGen.Public += AAsmPrinter;
            targets.arch_targets[dir].push_back(&ACodeGen);
        }
    };

    for (auto &t : selected_targets)
        add_arch(t);
}

void build_clang(auto &&llvm_project, LlvmTargets &targets)
{
    auto &clang = llvm_project.addDirectory("clang");

    auto &Support = clang.template addTarget<StaticLibraryTarget>("Support");
    Support += "include/clang/Support/.*"_rr;
    Support += "lib/Support/.*"_rr;
    Support.Public += Support.constructThisPackageDependency("llvm_project.llvm.Support");

    auto &CUTablegen = clang.template addTarget<ExecutableTarget>("TableGen");
    CUTablegen += "utils/TableGen/.*"_rr;
    CUTablegen.Public += CUTablegen.constructThisPackageDependency("llvm_project.llvm.Tablegen");
    CUTablegen.Public += Support;

    auto clang_tablegen = [&CUTablegen](auto &t, const std::string &in,
        const std::string &out, const Strings &args,
        bool add_target_idir = false)
    {
        auto c = t.addCommand();
        c << cmd::prog(CUTablegen);
        for (auto &a : args)
            c << a;
        c << "-I" << t.SourceDir;
        c << "-I" << t.SourceDir / "include";
        if (add_target_idir)
            c << "-I" << (t.SourceDir / in).parent_path();
        c << cmd::in(in);
        c << "-o" << cmd::out(out);
    };

    auto clang_diag_gen = [&clang_tablegen](auto &t, const std::string &component)
    {
        clang_tablegen(t, "include/clang/Basic/Diagnostic.td",
            "clang/Basic/Diagnostic" + component + "Kinds.inc", {"-gen-clang-diags-defs","-clang-component=" + component}, true);
    };

    auto &Basic = clang.template addTarget<StaticLibraryTarget>("Basic");
    {
        Basic += "include/.*"_rr;
        Basic += "lib/Basic/.*"_rr;
        Basic.Public += "include"_idir;
        Basic += "lib/Basic"_idir;
        Basic -= "include/.*\\.def"_rr;
        Basic.Public += "utils/ClangVisualizers/clang.natvis";
        Basic.Public +=
            Basic.constructThisPackageDependency("llvm_project.llvm.Core"),
            Basic.constructThisPackageDependency("llvm_project.llvm.Frontend.HLSL"),
            Basic.constructThisPackageDependency("llvm_project.llvm.Frontend.OpenMP"),
            Basic.constructThisPackageDependency("llvm_project.llvm.MC"),
            Basic.constructThisPackageDependency("llvm_project.llvm.TargetParser"),
            Basic.constructThisPackageDependency("llvm_project.llvm.WindowsDriver")
            ;
        Basic.Protected += "_CINDEX_LIB_"_def;
        Basic.Public += sw::Static, "CINDEX_NO_EXPORTS"_def;

        Basic.Variables["BACKEND_PACKAGE_STRING"] = Basic.getPackage().getVersion().toString();
        Basic.Variables["CLANG_VERSION"] = Basic.getPackage().getVersion().toString();
        Basic.Variables["CLANG_VERSION_MAJOR"] = Basic.Variables["PACKAGE_VERSION_MAJOR"];
        Basic.Variables["CLANG_VERSION_MINOR"] = Basic.Variables["PACKAGE_VERSION_MINOR"];
        Basic.Variables["CLANG_VERSION_PATCHLEVEL"] = Basic.Variables["PACKAGE_VERSION_PATCH"];

        Basic.configureFile("include/clang/Basic/Version.inc.in", "clang/Basic/Version.inc");
        Basic.writeFileOnce("SVNVersion.h");

        Basic.Variables["BUG_REPORT_URL"] = "http://llvm.org/bugs/";

        Basic.Variables["CLANG_DEFAULT_LINKER"] = "";
        Basic.Variables["CLANG_DEFAULT_CXX_STDLIB"] = "";
        Basic.Variables["CLANG_DEFAULT_RTLIB"] = "";
        Basic.Variables["CLANG_DEFAULT_OBJCOPY"] = "objcopy";
        Basic.Variables["CLANG_DEFAULT_OPENMP_RUNTIME"] = "libomp";
        Basic.Variables["CLANG_OPENMP_NVPTX_DEFAULT_ARCH"] = "sm_30";

        Basic.Variables["CLANG_HAVE_LIBXML"] = "1";
        Basic.Variables["ENABLE_X86_RELAX_RELOCATIONS"] = "0";
        Basic.Variables["CLANG_ENABLE_ARCMT"] = "0";
        Basic.Variables["CLANG_ENABLE_OBJC_REWRITER"] = "0";
        Basic.Variables["CLANG_ENABLE_STATIC_ANALYZER"] = "0";

        Basic.configureFile("include/clang/Config/config.h.cmake", "clang/Config/config.h");
        Basic.writeFileOnce("VCSVersion.inc");

        for (auto &c : {"Analysis", "AST", "Comment", "Common", "CrossTU",
                        "Driver", "Frontend", "Lex", "Parse", "Refactoring",
                        "Sema", "Serialization"})
        {
            clang_diag_gen(Basic, c);
        }

        clang_tablegen(Basic, "include/clang/Basic/Diagnostic.td",
            "clang/Basic/DiagnosticGroups.inc", { "-gen-clang-diag-groups" }, true);
        clang_tablegen(Basic, "include/clang/Basic/Diagnostic.td",
            "clang/Basic/DiagnosticIndexName.inc", { "-gen-clang-diags-index-name" }, true);

        clang_tablegen(Basic, "include/clang/Basic/Attr.td",
            "clang/Basic/AttrList.inc", { "-gen-clang-attr-list" }, true);
        clang_tablegen(Basic, "include/clang/Basic/Attr.td",
            "clang/Basic/AttrSubMatchRulesList.inc", { "-gen-clang-attr-subject-match-rule-list" }, true);
        clang_tablegen(Basic, "include/clang/Basic/Attr.td",
            "clang/Basic/AttrHasAttributeImpl.inc", { "-gen-clang-attr-has-attribute-impl" }, true);
        clang_tablegen(Basic, "include/clang/Basic/Attr.td",
            "clang/Basic/RegularKeywordAttrInfo.inc", { "-gen-clang-regular-keyword-attr-info" }, true);
        clang_tablegen(Basic, "include/clang/Basic/Attr.td",
            "clang/Basic/AttrLeftSideCanPrintList.inc", { "-gen-clang-attr-can-print-left-list" }, true);
        clang_tablegen(Basic, "include/clang/Basic/Attr.td",
            "clang/Basic/AttrLeftSideMustPrintList.inc", { "-gen-clang-attr-must-print-left-list" }, true);

        // Basic

        // arm
        clang_tablegen(Basic, "include/clang/Basic/arm_neon.td",
            "clang/Basic/arm_neon.inc", { "-gen-arm-neon-sema" }, true);
        clang_tablegen(Basic, "include/clang/Basic/arm_fp16.td",
            "clang/Basic/arm_fp16.inc", { "-gen-arm-neon-sema" }, true);

        clang_tablegen(Basic, "include/clang/Basic/arm_mve.td",
            "clang/Basic/arm_mve_builtins.inc", { "-gen-arm-mve-builtin-def" }, true);
        clang_tablegen(Basic, "include/clang/Basic/arm_mve.td",
            "clang/Basic/arm_mve_builtin_cg.inc", { "-gen-arm-mve-builtin-codegen" }, true);
        clang_tablegen(Basic, "include/clang/Basic/arm_mve.td",
            "clang/Basic/arm_mve_builtin_sema.inc", { "-gen-arm-mve-builtin-sema" }, true);
        clang_tablegen(Basic, "include/clang/Basic/arm_mve.td",
            "clang/Basic/arm_mve_builtin_aliases.inc", { "-gen-arm-mve-builtin-aliases" }, true);

        clang_tablegen(Basic, "include/clang/Basic/arm_sme.td",
            "clang/Basic/arm_sme_builtins.inc", { "-gen-arm-sme-builtins" }, true);
        clang_tablegen(Basic, "include/clang/Basic/arm_sme.td",
            "clang/Basic/arm_sme_builtin_cg.inc", { "-gen-arm-sme-builtin-codegen" }, true);
        clang_tablegen(Basic, "include/clang/Basic/arm_sme.td",
            "clang/Basic/arm_sme_sema_rangechecks.inc", { "-gen-arm-sme-sema-rangechecks" }, true);
        clang_tablegen(Basic, "include/clang/Basic/arm_sme.td",
            "clang/Basic/arm_sme_streaming_attrs.inc", { "-gen-arm-sme-streaming-attrs" }, true);
        clang_tablegen(Basic, "include/clang/Basic/arm_sme.td",
            "clang/Basic/arm_sme_builtins_za_state.inc", { "-gen-arm-sme-builtin-za-state" }, true);

        clang_tablegen(Basic, "include/clang/Basic/arm_sve.td",
            "clang/Basic/arm_sve_builtins.inc", { "-gen-arm-sve-builtins" }, true);
        clang_tablegen(Basic, "include/clang/Basic/arm_sve.td",
            "clang/Basic/arm_sve_builtin_cg.inc", { "-gen-arm-sve-builtin-codegen" }, true);
        clang_tablegen(Basic, "include/clang/Basic/arm_sve.td",
            "clang/Basic/arm_sve_typeflags.inc", { "-gen-arm-sve-typeflags" }, true);
        clang_tablegen(Basic, "include/clang/Basic/arm_sve.td",
            "clang/Basic/arm_sve_sema_rangechecks.inc", { "-gen-arm-sve-sema-rangechecks" }, true);
        clang_tablegen(Basic, "include/clang/Basic/arm_sve.td",
            "clang/Basic/arm_sve_streaming_attrs.inc", { "-gen-arm-sve-streaming-attrs" }, true);

        clang_tablegen(Basic, "include/clang/Basic/arm_cde.td",
            "clang/Basic/arm_cde_builtins.inc", { "-gen-arm-cde-builtin-def" }, true);
        clang_tablegen(Basic, "include/clang/Basic/arm_cde.td",
            "clang/Basic/arm_cde_builtin_cg.inc", { "-gen-arm-cde-builtin-codegen" }, true);
        clang_tablegen(Basic, "include/clang/Basic/arm_cde.td",
            "clang/Basic/arm_cde_builtin_sema.inc", { "-gen-arm-cde-builtin-sema" }, true);
        clang_tablegen(Basic, "include/clang/Basic/arm_cde.td",
            "clang/Basic/arm_cde_builtin_aliases.inc", { "-gen-arm-cde-builtin-aliases" }, true);

        // riscv
        clang_tablegen(Basic, "include/clang/Basic/riscv_vector.td",
            "clang/Basic/riscv_vector_builtins.inc", { "-gen-riscv-vector-builtins" }, true);
        clang_tablegen(Basic, "include/clang/Basic/riscv_vector.td",
            "clang/Basic/riscv_vector_builtin_cg.inc", { "-gen-riscv-vector-builtin-codegen" }, true);
        clang_tablegen(Basic, "include/clang/Basic/riscv_vector.td",
            "clang/Basic/riscv_vector_builtin_sema.inc", { "-gen-riscv-vector-builtin-sema" }, true);

        clang_tablegen(Basic, "include/clang/Basic/riscv_sifive_vector.td",
            "clang/Basic/riscv_sifive_vector_builtins.inc", { "-gen-riscv-sifive-vector-builtins" }, true);
        clang_tablegen(Basic, "include/clang/Basic/riscv_sifive_vector.td",
            "clang/Basic/riscv_sifive_vector_builtin_cg.inc", { "-gen-riscv-sifive-vector-builtin-codegen" }, true);
        clang_tablegen(Basic, "include/clang/Basic/riscv_sifive_vector.td",
            "clang/Basic/riscv_sifive_vector_builtin_sema.inc", { "-gen-riscv-sifive-vector-builtin-sema" }, true);

        // AST

        {
            clang_tablegen(Basic, "include/clang/Basic/Attr.td",
                "clang/AST/Attrs.inc", { "-gen-clang-attr-classes" }, true);
            clang_tablegen(Basic, "include/clang/Basic/Attr.td",
                "clang/AST/AttrImpl.inc", { "-gen-clang-attr-impl" }, true);
            clang_tablegen(Basic, "include/clang/Basic/Attr.td",
                "clang/AST/AttrTextNodeDump.inc", { "-gen-clang-attr-text-node-dump" }, true);
            clang_tablegen(Basic, "include/clang/Basic/Attr.td",
                "clang/AST/AttrNodeTraverse.inc", { "-gen-clang-attr-node-traverse" }, true);
            clang_tablegen(Basic, "include/clang/Basic/Attr.td",
                "clang/AST/AttrVisitor.inc", { "-gen-clang-attr-ast-visitor" }, true);
            //
            clang_tablegen(Basic, "include/clang/Basic/Attr.td",
                // ?
                "clang/AST/AttrDocTable.inc", { "-gen-clang-attr-doc-table" }, true);
            clang_tablegen(Basic, "include/clang/Basic/Attr.td",
                // ?
                "AttrDocTable.inc", { "-gen-clang-attr-doc-table" }, true);

            clang_tablegen(Basic, "include/clang/Basic/StmtNodes.td",
                "clang/AST/StmtNodes.inc", { "-gen-clang-stmt-nodes" }, true);
            clang_tablegen(Basic, "include/clang/Basic/DeclNodes.td",
                "clang/AST/DeclNodes.inc", { "-gen-clang-decl-nodes" }, true);
            clang_tablegen(Basic, "include/clang/Basic/TypeNodes.td",
                "clang/AST/TypeNodes.inc", { "-gen-clang-type-nodes" }, true);
            clang_tablegen(Basic, "include/clang/AST/PropertiesBase.td",
                "clang/AST/AbstractBasicReader.inc", { "-gen-clang-basic-reader" }, true);
            clang_tablegen(Basic, "include/clang/AST/PropertiesBase.td",
                "clang/AST/AbstractBasicWriter.inc", { "-gen-clang-basic-writer" }, true);
            clang_tablegen(Basic, "include/clang/AST/TypeProperties.td",
                "clang/AST/AbstractTypeReader.inc", { "-gen-clang-type-reader" }, true);
            clang_tablegen(Basic, "include/clang/AST/TypeProperties.td",
                "clang/AST/AbstractTypeWriter.inc", { "-gen-clang-type-writer" }, true);
            clang_tablegen(Basic, "include/clang/Basic/CommentNodes.td",
                "clang/AST/CommentNodes.inc", { "-gen-clang-comment-nodes" }, true);

            clang_tablegen(Basic, "include/clang/AST/CommentHTMLTags.td",
                "clang/AST/CommentHTMLTags.inc", { "-gen-clang-comment-html-tags" }, true);
            clang_tablegen(Basic, "include/clang/AST/CommentHTMLTags.td",
                "clang/AST/CommentHTMLTagsProperties.inc", { "-gen-clang-comment-html-tags-properties" }, true);

            clang_tablegen(Basic, "include/clang/AST/CommentHTMLNamedCharacterReferences.td",
                "clang/AST/CommentHTMLNamedCharacterReferences.inc", { "-gen-clang-comment-html-named-character-references" }, true);
            clang_tablegen(Basic, "include/clang/AST/CommentCommands.td",
                "clang/AST/CommentCommandInfo.inc", { "-gen-clang-comment-command-info" }, true);
            clang_tablegen(Basic, "include/clang/AST/CommentCommands.td",
                "clang/AST/CommentCommandList.inc", { "-gen-clang-comment-command-list" }, true);

            clang_tablegen(Basic, "include/clang/AST/StmtDataCollectors.td",
                "clang/AST/StmtDataCollectors.inc", { "-gen-clang-data-collectors" }, true);
        }

        // Sema

        clang_tablegen(Basic, "include/clang/Basic/Attr.td",
            "clang/Sema/AttrTemplateInstantiate.inc", { "-gen-clang-attr-template-instantiate" }, true);
        clang_tablegen(Basic, "include/clang/Basic/Attr.td",
            "clang/Sema/AttrParsedAttrList.inc", { "-gen-clang-attr-parsed-attr-list" }, true);
        clang_tablegen(Basic, "include/clang/Basic/Attr.td",
            "clang/Sema/AttrParsedAttrKinds.inc", { "-gen-clang-attr-parsed-attr-kinds" }, true);
        clang_tablegen(Basic, "include/clang/Basic/Attr.td",
            "clang/Sema/AttrSpellingListIndex.inc", { "-gen-clang-attr-spelling-index" }, true);
        clang_tablegen(Basic, "include/clang/Basic/Attr.td",
            "clang/Sema/AttrParsedAttrImpl.inc", { "-gen-clang-attr-parsed-attr-impl" }, true);

        // Parse

        clang_tablegen(Basic, "include/clang/Basic/Attr.td",
            "clang/Parse/AttrParserStringSwitches.inc", { "-gen-clang-attr-parser-string-switches" }, true);
        clang_tablegen(Basic, "include/clang/Basic/Attr.td",
            "clang/Parse/AttrSubMatchRulesParserStringSwitches.inc", { "-gen-clang-attr-subject-match-rules-parser-string-switches" }, true);

        // Driver

        auto UTablegen = Basic.constructThisPackageDependency("llvm_project.llvm.Utils.Tablegen");
        targets.tablegen(UTablegen, Basic, "include/clang/Driver/Options.td",
            "clang/Driver/Options.inc", { "-gen-opt-parser-defs" }, true,
            { Basic.getFile(*targets.support, "include") }
        );

        // Serialization

        clang_tablegen(Basic, "include/clang/Basic/Attr.td",
            "clang/Serialization/AttrPCHRead.inc", { "-gen-clang-attr-pch-read" }, true);
        clang_tablegen(Basic, "include/clang/Basic/Attr.td",
            "clang/Serialization/AttrPCHWrite.inc", { "-gen-clang-attr-pch-write" }, true);

        // StaticAnalyzer.Checkers

        clang_tablegen(Basic, "include/clang/StaticAnalyzer/Checkers/Checkers.td",
            "clang/StaticAnalyzer/Checkers/Checkers.inc", { "-gen-clang-sa-checkers" }, true);
    }

    auto &Driver = clang.template addTarget<StaticLibraryTarget>("Driver");
    Driver += "lib/Driver/.*"_rr;
    Driver += "lib/Driver"_idir;
    Driver.Public += Basic;
    if (Driver.getBuildSettings().TargetOS.Type == OSType::Windows)
    {
        Driver.Public += "comsuppw.lib"_slib;
        //Driver.Public += "comsuppwd.lib"_slib; // for debug
        Driver.Public += "OleAut32.lib"_slib;
        Driver.Public += "version.lib"_slib;
    }

    auto &Lex = clang.template addTarget<StaticLibraryTarget>("Lex");
    Lex += "lib/Lex/.*"_rr;
    Lex.Public += Basic;

    auto &APINotes = clang.template addTarget<StaticLibraryTarget>("APINotes");
    APINotes += "lib/APINotes/.*"_rr;
    APINotes.Public += Basic;
    APINotes.Public +=
        APINotes.constructThisPackageDependency("llvm_project.llvm.BitReader")
        ;

    auto &Rewrite = clang.template addTarget<StaticLibraryTarget>("Rewrite");
    Rewrite += "lib/Rewrite/.*"_rr;
    Rewrite.Public += Lex;

    auto &AST = clang.template addTarget<StaticLibraryTarget>("AST");
    AST += "lib/AST/.*"_rr;
    AST.Public += Lex, AST.constructThisPackageDependency("llvm_project.llvm.BinaryFormat");
    clang_tablegen(AST, "lib/AST/Interp/Opcodes.td", "Opcodes.inc", { "-gen-clang-opcodes" }, true);

    auto &ASTMatchers = clang.template addTarget<StaticLibraryTarget>("AST.Matchers");
    ASTMatchers += "lib/ASTMatchers/[^/]*"_rr;
    ASTMatchers.Public += AST;

    auto &Edit = clang.template addTarget<StaticLibraryTarget>("Edit");
    Edit += "lib/Edit/.*"_rr;
    Edit.Public += AST;

    auto &Clang_Analysis = clang.template addTarget<StaticLibraryTarget>("Analysis");
    Clang_Analysis += "lib/Analysis/.*"_rr;
    Clang_Analysis.Public += AST;
    {
        Clang_Analysis.addCommand()
            << cmd::wdir("lib/Analysis/FlowSensitive")
            << cmd::prog("org.sw.demo.python.exe"_dep)
            << cmd::in("utils/bundle_resources.py")
            << cmd::out("HTMLLogger.inc")
            << cmd::in("lib/Analysis/FlowSensitive/HTMLLogger.html")
            << cmd::in("lib/Analysis/FlowSensitive/HTMLLogger.css")
            << cmd::in("lib/Analysis/FlowSensitive/HTMLLogger.js")
        ;
        //add_custom_target(clangAnalysisFlowSensitiveResources DEPENDS HTMLLogger.inc)
    }

    auto &Sema = clang.template addTarget<StaticLibraryTarget>("Sema");
    Sema += "lib/Sema/.*"_rr;
    Sema.Public += Clang_Analysis, Edit, Support;
    clang_tablegen(Sema, "lib/Sema/OpenCLBuiltins.td", "OpenCLBuiltins.inc", { "-gen-clang-opencl-builtins" }, true);

    auto &Parse = clang.template addTarget<StaticLibraryTarget>("Parse");
    Parse += "lib/Parse/.*"_rr;
    Parse.Public += Sema;

    auto &Serialization = clang.template addTarget<StaticLibraryTarget>("Serialization");
    Serialization += "lib/Serialization/.*"_rr;
    Serialization.Public += Sema;

    auto &Frontend = clang.template addTarget<StaticLibraryTarget>("Frontend");
    Frontend += "lib/Frontend/[^/]*"_rr;
    Frontend.Public += Serialization, Parse, Driver,
        Frontend.constructThisPackageDependency("llvm_project.llvm.BitReader"),
        Frontend.constructThisPackageDependency("llvm_project.llvm.Option"),
        Frontend.constructThisPackageDependency("llvm_project.llvm.ProfileData")
        ;

    auto &RewriteFrontend = clang.template addTarget<StaticLibraryTarget>("RewriteFrontend");
    RewriteFrontend += "lib/Frontend/Rewrite/.*"_rr;
    RewriteFrontend.Public += Frontend, Rewrite;

    auto &FrontendTool = clang.template addTarget<StaticLibraryTarget>("FrontendTool");
    FrontendTool += "lib/FrontendTool/[^/]*"_rr;
    FrontendTool.Public += Basic;
    //FrontendTool.Public += FrontendTool.constructThisPackageDependency("llvm_project.llvm.Frontend.OpenMP");

    auto &ToolingCore = clang.template addTarget<StaticLibraryTarget>("Tooling.Core");
    ToolingCore += "lib/Tooling/Core/.*"_rr;
    ToolingCore.Public += Rewrite, AST;

    auto &ToolingInclusions = clang.template addTarget<StaticLibraryTarget>("Tooling.Inclusions");
    ToolingInclusions += "lib/Tooling/Inclusions/.*"_rr;
    ToolingInclusions.Public += ToolingCore;

    auto &Format = clang.template addTarget<StaticLibraryTarget>("Format");
    Format += "lib/Format/.*"_rr;
    Format.Public += ToolingInclusions;

    auto &Tooling = clang.template addTarget<StaticLibraryTarget>("Tooling");
    Tooling += "lib/Tooling/.*"_r;
    Tooling.Public += ToolingCore, Frontend, Format, ASTMatchers;
    // TODO: implement real preproc in release mode
    Tooling.configureFile("lib/Tooling/EmptyNodeIntrospection.inc.in", "clang/Tooling/NodeIntrospection.inc");

    auto &Index = clang.template addTarget<StaticLibraryTarget>("Index");
    Index += "lib/Index/.*"_rr;
    Index.Public += ToolingCore, Format, Frontend, Rewrite;

    auto &ExtractAPI = clang.template addTarget<StaticLibraryTarget>("ExtractAPI");
    ExtractAPI += "lib/ExtractAPI/.*"_rr;
    ExtractAPI.Public += Frontend, Rewrite;

    auto &Clang_CodeGen = clang.template addTarget<StaticLibraryTarget>("CodeGen");
    Clang_CodeGen += "lib/CodeGen/.*"_rr;
    Clang_CodeGen.Public += "include"_idir;
    Clang_CodeGen += "lib/CodeGen"_idir;
    Clang_CodeGen.Public += Frontend;
    for (auto d : { "Analysis", "BitReader", "BitWriter", "Core",
        "Coroutines", "Coverage", "Frontend.OpenMP", "Frontend.Offloading", "Frontend.Driver", "ipo", "IRReader", "InstCombine", "Instrumentation", "LTO",
        "Linker", "MC", "ObjCARCOpts", "Object", "Passes", "ProfileData", "ScalarOpts",
        "Support", "Target", "TransformUtils", "HipStdPar" })
    {
        Clang_CodeGen.Public += Clang_CodeGen.constructThisPackageDependency("llvm_project.llvm."s + d);
    }

    auto &libclang = clang.template addTarget<LibraryTarget>("tools.libclang");
    libclang += "tools/libclang/.*"_rr;
    libclang += sw::Shared, "CINDEX_EXPORTS"_def;
    libclang.Public += Clang_CodeGen, FrontendTool, Index, Tooling, ExtractAPI, APINotes;
    for (auto &[n,v] : targets.arch_targets)
    {
        for (auto &t : v)
            libclang.Public += *t;
    }

    auto &clang_clang = clang.template addTarget<ExecutableTarget>("tools.clang");
    clang_clang += "tools/driver/.*"_rr;
    clang_clang += "clang_main=main"_def;
    clang_clang.Public += Clang_CodeGen, FrontendTool, Index, Tooling, RewriteFrontend, ExtractAPI, APINotes;
    for (auto &[n, v] : targets.arch_targets)
    {
        for (auto &t : v)
            clang_clang.Public += *t;
    }
}

void build_libs(auto &&llvm_project)
{
    auto is_musl = [](auto &t)
    {
        return
            t.getSettings()["native"]["stdlib"]["c"].isValue() &&
            sw::UnresolvedPackage(t.getSettings()["native"]["stdlib"]["c"].getValue()).getPath() == "org.sw.demo.musl";
    };

    auto check_compiler = [](auto &t)
    {
        if (t.getCompilerType() == CompilerType::MSVC)
            t.HeaderOnly = true;
    };

    auto &builtins = llvm_project.addLibrary("compiler_rt.builtins");
    {
        auto &b = builtins;
        check_compiler(b);
        b.setRootDirectory("compiler-rt");

        {
            std::set<String> arch_list
            {
                "aarch64",
                "arm",
                "i386",
                "microblaze",
                "mips",
                "mipsn32",
                "mips64",
                "m68k",
                "or1k",
                "powerpc",
                "powerpc64",
                "s390x",
                "sh",
                "x32",
                "x86_64",
            };

            String arch;
            switch (b.getBuildSettings().TargetOS.Arch)
            {
            case ArchType::x86_64:
                arch = toString(b.getBuildSettings().TargetOS.Arch);
                break;
            case ArchType::x86:
                arch = "i386";
                break;
            case ArchType::arm:
                arch = "arm";
            case ArchType::aarch64:
                arch = "aarch64";
                break;
            default:
                throw SW_RUNTIME_ERROR("arch is not implemented");
            }

            arch_list.erase(arch);

            // filter out same .c files
            auto filter_out = [](auto &t)
            {
                for (const auto &[p, sf] : t[".*\\.[csS]"_rr])
                {
                    if (sf->skip)
                        continue;
                    auto p2 = p.parent_path().parent_path() / (p.stem() += ".c");
                    if (t.check_absolute(p2, true))
                        t -= p2;
                }
            };

            b.ExportAllSymbols = true;
            b += "lib/builtins/.*"_r;
            b += FileRegex("lib/builtins", arch + "/.*", true);
            b -= "lib/builtins/gcc_personality_v0.c";
            filter_out(b);
        }
    }

    auto &libunwind = llvm_project.addLibrary("libunwind");
    {
        auto &t = libunwind;
        check_compiler(t);
        t.setRootDirectory("libunwind");

        t += "include/.*"_rr;
        t += "src/.*"_rr;

        if (!t.getBuildSettings().TargetOS.isApple())
        {
            //t -= "src/Unwind_AppleExtras.cpp";
        }

        if (!is_musl(t))
        {
            t += "dl"_slib;
            t += "pthread"_slib;
        }

        /*if (t.getBuildSettings().TargetOS.isApple())
        {
        if (t.getCompilerType() == CompilerType::Clang)
        {
        t.CompileOptions.push_back("-fno-rtti");
        t.CompileOptions.push_back("-fno-exceptions");
        t.CompileOptions.push_back("-funwind-tables");
        }
        t.CompileOptions.push_back("-fvisibility=default");
        }*/

        // less size?
        //t += "_LIBUNWIND_IS_NATIVE_ONLY"_def;

        // need manual .eh_handle sections
        //t += "_LIBUNWIND_IS_BAREMETAL"_def;
    }

    auto &libcxxabi = llvm_project.addLibrary("libcxxabi");

    auto &libcxx = llvm_project.addLibrary("libcxx");
    {
        auto &t = libcxx;
        check_compiler(t);
        t.setRootDirectory("libcxx");

        t += cpp17;
        t -= "src/support/.*"_rr;

        t += "_LIBCPP_BUILDING_LIBRARY"_def;
        //t += "_LIBCXXABI_BUILDING_LIBRARY"_def;
        t += "LIBCXX_BUILDING_LIBCXXABI"_def;
        if (t.getBuildSettings().TargetOS.isApple())
        {
            t.Public += "_LIBCPP_DISABLE_AVAILABILITY"_def;
            t.CompileOptions.push_back("-fvisibility=default");
        }

        if (!is_musl(t))
            t += "pthread"_slib;

        if (is_musl(t))
        {
            t.Public += "_LIBCPP_HAS_MUSL_LIBC"_def;
            //t.Public += "_LIBCPP_HAS_THREAD_API_PTHREAD"_def;
        }

        t.Public += libcxxabi;
        t.Interface += libunwind;
        if (t.getBuildSettings().TargetOS.isApple())
        {
            t.Interface -= libcxxabi;
            t.Interface -= libunwind;
        }

        if (isClangFamily(t.getCompilerType()))
        {
            t.Public.CompileOptions.push_back("-nostdinc++");
            t.Public.LinkOptions.push_back("-nostdlib++");
        }
    }

    // libcxxabi
    {
        auto &t = libcxxabi;
        check_compiler(t);
        t.setRootDirectory("libcxxabi");

        t += cpp17;

        t -= "src/cxa_noexception.cpp";
        t += "_LIBCXXABI_BUILDING_LIBRARY"_def;
        //t += "LIBCXXABI_USE_LLVM_UNWINDER"_def;
        t += libcxx;
        if (t.getBuildSettings().TargetOS.isApple())
        {
            //t.CompileOptions.push_back("-fvisibility=default");
            //t.CompileOptions.push_back("-fvisibility-global-new-delete-hidden");
        }
    }
}

//#include <iostream>
template <typename T>
struct target : T {
    using T::T;

    path new_root_dir;

    template <typename U>
    auto &addTarget(auto &&name) {
        auto &t = T::template addTarget<target<U>>(name);
        t += cpp17;
        if (!new_root_dir.empty()) {
            t.new_root_dir = new_root_dir;
            t.setRootDirectory(path{} / new_root_dir);
        }
        return t;
    }
};
template <typename T>
struct project {
    T &t;
    path dir;
    std::vector<std::any> children;

    template <typename U>
    auto &add(auto &&what, auto &&name) {
        project<U> p{what};
        p.dir = !dir.empty() ? dir : path{name};
        auto &ref = children.emplace_back(p);
        return *std::any_cast<project<U>>(&ref);
    }
    auto &addProject(auto &&name) {
        return add<ProjectTarget>(t.addProject(name), name);
    }
    auto &addDirectory(auto &&name) {
        return add<DirectoryTarget>(t.addDirectory(name), name);
    }
    template <typename U>
    auto &addTarget(auto &&name) {
        project<target<U>> p{t.template addTarget<target<U>>(name)};
        p.t += cpp17;
        auto &ref = p.t;
        children.push_back(p);
        if (!dir.empty()) {
            p.t.new_root_dir = dir;
            ref.setRootDirectory(path{} / dir);
        }
        return ref;
    }
    auto &addLibrary(auto && ... args) {
        return addTarget<LibraryTarget>(args...);
    }
};

void build(Solution &s)
{
    project<sw::ProjectTarget> llvm_project{s.addProject("llvm_project", "18.1.7")};
    llvm_project.t += Git("https://github.com/llvm/llvm-project", "llvmorg-{v}");
    LlvmTargets targets;
    build_llvm(llvm_project, targets);
    build_clang(llvm_project, targets);
    build_libs(llvm_project);
}

void check(Checker &c)
{
    auto &s = c.addSet("support");
    s.checkFunctionExists("_alloca");
    s.checkFunctionExists("__alloca");
    s.checkFunctionExists("__ashldi3");
    s.checkFunctionExists("__ashrdi3");
    s.checkFunctionExists("__chkstk");
    s.checkFunctionExists("__chkstk_ms");
    s.checkFunctionExists("__cmpdi2");
    s.checkFunctionExists("__divdi3");
    s.checkFunctionExists("__fixdfdi");
    s.checkFunctionExists("__fixsfdi");
    s.checkFunctionExists("__floatdidf");
    s.checkFunctionExists("__lshrdi3");
    s.checkFunctionExists("__main");
    s.checkFunctionExists("__moddi3");
    s.checkFunctionExists("__udivdi3");
    s.checkFunctionExists("__umoddi3");
    s.checkFunctionExists("___chkstk");
    s.checkFunctionExists("___chkstk_ms");
    s.checkIncludeExists("CrashReporterClient.h");
    s.checkIncludeExists("dirent.h");
    s.checkIncludeExists("dlfcn.h");
    s.checkIncludeExists("errno.h");
    s.checkIncludeExists("fcntl.h");
    s.checkIncludeExists("fenv.h");
    s.checkIncludeExists("histedit.h");
    s.checkIncludeExists("inttypes.h");
    s.checkIncludeExists("link.h");
    s.checkIncludeExists("linux/magic.h");
    s.checkIncludeExists("linux/nfs_fs.h");
    s.checkIncludeExists("linux/smb.h");
    s.checkIncludeExists("mach/mach.h");
    s.checkIncludeExists("malloc.h");
    s.checkIncludeExists("malloc/malloc.h");
    s.checkIncludeExists("ndir.h");
    s.checkIncludeExists("pthread.h");
    s.checkIncludeExists("signal.h");
    s.checkIncludeExists("stdint.h");
    s.checkIncludeExists("sys/dir.h");
    s.checkIncludeExists("sys/ioctl.h");
    s.checkIncludeExists("sys/mman.h");
    s.checkIncludeExists("sys/ndir.h");
    s.checkIncludeExists("sys/param.h");
    s.checkIncludeExists("sys/resource.h");
    s.checkIncludeExists("sys/stat.h");
    s.checkIncludeExists("sys/time.h");
    s.checkIncludeExists("sys/types.h");
    s.checkIncludeExists("sys/uio.h");
    s.checkIncludeExists("termios.h");
    s.checkIncludeExists("unistd.h");
    s.checkIncludeExists("valgrind/valgrind.h");
    s.checkTypeSize("int64_t");
    s.checkTypeSize("size_t");
    s.checkTypeSize("uint64_t");
    s.checkTypeSize("u_int64_t");
    s.checkTypeSize("void *");
    {
        auto &c = s.checkSymbolExists("dladdr");
        c.Parameters.Includes.push_back("dlfcn.h");
    }
    {
        auto &c = s.checkSymbolExists("dlopen");
        c.Parameters.Includes.push_back("dlfcn.h");
    }
    {
        auto &c = s.checkSymbolExists("futimens");
        c.Parameters.Includes.push_back("sys/stat.h");
    }
    {
        auto &c = s.checkSymbolExists("futimes");
        c.Parameters.Includes.push_back("sys/time.h");
    }
    {
        auto &c = s.checkSymbolExists("getcwd");
        c.Parameters.Includes.push_back("unistd.h");
    }
    {
        auto &c = s.checkSymbolExists("getpagesize");
        c.Parameters.Includes.push_back("unistd.h");
    }
    {
        auto &c = s.checkSymbolExists("getrlimit");
        c.Parameters.Includes.push_back("sys/types.h");
        c.Parameters.Includes.push_back("sys/time.h");
        c.Parameters.Includes.push_back("sys/resource.h");
    }
    {
        auto &c = s.checkSymbolExists("getrusage");
        c.Parameters.Includes.push_back("sys/resource.h");
    }
    {
        auto &c = s.checkSymbolExists("gettimeofday");
        c.Parameters.Includes.push_back("sys/time.h");
    }
    {
        auto &c = s.checkSymbolExists("isatty");
        c.Parameters.Includes.push_back("unistd.h");
    }
    {
        auto &c = s.checkSymbolExists("lseek64");
        c.Parameters.Includes.push_back("sys/types.h");
        c.Parameters.Includes.push_back("unistd.h");
    }
    {
        auto &c = s.checkSymbolExists("mallctl");
        c.Parameters.Includes.push_back("malloc_np.h");
    }
    {
        auto &c = s.checkSymbolExists("mallinfo");
        c.Parameters.Includes.push_back("malloc.h");
    }
    {
        auto &c = s.checkSymbolExists("malloc_zone_statistics");
        c.Parameters.Includes.push_back("malloc/malloc.h");
    }
    {
        auto &c = s.checkSymbolExists("mkdtemp");
        c.Parameters.Includes.push_back("stdlib.h");
        c.Parameters.Includes.push_back("unistd.h");
    }
    {
        auto &c = s.checkSymbolExists("mkstemp");
        c.Parameters.Includes.push_back("stdlib.h");
        c.Parameters.Includes.push_back("unistd.h");
    }
    {
        auto &c = s.checkSymbolExists("mktemp");
        c.Parameters.Includes.push_back("stdlib.h");
        c.Parameters.Includes.push_back("unistd.h");
    }
    {
        auto &c = s.checkSymbolExists("posix_fallocate");
        c.Parameters.Includes.push_back("fcntl.h");
    }
    {
        auto &c = s.checkSymbolExists("posix_spawn");
        c.Parameters.Includes.push_back("spawn.h");
    }
    {
        auto &c = s.checkSymbolExists("pread");
        c.Parameters.Includes.push_back("unistd.h");
    }
    {
        auto &c = s.checkSymbolExists("realpath");
        c.Parameters.Includes.push_back("stdlib.h");
    }
    {
        auto &c = s.checkSymbolExists("sbrk");
        c.Parameters.Includes.push_back("unistd.h");
    }
    {
        auto &c = s.checkSymbolExists("setenv");
        c.Parameters.Includes.push_back("stdlib.h");
    }
    {
        auto &c = s.checkSymbolExists("setrlimit");
        c.Parameters.Includes.push_back("sys/resource.h");
    }
    {
        auto &c = s.checkSymbolExists("sigaltstack");
        c.Parameters.Includes.push_back("signal.h");
    }
    {
        auto &c = s.checkSymbolExists("strerror");
        c.Parameters.Includes.push_back("string.h");
    }
    {
        auto &c = s.checkSymbolExists("strerror_r");
        c.Parameters.Includes.push_back("string.h");
    }
    {
        auto &c = s.checkSymbolExists("strtoll");
        c.Parameters.Includes.push_back("stdlib.h");
    }
    {
        auto &c = s.checkSymbolExists("sysconf");
        c.Parameters.Includes.push_back("unistd.h");
    }
    {
        auto &c = s.checkSymbolExists("writev");
        c.Parameters.Includes.push_back("sys/uio.h");
    }
    {
        auto &c = s.checkSymbolExists("_chsize_s");
        c.Parameters.Includes.push_back("io.h");
    }
    {
        auto &c = s.checkSymbolExists("_Unwind_Backtrace");
        c.Parameters.Includes.push_back("unwind.h");
    }
    {
        auto &c = s.checkSymbolExists("__GLIBC__");
        c.Parameters.Includes.push_back("stdio.h");
    }
}
