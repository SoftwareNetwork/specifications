void build(Solution &s)
{
    auto &drm = s.addLibrary("mesa.drm", "2.4.101");
    drm += Git("https://gitlab.freedesktop.org/mesa/drm", "libdrm-{v}");

    {
        auto &t = drm;
        t += "include/.*"_rr;
        t += ".*\\.[hc]"_r;

        t.Public += "include"_idir;
        t.Protected += "."_idir;
        t.Protected += "include/drm"_idir; // public?

        t.Public += "HAVE_VISIBILITY"_def;
        t.Protected += "MAJOR_IN_SYSMACROS"_def;
        t.Public += "HAVE_LIBDRM_ATOMIC_PRIMITIVES"_def;
    }

    auto add_drm = [&drm](const String &name) -> decltype(auto)
    {
        auto &t = drm.addLibrary(name);
        t += FileRegex(name, ".*\\.[hc]", false);
        t.Protected += IncludeDirectory(name); // make public?
        t.Public += drm;
        return t;
    };

#define ADD_DRM(x) auto &x = add_drm(#x)

    ADD_DRM(amdgpu);
    amdgpu += Definition("AMDGPU_ASIC_ID_TABLE=\"" + normalize_path(amdgpu.SourceDir / "data/amdgpu.ids") + "\"");

    ADD_DRM(etnaviv);
    ADD_DRM(exynos);

    ADD_DRM(freedreno);
    freedreno += "freedreno/msm/.*"_rr;

    ADD_DRM(intel);
    intel += "org.sw.demo.xorg.pciaccess"_dep;

    ADD_DRM(nouveau);
    ADD_DRM(omap);
    ADD_DRM(radeon);
    ADD_DRM(tegra);

    auto &kms = drm.addLibrary("kms");
    kms += "libkms/.*\\.[hc]"_rr;
    kms += exynos, intel, radeon, nouveau;
}
