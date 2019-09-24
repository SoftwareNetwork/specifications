void build(Solution &s)
{
    auto &p = s.addProject("google.angle", "0.0.3803");
    p += Git("https://chromium.googlesource.com/angle/angle", "", "chromium/{p}");

    auto &third_party = p.addDirectory("third_party");

    auto &base = third_party.addTarget<StaticLibraryTarget>("base");
    //base.ApiName = "ANGLEBASE_EXPORT";
    base += "src/common/third_party/base/.*"_rr;
    base -= ".*unittest.cc"_rr;
    base.Public += "src/common/third_party/base"_idir;

    auto &khronos = third_party.addTarget<LibraryTarget>("khronos");
    khronos += "src/third_party/khronos/.*\\.h"_rr;
    khronos.Public += "src/third_party/khronos"_idir;

    auto &pmurhash = third_party.addTarget<StaticLibraryTarget>("pmurhash");
    pmurhash += "src/common/third_party/smhasher/.*"_rr;

    auto &xxhash = third_party.addTarget<StaticLibraryTarget>("xxhash");
    xxhash += "src/common/third_party/xxhash/xxhash\\.[hc]"_rr;

    //
    auto &common = p.addTarget<StaticLibraryTarget>("common");
    common +=
        "include/.*"_rr,
        "src/common/[^/]*\\.cpp"_rr,
        "src/common/[^/]*\\.h"_rr,
        "src/common/[^/]*\\.inc"_rr,
        "src/compiler/.*\\.h"_rr,
        "src/third_party/.*\\.h"_rr;

    common.Public +=
        "include"_id,
        "src"_id;

    common -= ".*unittest.cpp"_rr;
    common -= ".*linux.cpp"_rr;
    common -= ".*mac.cpp"_rr;
    common -= ".*android.*"_rr;
    common -= ".*posix.*"_rr;

    if (common.getBuildSettings().TargetOS.Type == OSType::Windows)
    {
        common.Public += "NOMINMAX"_d;
#ifdef SW_CPP_DRIVER_API_VERSION
        common.Public += "com.Microsoft.Windows.SDK.winrt"_dep;
#endif
    }
    common.Public += base, pmurhash;

    //
    auto &preprocessor = p.addTarget<StaticLibraryTarget>("compiler.preprocessor");
    preprocessor +=
        "src/compiler/preprocessor/.*\\.cpp"_rr,
        "src/compiler/preprocessor/.*\\.h"_rr;
    preprocessor.Public += common;

    //
    auto &compiler = third_party.addTarget<StaticLibraryTarget>("compiler");
    compiler += "src/third_party/compiler/.*"_rr;
    compiler.Public += common;

    //
    auto &translator = p.addTarget<StaticLibraryTarget>("compiler.translator");
    translator +=
        "src/compiler/translator/.*\\.cpp"_rr,
        "src/compiler/translator/.*\\.h"_rr;
    translator.Public +=
        "src"_id;
    translator.Private += "LIBANGLE_IMPLEMENTATION"_d;
    if (translator.getBuildSettings().TargetOS.Type == OSType::Windows)
    {
        translator.Public += "ANGLE_ENABLE_GLSL"_d;
        translator.Public += "ANGLE_ENABLE_HLSL"_d;
    }
    translator.Public += preprocessor;
    translator.Public += compiler;

    //
    auto &systeminfo = third_party.addTarget<StaticLibraryTarget>("systeminfo");
    systeminfo += "src/third_party/systeminfo/.*"_rr;
    systeminfo.Public += common;

    //
    auto &image = p.addTarget<StaticLibraryTarget>("util.image");
    image += "src/image_util/.*"_rr;
    image.Public += common;

    //
    auto &angle = p.addTarget<StaticLibraryTarget>("angle");
    angle +=
        "src/libANGLE/.*"_rr;
    angle -=
        "src/libANGLE/renderer/gl/egl/android/.*"_rr,
        "src/libANGLE/renderer/gl/egl/ozone/.*"_rr,
        "src/libANGLE/renderer/gl/glx/.*"_rr,
        "src/libANGLE/renderer/vulkan/.*"_rr;
    angle += "src/commit.h";
    angle -= ".*unittest.cpp"_rr;
    angle -= ".*EGLDL.cpp"_rr;
    angle.Public +=
        "src"_id;
    angle.Private += "LIBANGLE_IMPLEMENTATION"_d;
    if (angle.getBuildSettings().TargetOS.Type == OSType::Windows)
    {
        angle += "src/libANGLE/renderer/d3d.*"_rr;

        angle.Public += "ANGLE_ENABLE_D3D11"_d;
        angle.Public += "ANGLE_ENABLE_D3D9"_d;

        angle.Public += "d3d11.lib"_slib;
        angle.Public += "d3d9.lib"_slib;
        angle.Public += "dxguid.lib"_slib;
        angle.Public += "User32.lib"_slib;
    }
    angle -=
        "src/libANGLE/renderer/d3d/d3d11/winrt/.*"_rr;

    angle.Public += pmurhash, xxhash;
    angle.Public += image;
    angle.Public += translator;
    angle.Public += khronos;
    angle.Public += systeminfo;
    //angle += "org.sw.demo.Cyan4973.xxHash"_dep;

    angle.configureFile("src/commit.h", "id/commit.h");

    //
    auto &gles_v2 = p.addTarget<LibraryTarget>("gles_v2");
    {
        gles_v2.ApiNames.insert("ANGLE_EXPORT");
        gles_v2 +=
            "src/libGLESv2/.*\\.cpp"_rr,
            "src/libGLESv2/.*\\.h"_rr;
        gles_v2.Public +=
            "src"_id;
        gles_v2.Public += "EGLAPIENTRY=__cdecl"_d;
        gles_v2.Public += angle;

        auto d = angle + gles_v2;
        d->IncludeDirectoriesOnly = true;
    }

    //
    auto &egl = p.addTarget<LibraryTarget>("egl");
    {
        egl +=
            "src/libEGL/libEGL.cpp",
            "src/libEGL/libEGL.def";
        egl.Public +=
            "src"_id;
        egl.Public += gles_v2;
    }
}
