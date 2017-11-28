void build(Solution &sln)
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

    auto &s = sln.addDirectory("llvm_project");
    auto &llvm = s.addProject("llvm", "master");
    llvm.Source = Git("https://github.com/llvm-mirror/llvm", "", "{v}");

    auto &demangle = llvm.addTarget<StaticLibraryTarget>("Demangle");
    demangle += "include/llvm/Demangle/.*"_rr;
    demangle += "lib/Demangle/.*"_rr;

    const StringSet all_targets{
        "AArch64",
        "AMDGPU",
        "ARM",
        "BPF",
        "Hexagon",
        "Lanai",
        "Mips",
        "MSP430",
        "NVPTX",
        "PowerPC",
        "RISCV",
        "Sparc",
        "SystemZ",
        "X86",
        "XCore",
    };
    const auto selected_targets = all_targets;

    //
    auto &Support = llvm.addTarget<StaticLibraryTarget>("Support");
    {
        Support.setChecks("support");

        Support +=
            "cmake/config.guess",
            "include/.*"_rr,
            "lib/Support/.*"_rr;
        Support -=
            "include/.*\\.def"_rr,
            "lib/Support/.*\\.def"_rr;

        Support.Private +=
            "lib"_id;

        Support.Public +=
            "include"_id;

        Support.Public += demangle;
        Support.Public += "pub.cppan2.demo.madler.zlib-1"_dep;
        Support.Public += "pub.cppan2.demo.libxml2-2.9.7"_dep;

        Support.Variables["LLVM_ENABLE_ZLIB"] = "1";
        Support.Variables["LLVM_LIBXML2_ENABLED"] = "1";

        for (auto &t : selected_targets)
        {
            Support.Variables["LLVM_ENUM_ASM_PRINTERS"] += "LLVM_ASM_PRINTER(" + t + ")\n";
            if (!(t == "MSP430" || t == "NVPTX" || t == "XCore"))
                Support.Variables["LLVM_ENUM_ASM_PARSERS"] += "LLVM_ASM_PARSER(" + t + ")\n";
            Support.Variables["LLVM_ENUM_DISASSEMBLERS"] += "LLVM_DISASSEMBLER(" + t + ")\n";
            Support.Variables["LLVM_ENUM_TARGETS"] += "LLVM_TARGET(" + t + ")\n";
        }

        Support.Variables["LLVM_DEFAULT_TARGET_TRIPLE"] = "x86_64-pc-win32";
        Support.Variables["LLVM_ENABLE_THREADS"] = "1";
        Support.Variables["LLVM_HAS_ATOMICS"] = "1";
        Support.Variables["LLVM_HOST_TRIPLE"] = "\"x86_64-pc-win32\"";
        Support.Variables["LLVM_NATIVE_ARCH"] = "X86";

        Support.Variables["LLVM_NATIVE_ASMPARSER"] = "LLVMInitializeX86AsmParser";
        Support.Variables["LLVM_NATIVE_ASMPRINTER"] = "LLVMInitializeX86AsmPrinter";
        Support.Variables["LLVM_NATIVE_DISASSEMBLER"] = "LLVMInitializeX86Disassembler";
        Support.Variables["LLVM_NATIVE_TARGET"] = "LLVMInitializeX86Target";
        Support.Variables["LLVM_NATIVE_TARGETINFO"] = "LLVMInitializeX86TargetInfo";
        Support.Variables["LLVM_NATIVE_TARGETMC"] = "LLVMInitializeX86TargetMC";

        if (s.Settings.TargetOS.Type == OSType::Windows)
            Support.Variables["LLVM_ON_WIN32"] = "1";
        else
            Support.Variables["LLVM_ON_UNIX"] = "1";

        Support.Variables["LLVM_VERSION_MAJOR"] = Support.Variables["PACKAGE_VERSION_MAJOR"];
        Support.Variables["LLVM_VERSION_MINOR"] = Support.Variables["PACKAGE_VERSION_MINOR"];
        Support.Variables["LLVM_VERSION_PATCH"] = Support.Variables["PACKAGE_VERSION_PATCH"];
        Support.replaceInFileOnce("include/llvm/Config/llvm-config.h.cmake", "\"${PACKAGE_VERSION}\"", "${PACKAGE_VERSION}");
        Support.replaceInFileOnce("include/llvm/Config/config.h.cmake", "\"${PACKAGE_VERSION}\"", "${PACKAGE_VERSION}");

        Support.Variables["BUG_REPORT_URL"] = "http://llvm.org/bugs/";
        Support.Variables["PACKAGE_BUGREPORT"] = "http://llvm.org/bugs/";
        Support.Variables["ENABLE_BACKTRACES"] = "1";
        Support.Variables["ENABLE_CRASH_OVERRIDES"] = "1";
        Support.Variables["BACKTRACE_HEADER"] = "execinfo.h";

        Support.Variables["HAVE_DECL_ARC4RANDOM"] = "1";
        Support.Variables["HAVE_DECL_FE_ALL_EXCEPT"] = "1";
        Support.Variables["HAVE_DECL_FE_INEXACT"] = "1";
        Support.Variables["HAVE_DECL_STRERROR_S"] = "1";
        Support.Variables["LLVM_ENABLE_DIA_SDK"] = "0"; // todo

        Support.Variables["LLVM_VERSION_PRINTER_SHOW_HOST_TARGET_INFO"] = "1";
        Support.Variables["LTDL_SHLIB_EXT"] = ".dll";
        Support.Variables["RETSIGTYPE"] = "void";
        Support.Variables["stricmp"] = "_stricmp";
        Support.Variables["strdup"] = "_strdup";

        Support.configureFile("include/llvm/Config/AsmPrinters.def.in", "llvm/Config/AsmPrinters.def");
        Support.configureFile("include/llvm/Config/AsmParsers.def.in", "llvm/Config/AsmParsers.def");
        Support.configureFile("include/llvm/Config/Disassemblers.def.in", "llvm/Config/Disassemblers.def");
        Support.configureFile("include/llvm/Config/Targets.def.in", "llvm/Config/Targets.def");

        Support.configureFile("include/llvm/Config/config.h.cmake", "llvm/Config/config.h");
        Support.configureFile("include/llvm/Config/llvm-config.h.cmake", "llvm/Config/llvm-config.h");
        Support.configureFile("include/llvm/Config/abi-breaking.h.cmake", "llvm/Config/abi-breaking.h");
        Support.configureFile("include/llvm/Support/DataTypes.h.cmake", "llvm/Support/DataTypes.h");
        Support.writeFileOnce("llvm/Support/VCSRevision.h", true);
    }

    //
    auto &Tablegen = llvm.addTarget<StaticLibraryTarget>("Tablegen");
    Tablegen += "lib/TableGen/.*"_rr;
    Tablegen.Public += Support;

    //
    auto &UTablegen = llvm.addTarget<ExecutableTarget>("Utils.Tablegen");
    UTablegen += "utils/TableGen/.*"_rr;
    UTablegen += "lib/Target/X86/Disassembler/X86DisassemblerDecoderCommon.h";
    UTablegen.Public += Tablegen;

    auto tablegen = [&UTablegen](auto &t, const std::string &in,
        const std::string &out, const Strings &args,
        bool add_target_idir = false, const Files idirs = Files())
    {
        (t + UTablegen)->Dummy = true;

        auto c = std::make_shared<Command>();
        c->setProgram(UTablegen);
        for (auto &a : args)
            c->args.push_back(a);
        c->args.push_back("-I");
        c->args.push_back(t.SourceDir.string());
        c->args.push_back("-I");
        c->args.push_back((t.SourceDir / "include").string());
        if (add_target_idir)
        {
            c->args.push_back("-I");
            c->args.push_back((t.SourceDir / in).parent_path().string());
        }
        for (auto &i : idirs)
        {
            c->args.push_back("-I");
            c->args.push_back(i.string());
        }
        c->args.push_back((t.SourceDir / in).string());
        c->args.push_back("-o");
        c->args.push_back((t.BinaryDir / out).string());

        c->addInput(t.SourceDir / in);
        c->addOutput(t.BinaryDir / out);

        t.Public += t.BinaryDir / out;

        fs::create_directories((t.BinaryDir / out).parent_path());
    };

    //
    auto &BinaryFormat = llvm.addTarget<StaticLibraryTarget>("BinaryFormat");
    BinaryFormat += "lib/BinaryFormat/.*"_rr;
    BinaryFormat.Public += Support;

    //
    auto &MC = llvm.addTarget<StaticLibraryTarget>("MC");
    MC += "lib/MC/[^/]*"_rr;
    MC.Public += Support;

    //
    auto &MCParser = MC.addTarget<StaticLibraryTarget>("Parser");
    MCParser += "lib/MC/MCParser/.*"_rr;
    MCParser.Public += MC;

    //
    auto &MCDisassembler = MC.addTarget<StaticLibraryTarget>("Disassembler");
    MCDisassembler += "lib/MC/MCDisassembler/.*"_rr;
    MCDisassembler.Public += MC;

    //
    auto &Core = llvm.addTarget<StaticLibraryTarget>("Core");
    Core += "include/llvm/IR/.*\\.td"_rr, "lib/IR/.*"_rr;
    Core.Public += Support, BinaryFormat;
    tablegen(Core, "lib/IR/AttributesCompatFunc.td", "AttributesCompatFunc.inc", { "-gen-attrs" });
    tablegen(Core, "include/llvm/IR/Attributes.td", "llvm/IR/Attributes.gen", { "-gen-attrs" });
    tablegen(Core, "include/llvm/IR/Intrinsics.td", "llvm/IR/Intrinsics.gen", { "-gen-intrinsic" });

    //
    auto &AsmParser = llvm.addTarget<StaticLibraryTarget>("AsmParser");
    AsmParser += "lib/AsmParser/.*"_rr;
    AsmParser.Public += Core;

    //
    auto &BitReader = llvm.addTarget<StaticLibraryTarget>("BitReader");
    BitReader += "lib/Bitcode/Reader/.*"_rr;
    BitReader.Public += Core;

    //
    auto &LineEditor = llvm.addTarget<StaticLibraryTarget>("LineEditor");
    LineEditor += "lib/LineEditor/.*"_rr;
    LineEditor.Public += Support;

    //
    auto &Option = llvm.addTarget<StaticLibraryTarget>("Option");
    Option += "lib/Option/.*"_rr;
    Option.Public += Support;

    //
    auto &ProfileData = llvm.addTarget<StaticLibraryTarget>("ProfileData");
    ProfileData += "lib/ProfileData/[^/]*"_rr;
    ProfileData.Public += Core;

    //
    auto &DebugInfoMSF = llvm.addTarget<StaticLibraryTarget>("DebugInfo.MSF");
    DebugInfoMSF += "lib/DebugInfo/MSF/.*"_rr;
    DebugInfoMSF.Public += Support;

    //
    auto &DebugInfoCodeView = llvm.addTarget<StaticLibraryTarget>("DebugInfo.CodeView");
    DebugInfoCodeView += "lib/DebugInfo/CodeView/.*"_rr;
    DebugInfoCodeView.Public += DebugInfoMSF;

    //
    auto &DebugInfoPDB = llvm.addTarget<StaticLibraryTarget>("DebugInfo.PDB");
    DebugInfoPDB += "lib/DebugInfo/PDB/.*"_rr;
    DebugInfoPDB -= "lib/DebugInfo/PDB/DIA/.*"_rr;
    DebugInfoPDB.Public += DebugInfoMSF, DebugInfoCodeView;

    //
    auto &Object = llvm.addTarget<StaticLibraryTarget>("Object");
    Object += "lib/Object/.*"_rr;
    Object.Public += BitReader, Core, MCParser;

    //
    auto &DebugInfoDWARF = llvm.addTarget<StaticLibraryTarget>("DebugInfo.DWARF");
    DebugInfoDWARF += "lib/DebugInfo/DWARF/.*"_rr;
    DebugInfoDWARF.Public += Object;

    //
    auto &Analysis = llvm.addTarget<StaticLibraryTarget>("Analysis");
    Analysis += "lib/Analysis/.*"_rr;
    Analysis.Public += Object, ProfileData;

    //
    auto &BitWriter = llvm.addTarget<StaticLibraryTarget>("BitWriter");
    BitWriter += "lib/Bitcode/Writer/.*"_rr;
    BitWriter.Public += Analysis;

    //
    auto &TransformUtils = llvm.addTarget<StaticLibraryTarget>("TransformUtils");
    TransformUtils += "lib/Transforms/Utils/.*"_rr;
    TransformUtils.Public += Analysis;

    //
    auto &Linker = llvm.addTarget<StaticLibraryTarget>("Linker");
    Linker += "lib/Linker/.*"_rr;
    Linker.Public += TransformUtils;

    //
    auto &InstCombine = llvm.addTarget<StaticLibraryTarget>("InstCombine");
    InstCombine += "lib/Transforms/InstCombine/.*"_rr;
    InstCombine.Public += TransformUtils;

    //
    auto &Instrumentation = llvm.addTarget<StaticLibraryTarget>("Instrumentation");
    Instrumentation += "lib/Transforms/Instrumentation/.*"_rr;
    Instrumentation.Public += TransformUtils;

    //
    auto &ScalarOpts = llvm.addTarget<StaticLibraryTarget>("ScalarOpts");
    ScalarOpts += "lib/Transforms/Scalar/.*"_rr;
    ScalarOpts.Public += InstCombine;

    //
    auto &Vectorize = llvm.addTarget<StaticLibraryTarget>("Vectorize");
    Vectorize += "lib/Transforms/Vectorize/.*"_rr;
    Vectorize.Public += TransformUtils;

    //
    auto &ObjCARCOpts = llvm.addTarget<StaticLibraryTarget>("ObjCARCOpts");
    ObjCARCOpts += "lib/Transforms/ObjCARC/.*"_rr;
    ObjCARCOpts.Public += TransformUtils;

    //
    auto &ObjectYAML = llvm.addTarget<StaticLibraryTarget>("ObjectYAML");
    ObjectYAML += "lib/ObjectYAML/.*"_rr;
    ObjectYAML.Public += DebugInfoCodeView;

    //
    auto &LibDriver = llvm.addTarget<StaticLibraryTarget>("LibDriver");
    LibDriver += "lib/ToolDrivers/llvm-lib/.*"_rr;
    LibDriver.Public += Object, Option;
    tablegen(LibDriver, "lib/ToolDrivers/llvm-lib/Options.td", "Options.inc", { "-gen-opt-parser-defs" });

    //
    auto &IRReader = llvm.addTarget<StaticLibraryTarget>("IRReader");
    IRReader += "lib/IRReader/.*"_rr;
    IRReader.Public += AsmParser, BitReader;

    //
    auto &ipo = llvm.addTarget<StaticLibraryTarget>("ipo");
    ipo += "lib/Transforms/IPO/.*"_rr;
    ipo.Public += BitWriter, IRReader, InstCombine, Instrumentation, Linker,
        Object, ScalarOpts, Vectorize;

    //
    auto &Coroutines = llvm.addTarget<StaticLibraryTarget>("Coroutines");
    Coroutines += "lib/Transforms/Coroutines/.*"_rr;
    Coroutines.Public += ipo;

    //
    auto &Coverage = llvm.addTarget<StaticLibraryTarget>("Coverage");
    Coverage += "lib/ProfileData/Coverage/.*"_rr;
    Coverage.Public += Object, ProfileData;

    //
    auto &Symbolize = llvm.addTarget<StaticLibraryTarget>("Symbolize");
    Symbolize += "lib/DebugInfo/Symbolize/.*"_rr;
    Symbolize.Public += Object, DebugInfoDWARF, DebugInfoPDB;

    //
    auto &RuntimeDyld = llvm.addTarget<StaticLibraryTarget>("RuntimeDyld");
    RuntimeDyld += "lib/ExecutionEngine/RuntimeDyld/.*"_rr;
    RuntimeDyld.Public += Object;

    //
    auto &Target = llvm.addTarget<StaticLibraryTarget>("Target");
    Target += "lib/Target/[^/]*"_rr;
    Target.Public += Analysis;

    //
    auto &CodeGen = llvm.addTarget<StaticLibraryTarget>("CodeGen");
    CodeGen += "lib/CodeGen/[^/]*"_rr;
    CodeGen.Public += BitReader, BitWriter, ScalarOpts, Target;

    //
    auto &AsmPrinter = llvm.addTarget<StaticLibraryTarget>("AsmPrinter");
    AsmPrinter += "lib/CodeGen/AsmPrinter/[^/]*"_rr;
    AsmPrinter -= "lib/CodeGen/AsmPrinter/[^/]*\\.def"_rr;
    AsmPrinter.Public += Analysis, CodeGen, DebugInfoCodeView, DebugInfoMSF,
        MCParser;

    //
    auto &GlobalISel = llvm.addTarget<StaticLibraryTarget>("GlobalISel");
    GlobalISel += "lib/CodeGen/GlobalISel/.*"_rr;
    GlobalISel.Public += TransformUtils, CodeGen;

    //
    auto &MIRParser = llvm.addTarget<StaticLibraryTarget>("MIRParser");
    MIRParser += "lib/CodeGen/MIRParser/.*"_rr;
    MIRParser.Public += AsmParser, CodeGen, Core, MC;

    //
    auto &SelectionDAG = llvm.addTarget<StaticLibraryTarget>("SelectionDAG");
    SelectionDAG += "lib/CodeGen/SelectionDAG/.*"_rr;
    SelectionDAG.Public += TransformUtils, CodeGen;

    //
    auto &ExecutionEngine = llvm.addTarget<StaticLibraryTarget>("ExecutionEngine");
    ExecutionEngine += "lib/ExecutionEngine/[^/]*"_rr;
    ExecutionEngine.Public += Object, RuntimeDyld, Target, Core;

    //
    auto &Interpreter = llvm.addTarget<StaticLibraryTarget>("Interpreter");
    Interpreter += "lib/ExecutionEngine/Interpreter/.*"_rr;
    Interpreter.Public += CodeGen, ExecutionEngine;

    //
    auto &MCJIT = llvm.addTarget<StaticLibraryTarget>("MCJIT");
    MCJIT += "lib/ExecutionEngine/MCJIT/.*"_rr;
    MCJIT.Public += Object, ExecutionEngine, RuntimeDyld, Target;

    //
    auto &OrcJIT = llvm.addTarget<StaticLibraryTarget>("OrcJIT");
    OrcJIT += "lib/ExecutionEngine/Orc/.*"_rr;
    OrcJIT.Public += Object, ExecutionEngine, RuntimeDyld, TransformUtils;

    //
    auto &Passes = llvm.addTarget<StaticLibraryTarget>("Passes");
    Passes += "lib/Passes/.*"_rr;
    Passes -= "lib/Passes/.*\\.def"_rr;
    Passes.Public += Analysis, CodeGen, InstCombine, Instrumentation, Vectorize, ipo;

    //
    auto &LTO = llvm.addTarget<StaticLibraryTarget>("LTO");
    LTO += "lib/LTO/.*"_rr;
    LTO.Public += Linker, ObjCARCOpts, Object, Passes;

    //
    //
    //
    using tablegen_desc = std::map<std::string, Strings>;

    std::map<std::string, tablegen_desc> target_tables{
        {"X86",
         {
             {"GenAsmMatcher.inc", {"-gen-asm-matcher"}},
             {"GenFastISel.inc", {"-gen-fast-isel"}},
             {"GenAsmWriter1.inc", {"-gen-asm-writer", "-asmwriternum=1"}},
             {"GenEVEX2VEXTables.inc", {"-gen-x86-EVEX2VEX-tables"}},
         }},
        {"AArch64",
         {
             {"GenAsmMatcher.inc", {"-gen-asm-matcher"}},
             {"GenFastISel.inc", {"-gen-fast-isel"}},
             {"GenAsmWriter1.inc", {"-gen-asm-writer", "-asmwriternum=1"}},
             {"GenSystemOperands.inc", {"-gen-searchable-tables"}},
             {"GenMCCodeEmitter.inc", {"-gen-emitter"}},
             {"GenMCPseudoLowering.inc", {"-gen-pseudo-lowering"}},
         }},
        {"AMDGPU",
         {
             {"GenAsmMatcher.inc", {"-gen-asm-matcher"}},
             {"GenFastISel.inc", {"-gen-fast-isel"}},
             {"GenMCCodeEmitter.inc", {"-gen-emitter"}},
             {"GenMCPseudoLowering.inc", {"-gen-pseudo-lowering"}},
             {"GenIntrinsics.inc", {"-gen-tgt-intrinsic"}},
             {"GenDFAPacketizer.inc", {"-gen-dfa-packetizer"}},
         }},
        {"ARM",
         {
             {"GenAsmMatcher.inc", {"-gen-asm-matcher"}},
             {"GenFastISel.inc", {"-gen-fast-isel"}},
             {"GenMCCodeEmitter.inc", {"-gen-emitter"}},
             {"GenMCPseudoLowering.inc", {"-gen-pseudo-lowering"}},
             {"GenSystemRegister.inc", {"-gen-searchable-tables"}},
         }},
        {"AVR",
         {
             {"GenAsmMatcher.inc", {"-gen-asm-matcher"}},
             {"GenMCCodeEmitter.inc", {"-gen-emitter"}},
         }},
        {"Mips",
         {
             {"GenAsmMatcher.inc", {"-gen-asm-matcher"}},
             {"GenFastISel.inc", {"-gen-fast-isel"}},
             {"GenMCCodeEmitter.inc", {"-gen-emitter"}},
             {"GenMCPseudoLowering.inc", {"-gen-pseudo-lowering"}},
         }},
        {"PowerPC",
         {
             {"GenAsmMatcher.inc", {"-gen-asm-matcher"}},
             {"GenFastISel.inc", {"-gen-fast-isel"}},
             {"GenMCCodeEmitter.inc", {"-gen-emitter"}},
             {"GenMCPseudoLowering.inc", {"-gen-pseudo-lowering"}},
         }},
        {"Lanai",
         {
             {"GenAsmMatcher.inc", {"-gen-asm-matcher"}},
             {"GenMCCodeEmitter.inc", {"-gen-emitter"}},
             {"GenMCPseudoLowering.inc", {"-gen-pseudo-lowering"}},
         }},
        {"Sparc",
         {
             {"GenAsmMatcher.inc", {"-gen-asm-matcher"}},
             {"GenMCCodeEmitter.inc", {"-gen-emitter"}},
             {"GenMCPseudoLowering.inc", {"-gen-pseudo-lowering"}},
         }},
        {"SystemZ",
         {
             {"GenAsmMatcher.inc", {"-gen-asm-matcher"}},
             {"GenMCCodeEmitter.inc", {"-gen-emitter"}},
             {"GenMCPseudoLowering.inc", {"-gen-pseudo-lowering"}},
         }},
        {"BPF",
         {
             {"GenAsmMatcher.inc", {"-gen-asm-matcher"}},
             {"GenMCCodeEmitter.inc", {"-gen-emitter"}},
             {"GenMCPseudoLowering.inc", {"-gen-pseudo-lowering"}},
         }},
        {"WebAssembly",
         {
             {"GenFastISel.inc", {"-gen-fast-isel"}},
             {"GenAsmMatcher.inc", {"-gen-asm-matcher"}},
             {"GenMCCodeEmitter.inc", {"-gen-emitter"}},
             {"GenMCPseudoLowering.inc", {"-gen-pseudo-lowering"}},
         }},
        {"XCore",
         {
             //{ "GenAsmMatcher.inc",{ "-gen-asm-matcher" } },
         }},
        {"Hexagon",
         {
             {"GenAsmMatcher.inc", {"-gen-asm-matcher"}},
             {"GenMCCodeEmitter.inc", {"-gen-emitter"}},
             {"GenMCPseudoLowering.inc", {"-gen-pseudo-lowering"}},
             {"GenDFAPacketizer.inc", {"-gen-dfa-packetizer"}},
         }},
        {"RISCV",
         {
             {"GenAsmMatcher.inc", {"-gen-asm-matcher"}},
             {"GenMCCodeEmitter.inc", {"-gen-emitter"}},
             {"GenMCPseudoLowering.inc", {"-gen-pseudo-lowering"}},
         }},
    };

    std::map<std::string, std::vector<StaticLibraryTarget*>> targets;

    auto add_arch = [&](std::string a)
    {
        const auto dir = a;
        auto &d = llvm.addDirectory(dir);

        // after dir
        a = a == "PowerPC" ? "PPC" : a;

        auto &ATablegen = d.addTarget<StaticLibraryTarget>("Tablegen");
        ATablegen += "include/.*\\.td"_rr;
        ATablegen += FileRegex("lib/Target/" + dir, std::regex(".*\\.td"), true);

        std::map<std::string, Strings> opts{
            { a + "GenRegisterInfo.inc", {"-gen-register-info"}},
            { a + "GenDisassemblerTables.inc", {"-gen-disassembler"}},
            { a + "GenInstrInfo.inc", {"-gen-instr-info"}},
            { a + "GenAsmWriter.inc", {"-gen-asm-writer"}},
            { a + "GenDAGISel.inc", {"-gen-dag-isel"}},
            { a + "GenCallingConv.inc", {"-gen-callingconv"}},
            { a + "GenSubtargetInfo.inc", {"-gen-subtarget"}},
            { a + "GenRegisterBank.inc", {"-gen-register-bank"}},
            { a + "GenGlobalISel.inc", {"-gen-global-isel"}},
        };
        for (auto &[out, opt] : target_tables[dir])
            opts[a + out] = opt;
        for (auto &[out, opt] : opts)
        {
            tablegen(ATablegen, "lib/Target/" + dir + "/" + a + ".td", out, opt, true);
        }

        //
        auto &AInfo = d.addTarget<StaticLibraryTarget>("Info");
        AInfo += FileRegex("lib/Target/" + dir + "/TargetInfo", std::regex(".*"), true);
        AInfo += FileRegex("lib/Target/" + dir, std::regex(".*\\.h"), true);
        AInfo -= FileRegex("lib/Target/" + dir, std::regex(".*\\.def"), true);
        AInfo.Public += IncludeDirectory{ "lib/Target/" + dir };
        AInfo.Public += Support, ATablegen, Core;
        targets[dir].push_back(&AInfo);

        //
        StaticLibraryTarget *AUtils = nullptr;
        if (fs::exists(AInfo.SourceDir / ("lib/Target/" + dir + "/Utils")))
        {
            auto &Utils = d.addTarget<StaticLibraryTarget>("Utils");
            Utils += FileRegex("lib/Target/" + dir + "/Utils", std::regex(".*"), true);
            Utils.Public += AInfo;
            AUtils = &Utils;
            targets[dir].push_back(&Utils);
        }

        //
        StaticLibraryTarget *AAsmPrinter = nullptr;
        if (fs::exists(AInfo.SourceDir / ("lib/Target/" + dir + "/InstPrinter")))
        {
            auto &AsmPrinter = d.addTarget<StaticLibraryTarget>("AsmPrinter");
            AsmPrinter += FileRegex("lib/Target/" + dir + "/InstPrinter", std::regex(".*"), true);
            AsmPrinter.Public += AInfo, MC;
            if (AUtils)
                AsmPrinter.Public += *AUtils;
            AAsmPrinter = &AsmPrinter;
            targets[dir].push_back(&AsmPrinter);
        }

        //
        StaticLibraryTarget *ADisassembler = nullptr;
        if (fs::exists(AInfo.SourceDir / ("lib/Target/" + dir + "/Disassembler")))
        {
            auto &Disassembler = d.addTarget<StaticLibraryTarget>("Disassembler");
            Disassembler += FileRegex("lib/Target/" + dir + "/Disassembler", std::regex(".*"), true);
            Disassembler.Public += AInfo, MCDisassembler;
            ADisassembler = &Disassembler;
            targets[dir].push_back(&Disassembler);
        }

        //
        if (fs::exists(AInfo.SourceDir / ("lib/Target/" + dir + "/AsmParser")))
        {
            auto &AAsmParser = d.addTarget<StaticLibraryTarget>("AsmParser");
            AAsmParser += FileRegex("lib/Target/" + dir + "/AsmParser", std::regex(".*"), true);
            AAsmParser.Public += AInfo, MCParser;
            targets[dir].push_back(&AAsmParser);
        }

        //
        auto &ADesc = d.addTarget<StaticLibraryTarget>("Desc");
        ADesc += FileRegex("lib/Target/" + dir + "/MCTargetDesc", std::regex(".*"), true);
        ADesc.Public += MCDisassembler, Object, AInfo;
        if (AAsmPrinter)
            ADesc.Public += *AAsmPrinter;
        targets[dir].push_back(&ADesc);

        //
        auto &ACodeGen = d.addTarget<StaticLibraryTarget>("CodeGen");
        ACodeGen += FileRegex("lib/Target/" + dir, std::regex("[^/]*"), true);
        ACodeGen -= FileRegex("lib/Target/" + dir, std::regex("[^/]*\\.def"), true);
        ACodeGen.Public += Analysis, AsmPrinter, CodeGen, Core, GlobalISel,
            SelectionDAG, Target, ADesc;
        if (AAsmPrinter)
            ACodeGen.Public += *AAsmPrinter;
        targets[dir].push_back(&ACodeGen);
    };

    for (auto &t : selected_targets)
        add_arch(t);

    auto &clang = s.addProject("clang", "master");
    clang.Source = Git("https://github.com/llvm-mirror/clang", "", "{v}");
    //clang.setRootDirectory("tools/clang");

    auto &CUTablegen = clang.addTarget<ExecutableTarget>("TableGen");
    CUTablegen += "utils/TableGen/.*"_rr;
    CUTablegen.Public += Tablegen;

    auto clang_tablegen = [&CUTablegen](auto &t, const std::string &in,
        const std::string &out, const Strings &args,
        bool add_target_idir = false)
    {
        (t + CUTablegen)->Dummy = true;

        auto c = std::make_shared<Command>();
        c->setProgram(CUTablegen);
        for (auto &a : args)
            c->args.push_back(a);
        c->args.push_back("-I");
        c->args.push_back(t.SourceDir.string());
        c->args.push_back("-I");
        c->args.push_back((t.SourceDir / "include").string());
        if (add_target_idir)
        {
            c->args.push_back("-I");
            c->args.push_back((t.SourceDir / in).parent_path().string());
        }
        c->args.push_back((t.SourceDir / in).string());
        c->args.push_back("-o");
        c->args.push_back((t.BinaryDir / out).string());

        c->addInput(t.SourceDir / in);
        c->addOutput(t.BinaryDir / out);

        t.Public += t.BinaryDir / out;

        fs::create_directories((t.BinaryDir / out).parent_path());
    };

    auto clang_diag_gen = [&clang_tablegen](auto &t, const std::string &component)
    {
        clang_tablegen(t, "include/clang/Basic/Diagnostic.td",
            "clang/Basic/Diagnostic" + component + "Kinds.inc", {"-gen-clang-diags-defs","-clang-component="+component}, true);
    };

    auto &Basic = clang.addTarget<StaticLibraryTarget>("Basic");
    {
        Basic += "include/.*"_rr;
        Basic += "lib/Basic/.*"_rr;
        Basic += "lib/Basic"_idir;
        Basic -= "include/.*\\.def"_rr;
        Basic -= "lib/Basic/.*\\.def"_rr;
        Basic.Public += Core, MC;
        Basic.Protected += "_CINDEX_LIB_"_def;

        Basic.Variables["BACKEND_PACKAGE_STRING"] = Basic.getPackage().version.toString();
        Basic.Variables["CLANG_VERSION"] = Basic.getPackage().version.toString();
        Basic.Variables["CLANG_VERSION_MAJOR"] = Basic.Variables["PACKAGE_VERSION_MAJOR"];
        Basic.Variables["CLANG_VERSION_MINOR"] = Basic.Variables["PACKAGE_VERSION_MINOR"];
        Basic.Variables["CLANG_VERSION_PATCHLEVEL"] = Basic.Variables["PACKAGE_VERSION_PATCH"];

        Basic.configureFile("include/clang/Basic/Version.inc.in", "clang/Basic/Version.inc");
        Basic.writeFileOnce("SVNVersion.h", true);

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

        for (auto &c : { "Analysis", "AST", "Comment", "Common",
                        "CrossTU",
                        "Driver",
                        "Frontend",
                        "Lex",
                        "Parse",
                        "Refactoring",
                        "Sema",
                        "Serialization" })
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

        clang_tablegen(Basic, "include/clang/Basic/arm_neon.td",
            "clang/Basic/arm_neon.inc", { "-gen-arm-neon-sema" }, true);

        // AST

        clang_tablegen(Basic, "include/clang/Basic/Attr.td",
            "clang/AST/Attrs.inc", { "-gen-clang-attr-classes" }, true);
        clang_tablegen(Basic, "include/clang/Basic/Attr.td",
            "clang/AST/AttrImpl.inc", { "-gen-clang-attr-impl" }, true);
        clang_tablegen(Basic, "include/clang/Basic/Attr.td",
            "clang/AST/AttrDump.inc", { "-gen-clang-attr-dump" }, true);
        clang_tablegen(Basic, "include/clang/Basic/Attr.td",
            "clang/AST/AttrVisitor.inc", { "-gen-clang-attr-ast-visitor" }, true);

        clang_tablegen(Basic, "include/clang/Basic/StmtNodes.td",
            "clang/AST/StmtNodes.inc", { "-gen-clang-stmt-nodes" }, true);
        clang_tablegen(Basic, "include/clang/Basic/DeclNodes.td",
            "clang/AST/DeclNodes.inc", { "-gen-clang-decl-nodes" }, true);
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

        Files idirs;
        for (auto &i : Support.Public.IncludeDirectories)
            idirs.insert(i);
        tablegen(Basic, "include/clang/Driver/Options.td",
            "clang/Driver/Options.inc", { "-gen-opt-parser-defs" }, true, idirs);

        // Serialization

        clang_tablegen(Basic, "include/clang/Basic/Attr.td",
            "clang/Serialization/AttrPCHRead.inc", { "-gen-clang-attr-pch-read" }, true);
        clang_tablegen(Basic, "include/clang/Basic/Attr.td",
            "clang/Serialization/AttrPCHWrite.inc", { "-gen-clang-attr-pch-write" }, true);

        // StaticAnalyzer.Checkers

        clang_tablegen(Basic, "include/clang/StaticAnalyzer/Checkers/Checkers.td",
            "clang/StaticAnalyzer/Checkers/Checkers.inc", { "-gen-clang-sa-checkers" }, true);
    }

    auto &Driver = clang.addTarget<StaticLibraryTarget>("Driver");
    Driver += "lib/Driver/.*"_rr;
    Driver += "lib/Driver"_idir;
    Driver.Public += Basic;
    if (s.Settings.TargetOS.Type == OSType::Windows)
        Driver.Public += "version.lib"_lib;

    auto &Lex = clang.addTarget<StaticLibraryTarget>("Lex");
    Lex += "lib/Lex/.*"_rr;
    Lex.Public += Basic;

    auto &Rewrite = clang.addTarget<StaticLibraryTarget>("Rewrite");
    Rewrite += "lib/Rewrite/.*"_rr;
    Rewrite.Public += Lex;

    auto &AST = clang.addTarget<StaticLibraryTarget>("AST");
    AST += "lib/AST/.*"_rr;
    AST.Public += Lex, BinaryFormat;

    auto &ASTMatchers = clang.addTarget<StaticLibraryTarget>("AST.Matchers");
    ASTMatchers += "lib/ASTMatchers/[^/]*"_rr;
    ASTMatchers.Public += AST;

    auto &Edit = clang.addTarget<StaticLibraryTarget>("Edit");
    Edit += "lib/Edit/.*"_rr;
    Edit.Public += AST;

    auto &Clang_Analysis = clang.addTarget<StaticLibraryTarget>("Analysis");
    Clang_Analysis += "lib/Analysis/.*"_rr;
    Clang_Analysis.Public += AST;

    auto &Sema = clang.addTarget<StaticLibraryTarget>("Sema");
    Sema += "lib/Sema/.*"_rr;
    Sema.Public += Clang_Analysis, Edit;

    auto &Parse = clang.addTarget<StaticLibraryTarget>("Parse");
    Parse += "lib/Parse/.*"_rr;
    Parse.Public += Sema;

    auto &Serialization = clang.addTarget<StaticLibraryTarget>("Serialization");
    Serialization += "lib/Serialization/.*"_rr;
    Serialization.Public += Sema;

    auto &Frontend = clang.addTarget<StaticLibraryTarget>("Frontend");
    Frontend += "lib/Frontend/[^/]*"_rr;
    Frontend.Public += Serialization, Parse, Driver, BitReader, Option, ProfileData;

    auto &RewriteFrontend = clang.addTarget<StaticLibraryTarget>("RewriteFrontend");
    RewriteFrontend += "lib/Frontend/Rewrite/.*"_rr;
    RewriteFrontend.Public += Frontend, Rewrite;

    auto &FrontendTool = clang.addTarget<StaticLibraryTarget>("FrontendTool");
    FrontendTool += "lib/FrontendTool/[^/]*"_rr;
    FrontendTool.Public += Basic;

    auto &ToolingCore = clang.addTarget<StaticLibraryTarget>("Tooling.Core");
    ToolingCore += "lib/Tooling/Core/.*"_rr;
    ToolingCore.Public += Rewrite, AST;

    auto &Format = clang.addTarget<StaticLibraryTarget>("Format");
    Format += "lib/Format/.*"_rr;
    Format.Public += ToolingCore;

    auto &Tooling = clang.addTarget<StaticLibraryTarget>("Tooling");
    Tooling += "lib/Tooling/[^/]*"_rr;
    Tooling.Public += ToolingCore, Frontend, Format, ASTMatchers;

    auto &Index = clang.addTarget<StaticLibraryTarget>("Index");
    Index += "lib/Index/.*"_rr;
    Index.Public += ToolingCore, Format, Frontend, Rewrite;

    auto &Clang_CodeGen = clang.addTarget<StaticLibraryTarget>("CodeGen");
    Clang_CodeGen += "lib/CodeGen/.*"_rr;
    Clang_CodeGen.Public += Frontend, Analysis, BitReader, BitWriter, Core,
        Coroutines, Coverage, ipo, IRReader, InstCombine, Instrumentation, LTO,
        Linker, MC, ObjCARCOpts, Object, Passes, ProfileData, ScalarOpts,
        Support, Target, TransformUtils;

    auto &libclang = clang.addTarget<LibraryTarget>("tools.libclang");
    libclang += "tools/libclang/.*"_rr;
    libclang.Public += Clang_CodeGen, FrontendTool, Index, Tooling;

    for (auto &[n,v] : targets)
    {
        for (auto &t : v)
            libclang.Public += *t;
    }

    auto &clang_clang = clang.addTarget<ExecutableTarget>("tools.clang");
    clang_clang += "tools/driver/.*"_rr;
    clang_clang.Public += Clang_CodeGen, FrontendTool, Index, Tooling, RewriteFrontend;

    for (auto &[n, v] : targets)
    {
        for (auto &t : v)
            clang_clang.Public += *t;
    }
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
