#pragma sw require header org.sw.demo.lexxmark.winflexbison.bison

// https://www.mesa3d.org/sourcetree.html
void build(Solution &s)
{
   auto &p = s.addProject("freedesktop.mesa", "20.0.5");
   p += Git("https://gitlab.freedesktop.org/mesa/mesa", "mesa-{v}");

   auto &includes = p.addLibrary("includes");
   {
      auto &t = includes;
      t += "include/.*"_rr;
      t.Public += "include"_idir;
      t.Public += "src/gallium/auxiliary"_idir;
      t.Public += "src/gallium/include"_idir;
      t.Public += "src/mapi"_idir;
      t.Public += "src/mesa"_idir;
      t.Public += "src"_idir;

      // also needed on clang/clang-cl
      //if (t.getBuildSettings().TargetOS.is(OSType::Windows))
      if (t.getCompilerType() == CompilerType::MSVC)
      {
         t.Public += "_USE_MATH_DEFINES"_def;
      }
   }

   auto &compiler = p.addStaticLibrary("compiler");
   {
      auto &t = compiler;
      t += "src/compiler/.*"_r;
      t.Public += includes;

      {
         auto c = t.addCommand();
         c << cmd::prog("org.sw.demo.python.exe"_dep)
            << cmd::dep("org.sw.demo.python.pypi.mako"_dep)
            << cmd::in("src/compiler/glsl/ir_expression_operation.py")
            << "enum"
            << cmd::std_out("ir_expression_operation.h")
            ;
      }
   }

   auto &nir = compiler.addStaticLibrary("nir");
   {
      auto &t = nir;
      t += "src/compiler/nir/.*"_r;
      t.Public += compiler;

      t.Public += "src/compiler/nir"_idir;
      t.Public += "src/compiler"_idir;

      {
         auto c = t.addCommand();
         c << cmd::prog("org.sw.demo.python.exe"_dep)
            << cmd::in("src/compiler/nir/nir_builder_opcodes_h.py")
            << cmd::std_out("nir_builder_opcodes.h")
            ;
      }

      {
         auto c = t.addCommand();
         c << cmd::prog("org.sw.demo.python.exe"_dep)
            << cmd::in("src/compiler/nir/nir_opcodes_h.py")
            << cmd::std_out("nir_opcodes.h")
            ;
      }
      {
         auto c = t.addCommand();
         c << cmd::prog("org.sw.demo.python.exe"_dep)
            << cmd::in("src/compiler/nir/nir_opcodes_c.py")
            << cmd::std_out("nir_opcodes.c")
            ;
      }

      {
         auto c = t.addCommand();
         c << cmd::prog("org.sw.demo.python.exe"_dep)
            << cmd::in("src/compiler/nir/nir_intrinsics_h.py")
            << "--outdir"
            << t.BinaryDir
            << cmd::end()
            << cmd::out("nir_intrinsics.h")
            ;
      }
      {
         auto c = t.addCommand();
         c << cmd::prog("org.sw.demo.python.exe"_dep)
            << cmd::in("src/compiler/nir/nir_intrinsics_c.py")
            << "--outdir"
            << t.BinaryDir
            << cmd::end()
            << cmd::out("nir_intrinsics.c")
            ;
      }

      {
         auto c = t.addCommand();
         c << cmd::prog("org.sw.demo.python.exe"_dep)
            << cmd::in("src/compiler/nir/nir_opt_algebraic.py")
            << cmd::std_out("nir_opt_algebraic.c")
            ;
      }

      {
         auto c = t.addCommand();
         c << cmd::prog("org.sw.demo.python.exe"_dep)
            << cmd::in("src/compiler/nir/nir_constant_expressions.py")
            << cmd::std_out("nir_constant_expressions.c")
            ;
      }
   }

   auto &spirv = compiler.addStaticLibrary("spirv");
   {
      auto &t = spirv;
      t += "src/compiler/spirv/.*"_r;
      t -= "src/compiler/spirv/spirv2nir.c";

      t += "src/compiler/spirv"_idir;
      t.Public += nir;

      {
         auto c = t.addCommand();
         c << cmd::prog("org.sw.demo.python.exe"_dep)
            << cmd::in("src/compiler/spirv/vtn_gather_types_c.py")
            << cmd::in("src/compiler/spirv/spirv.core.grammar.json")
            << cmd::out("vtn_gather_types.c")
            ;
      }
      {
         auto c = t.addCommand();
         c << cmd::prog("org.sw.demo.python.exe"_dep)
            << cmd::in("src/compiler/spirv/spirv_info_c.py")
            << cmd::in("src/compiler/spirv/spirv.core.grammar.json")
            << cmd::out("spirv_info.c")
            ;
      }
   }

   auto &glsl_cpp = compiler.addStaticLibrary("glsl.cpp");
   {
      auto &t = glsl_cpp;
      t += "src/compiler/glsl/glcpp/pp.c";
      t += "src/compiler/glsl/glcpp/glcpp.h";

      t += "src/compiler/glsl/glcpp"_idir;
      t += includes;

      FlexBisonData d;
      d.out = "glcpp-parse.c";
      d.outh = "glcpp-parse.h";
      d.args.push_back("-p");
      d.args.push_back("glcpp_parser_");
      gen_bison("org.sw.demo.lexxmark.winflexbison"_dep, t, "src/compiler/glsl/glcpp/glcpp-parse.y", d);
      gen_flex("org.sw.demo.lexxmark.winflexbison"_dep, t, "src/compiler/glsl/glcpp/glcpp-lex.l");
   }

   auto &glsl = compiler.addStaticLibrary("glsl");
   {
      auto &t = glsl;
      t += "src/compiler/glsl/.*"_r;
      t -= "src/compiler/glsl/main.cpp";
      t -= "src/compiler/glsl/.*standalone.*"_r;
      t -= "src/compiler/glsl/.*test.*"_r;

      t.Public += "src/compiler/glsl"_idir;

      t.Public += nir;
      t += glsl_cpp;

      FlexBisonData d;
      d.out = "glsl_parser.cpp";
      d.outh = "glsl_parser.h";
      d.args.push_back("-p");
      d.args.push_back("_mesa_glsl_");
      gen_bison("org.sw.demo.lexxmark.winflexbison"_dep, t, "src/compiler/glsl/glsl_parser.yy", d);
      gen_flex("org.sw.demo.lexxmark.winflexbison"_dep, t, "src/compiler/glsl/glsl_lexer.ll", "glsl_lexer.cpp", "glsl_lexer.h");

      {
         auto c = t.addCommand();
         c << cmd::prog("org.sw.demo.python.exe"_dep)
            << cmd::in("src/compiler/glsl/ir_expression_operation.py")
            << "constant"
            << cmd::std_out("ir_expression_operation_constant.h")
            ;
      }
      {
         auto c = t.addCommand();
         c << cmd::prog("org.sw.demo.python.exe"_dep)
            << cmd::in("src/compiler/glsl/ir_expression_operation.py")
            << "strings"
            << cmd::std_out("ir_expression_operation_strings.h")
            ;
      }
      {
         auto c = t.addCommand();
         c << cmd::prog("org.sw.demo.python.exe"_dep)
            << cmd::in("src/compiler/glsl/xxd.py")
            << cmd::in("src/compiler/glsl/float64.glsl")
            << cmd::out("float64_glsl.h")
            << "-n" << "float64_source"
            ;
      }
   }

   auto &glapi_gen = p.addStaticLibrary("gen");
   {
      auto &t = glapi_gen;
      t -= "src/mapi/glapi/gen/.*"_rr;

      //t.Public += includes;

      {
         auto c = t.addCommand();
         c << cmd::prog("org.sw.demo.python.exe"_dep)
            << cmd::in("src/mapi/glapi/gen/gl_table.py")
            << "-f"
            << cmd::in("src/mapi/glapi/gen/gl_and_es_API.xml")
            << cmd::std_out("glapitable.h")
            ;
      }
      auto add_dispatch = [&t](const auto &out)
      {
         auto c = t.addCommand();
         c << cmd::prog("org.sw.demo.python.exe"_dep)
            << cmd::in("src/mapi/glapi/gen/gl_table.py")
            << "-m" << "remap_table"
            << "-f"
            << cmd::in("src/mapi/glapi/gen/gl_and_es_API.xml")
            << cmd::std_out(out)
            ;
      };
      add_dispatch("dispatch.h");
      add_dispatch("main/dispatch.h");
      {
         auto c = t.addCommand();
         c << cmd::prog("org.sw.demo.python.exe"_dep)
            << cmd::in("src/mapi/glapi/gen/gl_apitemp.py")
            << "-f"
            << cmd::in("src/mapi/glapi/gen/gl_and_es_API.xml")
            << cmd::std_out("glapitemp.h")
            ;
      }
      {
         auto c = t.addCommand();
         c << cmd::prog("org.sw.demo.python.exe"_dep)
            << cmd::in("src/mapi/glapi/gen/gl_procs.py")
            << "-c"
            << "-f"
            << cmd::in("src/mapi/glapi/gen/gl_and_es_API.xml")
            << cmd::std_out("glprocs.h")
            ;
      }
      {
         auto c = t.addCommand();
         c << cmd::prog("org.sw.demo.python.exe"_dep)
            << cmd::in("src/mapi/glapi/gen/gl_marshal_h.py")
            << "-f"
            << cmd::in("src/mapi/glapi/gen/gl_and_es_API.xml")
            << cmd::std_out("marshal_generated.h")
            ;
      }
      {
         auto c = t.addCommand();
         c << cmd::prog("org.sw.demo.python.exe"_dep)
            << cmd::in("src/mapi/glapi/gen/remap_helper.py")
            << "-f"
            << cmd::in("src/mapi/glapi/gen/gl_and_es_API.xml")
            << cmd::std_out("main/remap_helper.h")
            ;
      }

      {
         auto c = t.addCommand();
         c << cmd::prog("org.sw.demo.python.exe"_dep)
            << cmd::in("src/mapi/glapi/gen/gl_x86-64_asm.py")
            << "-f"
            << cmd::in("src/mapi/glapi/gen/gl_and_es_API.xml")
            << cmd::std_out("glapi_x86-64.S")
            ;
      }
   }

   auto &util = p.addStaticLibrary("util");
   {
      auto &t = util;

      t += "src/util/.*"_r;
      t -= "src/util/.*_test.*"_r;
      t -= "src/util/xmlconfig.*"_r;

      t.Public += "src"_idir;
      t.Public += "src/util"_idir;

      t.Public += includes;

      {
         auto c = t.addCommand();
         c << cmd::prog("org.sw.demo.python.exe"_dep)
            << cmd::in("src/util/format_srgb.py")
            << cmd::std_out("format_srgb.c")
            ;
      }
   }

   auto &format = util.addStaticLibrary("format");
   {
      auto &t = format;

      t += "src/util/format/.*"_rr;

      t += "src"_idir;
      t += "src/util/format"_idir;

      t += includes;

      {
         auto c = t.addCommand();
         c << cmd::prog("org.sw.demo.python.exe"_dep)
            << cmd::in("src/util/format/u_format_table.py")
            << cmd::in("src/util/format/u_format.csv")
            << cmd::std_out("u_format_table.c")
            ;
      }
   }

   auto &sha1 = util.addStaticLibrary("sha1");
   {
      auto &t = sha1;
      t += "src/util/sha1/.*"_rr;
      t += util;
   }

   // glapi
   auto &glapi = p.addStaticLibrary("glapi");
   {
      auto &t = glapi;

      enum
      {
         MAPI_MODE_BRIDGE,
         MAPI_MODE_UTIL,
         MAPI_MODE_GLAPI,
      };
      auto type = MAPI_MODE_UTIL;
      //if (t.getBuildSettings().Native.LibrariesType == LibraryType::Shared)
         //type = MAPI_MODE_GLAPI;
      //type = MAPI_MODE_BRIDGE;
      String mapi_header_type = "glapi";
      String definition;

      //
      if (type == MAPI_MODE_GLAPI)
      {
         t += "src/mapi/u_.*"_r;
         t += "src/mapi/entry.*"_r;
         t += "src/mapi/stub.*"_r;
         t += "src/mapi/table.*"_r;
         t += "src/mapi/mapi_glapi.c";

         definition = "MAPI_MODE_GLAPI";
         mapi_header_type = "shared-" + mapi_header_type;
      }

      if (type == MAPI_MODE_BRIDGE)
      {
         t += "src/mapi/entry.*"_r;
         //t += "src/gallium/state_trackers/wgl/opengl32.def";

         definition = "MAPI_MODE_BRIDGE";

         if (t.getBuildSettings().TargetOS.is(OSType::Windows))
         {
            t += "_GDI32_"_def;
            t += "BUILD_GL32"_def;
         }
      }

      if (type == MAPI_MODE_UTIL)
      {
         t += "src/mapi/u_.*"_r;
         t += "src/mapi/glapi/glapi.*"_r;

         definition = "MAPI_MODE_UTIL";

         if (t.getBuildSettings().TargetOS.is(OSType::Windows))
         {
            t += "_GDI32_"_def;
            t += "BUILD_GL32"_def;
         }
      }

      if (t.getBuildSettings().TargetOS.is(OSType::Windows))
      {
         t += sw::Shared, "_GLAPI_DLL_EXPORTS"_def;
         t.Public += sw::Static, "_GLAPI_NO_EXPORTS"_def;
      }

      t += Definition(definition);

      //
      t.Public += "src/mapi/glapi"_idir;
      t.Public += "src/mapi"_idir;

      t.Public += util;
      t.Public += glapi_gen;

      {
         auto c = t.addCommand();
         c << cmd::prog("org.sw.demo.python.exe"_dep)
            << cmd::in("src/mapi/glapi/gen/gl_gentable.py")
            << "-f"
            << cmd::in("src/mapi/glapi/gen/gl_and_es_API.xml")
            << cmd::std_out("glapi_gentable.c")
            ;
      }

      {
         auto c = t.addCommand();
         c << cmd::prog("org.sw.demo.python.exe"_dep)
            << cmd::in("src/mapi/glapi/gen/gl_enums.py")
            << "-f"
            << cmd::in("src/mapi/glapi/registry/gl.xml")
            << cmd::std_out("enums.c")
            ;
      }

      {
         auto c = t.addCommand();
         c << cmd::prog("org.sw.demo.python.exe"_dep)
            << cmd::in("src/mapi/mapi_abi.py")
            << "--printer"
            << mapi_header_type
            << cmd::in("src/mapi/glapi/gen/gl_and_es_API.xml")
            << cmd::std_out("glapi_mapi_tmp.h")
            ;
         t += Definition("MAPI_ABI_HEADER=\"" + normalize_path(t.BinaryDir / "glapi_mapi_tmp.h") + "\"");
      }
   }

   auto &common = p.addStaticLibrary("common");
   {
      auto &t = common;

      t.PackageDefinitions = true;

      t += "src/mesa/main/.*"_r;
      t += "src/mesa/math/.*"_r;
      t += "src/mesa/program/.*"_r;
      t += "src/mesa/vbo/.*"_r;
      t += "src/mesa/x86/common_x86.c";

      /*if (t.getBuildSettings().TargetOS.is(OSType::Windows))
      {
         t += "_GDI32_"_def;
         t += "BUILD_GL32"_def;
      }*/

      t.Public += "src/mesa/main"_idir;
      t.Public += "src/mesa"_idir;

      t.Public += includes;
      t.Public += glapi_gen;
      t.Public += glapi; // use defs from it
      t.Public += util;
      t.Public += nir;
      t.Public += sha1;
      t.Public += glsl;
      t.Public += spirv;

      t.writeFileOnce("git_sha1.h", "#define MESA_GIT_SHA1 \" (git-36e0d2f39b)\"");

      {
         auto c = t.addCommand();
         c << cmd::prog("org.sw.demo.python.exe"_dep)
            << cmd::in("src/mesa/main/get_hash_generator.py")
            << "-f"
            << cmd::in("src/mapi/glapi/gen/gl_and_es_API.xml")
            << cmd::std_out("get_hash.h")
            ;
      }

      String files[][2] = {{"format_info.h"s, "format_info.py"s},
         {"format_pack.c"s, "format_pack.py"s},
         {"format_unpack.c"s, "format_unpack.py"s}};
      for (auto &p : files)
      {
         auto c = t.addCommand();
         c << cmd::prog("org.sw.demo.python.exe"_dep)
            << cmd::in("src/mesa/main/"s + p[1])
            << cmd::in("src/mesa/main/formats.csv")
            << cmd::std_out(p[0])
            ;
      }

      {
         auto c = t.addCommand();
         c << cmd::prog("org.sw.demo.python.exe"_dep)
            << cmd::in("src/mesa/main/format_fallback.py")
            << cmd::in("src/mesa/main/formats.csv")
            << cmd::out("format_fallback.c")
            ;
      }

      {
         auto c = t.addCommand();
         c << cmd::prog("org.sw.demo.python.exe"_dep)
            << cmd::in("src/mapi/glapi/gen/gl_genexec.py")
            << "-f"
            << cmd::in("src/mapi/glapi/gen/gl_and_es_API.xml")
            << cmd::std_out("api_exec.c")
            ;
      }

      for (int i = 0; i < 8; i++)
      {
         auto x = std::to_string(i);
         auto c = t.addCommand();
         c << cmd::prog("org.sw.demo.python.exe"_dep)
            << cmd::in("src/mapi/glapi/gen/gl_marshal.py")
            << "-f"
            << cmd::in("src/mapi/glapi/gen/gl_and_es_API.xml")
            << "-i" << x
            << "-n" << "8"
            << cmd::std_out("marshal_generated" + x + ".c")
            ;
      }

      gen_bison("org.sw.demo.lexxmark.winflexbison"_dep, t, "src/mesa/program/program_parse.y", "program_parse.tab.c", "program/program_parse.tab.h");
      gen_flex("org.sw.demo.lexxmark.winflexbison"_dep, t, "src/mesa/program/program_lexer.l");
   }

   // gallium
   auto &gallium = p.addStaticLibrary("gallium");
   {
      auto &t = gallium;
      t.PackageDefinitions = true;
      t += "src/mesa/state_tracker/.*"_r;
      t.Public += includes;
      t.Public += common;
      t.Public += nir;
   }

   // gallium_auxiliary
   auto &gallium_auxiliary = gallium.addStaticLibrary("auxiliary");
   {
      auto &t = gallium_auxiliary;
      t += "src/gallium/auxiliary/.*"_rr;
      t -= "src/gallium/auxiliary/draw/.*llvm.*"_rr;
      t -= "src/gallium/auxiliary/pipe-loader/.*"_rr;
      t -= "src/gallium/auxiliary/vl/.*"_rr;

      // llvm
      t -= "src/gallium/auxiliary/gallivm/.*"_rr;
      t -= "src/gallium/auxiliary/nir/nir_to_tgsi_info.c";

      // libdrm
      t -= "src/gallium/auxiliary/renderonly/renderonly.c";

      //
      t -= "src/gallium/auxiliary/indices/u_indices.c";
      t -= "src/gallium/auxiliary/indices/u_unfilled_indices.c";
      t -= "src/gallium/auxiliary/util/u_debug_stack_android.cpp";

      t.Public += includes;
      t.Public += nir;
   }

   auto &gallium_state_trackers = gallium.addDirectory("state_trackers");
   auto &wgl = gallium_state_trackers.addStaticLibrary("wgl");
   {
      auto &t = wgl;

      t += "src/gallium/state_trackers/wgl/.*"_rr;

      t.Public += "src/gallium/state_trackers/wgl"_idir;

      //t.Public += glapi;
      t.Public += common;
      t.Public += gallium_auxiliary;
      t.Public += gallium;
      t.Public += format;

      if (t.getBuildSettings().TargetOS.is(OSType::Windows))
      {
         t += "_GDI32_"_def;
         t += "BUILD_GL32"_def;
         t += "WIN32_LEAN_AND_MEAN"_def;

         t += "gdi32.lib"_slib;
         t += "user32.lib"_slib;
      }
   }

   auto &gallium_winsys = gallium.addDirectory("winsys");
   auto &gallium_winsys_sw = gallium_winsys.addDirectory("sw");
   auto &gallium_winsys_sw_gdi = gallium_winsys_sw.addStaticLibrary("gdi");
   {
      auto &t = gallium_winsys_sw_gdi;

      t += "src/gallium/winsys/sw/gdi/.*"_rr;
      t.Public += "src/gallium/winsys/sw"_idir;
      (t + includes)->IncludeDirectoriesOnly = true;
   }

   auto &gallium_drivers = gallium.addDirectory("drivers");
   auto &gallium_drivers_softpipe = gallium_winsys_sw.addStaticLibrary("softpipe");
   {
      auto &t = gallium_drivers_softpipe;

      t += "src/gallium/drivers/softpipe/.*"_rr;
      t.Public += "src/gallium/drivers"_idir;
      t.Protected += "GALLIUM_SOFTPIPE"_def;
      (t + includes)->IncludeDirectoriesOnly = true;
   }

   auto &gallium_targets = gallium.addDirectory("targets");
   auto &gl_gdi = gallium_targets.addLibrary("gl.gdi");
   {
      auto &t = gl_gdi;
      t += "src/gallium/targets/libgl-gdi/libgl_gdi.c";
      t += "src/gallium/state_trackers/wgl/opengl32.def";
      //t += "src/gallium/state_trackers/wgl/opengl32.mingw.def";
      t += wgl, gallium_winsys_sw_gdi, gallium_drivers_softpipe, glapi;
   }

   // not used
   {
      auto &glx = p.addStaticLibrary("glx");
      {
         auto &t = glx;
         t += "src/glx/.*"_r;
         t += "src/glx/windows/.*"_r;
         t.HeaderOnly = true;
         //t.Public += includes;
      }

      auto &dri_windows = glx.addStaticLibrary("dri.windows");
      {
         auto &t = dri_windows;
         t += "src/glx/windows/xwindowsdri.c";
         t += "src/glx/windows"_idir;
         t.HeaderOnly = true;
      }

      auto &glx_windows = glx.addStaticLibrary("windows");
      {
         auto &t = glx_windows;
         t += "src/glx/windows/windowsgl.c";
         t += "src/glx/windows/windows_drawable.c";
         t += "src/glx/windows/wgl.c";
         t += "src/glx/windows"_idir;
         t.Public += includes;
         t.Public += glapi;
      }
   }
}
