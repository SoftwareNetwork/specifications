void build(Solution &s)
{
    auto &opus = s.addTarget<LibraryTarget>("xiph.opus", "1.4.0");
    opus += Git("https://github.com/xiph/opus", "v{M}.{m}{po}");

    opus.ApiName = "OPUS_EXPORT";

    opus +=
        "celt/.*"_rr,
        "include/.*"_rr,
        "silk/.*"_rr,
        "src/.*"_rr;

    opus -=
        "src/opus_compare.c",
        ".*demo.*"_rr,
        ".*train.*"_rr,
        "celt/arm/.*"_rr,
        "celt/dump_modes/.*"_rr,
        "celt/mips/.*"_rr,
        "celt/tests/.*"_rr,
        "silk/arm/.*"_rr,
        "silk/fixed/arm/.*"_rr,
        "silk/fixed/mips/.*"_rr,
        "silk/mips/.*"_rr,
        "silk/tests/.*"_rr;

    opus.Public +=
        "silk/float"_id,
        "silk"_id,
        "celt"_id,
        "."_id,
        "include"_id;

    opus.Private += "OPUS_BUILD"_d;
    opus.Private += "USE_ALLOCA"_d;

    opus.replaceInFileOnce("silk/sort.c", "#ifdef FIXED_POINT", "#if 1");
    opus.replaceInFileOnce("silk/x86/x86_silk_map.c", "#if defined(FIXED_POINT)", "#if 1");

    if (opus.getBuildSettings().TargetOS.Arch == ArchType::aarch64)
    {
        opus -= ".*sse.*"_rr;
        /*opus +=
            "celt/arm/.*"_rr,
            "silk/arm/.*"_rr,
            "silk/fixed/arm/.*"_rr
            ;*/
        opus += "OPUS_ARM_ASM"_def;
        opus += "OPUS_ARM_MAY_HAVE_EDSP"_def;
        opus += "OPUS_ARM_MAY_HAVE_NEON"_def;
        opus += "OPUS_ARM_MAY_HAVE_MEDIA"_def;
        opus += "OPUS_ARM_MAY_HAVE_NEON_INTR"_def;
    }
    else
    {
        opus.Private += "OPUS_HAVE_RTCD"_d;
        opus.Private += "OPUS_X86_MAY_HAVE_SSE"_d;
        opus.Private += "OPUS_X86_MAY_HAVE_SSE2"_d;
        opus.Private += "OPUS_X86_MAY_HAVE_SSE4_1"_d;
    }
    if (opus.getBuildSettings().TargetOS.Type == OSType::Linux)
    {
        opus.CompileOptions.push_back("-msse4");
    }
    if (opus.getBuildSettings().TargetOS.Type == OSType::Windows &&
        (opus.getCompilerType() == CompilerType::Clang || opus.getCompilerType() == CompilerType::ClangCl
        ))
    {
        opus.CompileOptions.push_back("-mavx2");
    }

    if (!opus.DryRun)
    {
        auto dummy = opus.BinaryDir / "private/sw_copy_headers.txt";
        if (!fs::exists(dummy))
        {
            auto dir2 = opus.BinaryDir / "opus";
            fs::create_directories(dir2);
            for (auto &f : fs::recursive_directory_iterator(opus.SourceDir / "include"))
                fs::copy_file(f, dir2 / f.path().filename(), fs::copy_options::overwrite_existing);
            write_file(dummy, "");
        }
    }
}
