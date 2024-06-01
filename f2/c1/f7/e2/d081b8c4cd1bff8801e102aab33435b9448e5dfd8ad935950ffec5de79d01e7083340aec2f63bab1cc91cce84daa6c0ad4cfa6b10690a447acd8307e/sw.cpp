void build(Solution &s)
{
    auto &luajit = s.addLibrary("luajit", "2.1.0");
    // mirror! non official
    luajit += Git("https://github.com/LuaJIT/LuaJIT", "", "v{M}.{m}");

    auto &minilua = luajit.addExecutable("minilua");
    {
        minilua += "src/host/minilua.c";
    }

    auto luajit_h = [&](auto &t) {
        t.writeFileOnce("luajit_relver.txt", "0");
        t.addCommand(SW_VISIBLE_BUILTIN_FUNCTION(copy_file))
            << cmd::in("src/luajit_rolling.h")
            << cmd::out("luajit_rolling.h")
            ;
        t.addCommand()
            << cmd::prog(minilua)
            << cmd::wdir(t.BinaryDir)
            << cmd::in("src/host/genversion.lua")
            << cmd::end()
            << cmd::out("luajit.h")
            ;
    };

    auto &buildvm = luajit.addExecutable("buildvm");
    {
        buildvm += "src/host/buildvm.*"_rr;
        buildvm += "src"_idir;
        auto c = buildvm.addCommand()
            << cmd::prog(minilua)
            << cmd::in("dynasm/dynasm.lua")
            << "-LN"
            ;
        if (buildvm.getBuildSettings().TargetOS.Type == OSType::Windows)
        c
            << "-D"
            << "WIN"
            ;
        c
            << "-D"
            << "JIT"
            << "-D"
            << "FFI"
            << "-D"
            << "ENDIAN_LE"
            << "-D"
            << "FPU"
            << "-D"
            << "P64"
            //
            << "-o"
            << cmd::out("buildvm_arch.h")
            << cmd::in("src/vm_x64.dasc")
            ;
        luajit_h(buildvm);
    }

    // luajit
    {
        luajit -= "src/.*"_rr;
        luajit += "src/ljamalg.c";
        luajit_h(luajit);

        //luajit += "LUA_LIB"_def;
        if (luajit.getBuildSettings().TargetOS.Type == OSType::Windows)
            luajit.Public += sw::Shared, "LUA_BUILD_AS_DLL"_def;

        if (luajit.getBuildSettings().TargetOS.Type != OSType::Windows) {
            //if (luajit.getBuildSettings().TargetOS.isApple()) {
                //luajit += "LUAJIT_UNWIND_INTERNAL"_def;
            //} else {
                luajit += "LUAJIT_UNWIND_EXTERNAL"_def;
            //}
        }

        auto bvm = [&](auto &&what) {
            auto c = luajit.addCommand();
            c
                << cmd::prog(buildvm)
                << "-m"
                << what
                << "-o"
                << cmd::out("lj_"s + what + ".h")
                ;
            return c;
        };
        auto bvm2 = [&](auto c) {
            c
                << cmd::in("src/lib_base.c", cmd::Skip)
                << cmd::in("src/lib_math.c", cmd::Skip)
                << cmd::in("src/lib_bit.c", cmd::Skip)
                << cmd::in("src/lib_string.c", cmd::Skip)
                << cmd::in("src/lib_table.c", cmd::Skip)
                << cmd::in("src/lib_io.c", cmd::Skip)
                << cmd::in("src/lib_os.c", cmd::Skip)
                << cmd::in("src/lib_package.c", cmd::Skip)
                << cmd::in("src/lib_debug.c", cmd::Skip)
                << cmd::in("src/lib_jit.c", cmd::Skip)
                << cmd::in("src/lib_ffi.c", cmd::Skip)
                << cmd::in("src/lib_buffer.c", cmd::Skip)
                ;
            return c;
        };
        bvm2(bvm("bcdef"));
        bvm2(bvm("ffdef"));
        bvm2(bvm("libdef"));
        bvm2(bvm("recdef"));
        bvm("folddef") << cmd::in("src/lj_opt_fold.c", cmd::Skip);

        if (luajit.getBuildSettings().TargetOS.Type == OSType::Windows) {
            luajit.addCommand()
                << cmd::prog(buildvm)
                << "-m"
                << "peobj"
                << "-o"
                << cmd::out("lj_vm.obj")
                ;
        } else {
            luajit.addCommand()
                << cmd::prog(buildvm)
                << "-m"
                << (luajit.getBuildSettings().TargetOS.isApple() ? "machasm" : "elfasm")
                << "-o"
                << cmd::out("lj_vm.S")
                ;
        }
/*
buildvm -m vmdef -o jit\vmdef.lua %ALL_LIB%
@if errorlevel 1 goto :BAD*/

    }
}
