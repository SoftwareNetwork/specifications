void build(Solution &s)
{
    auto &drm = s.addLibrary("mesa.drm", "2.4.100");
    drm += Git("https://gitlab.freedesktop.org/mesa/drm", "libdrm-{v}");

    {
        auto &t = drm;
        t += "include/.*"_rr;
        t += ".*\\.[hc]"_r;

        t.Public += "include"_idir;
        t.Protected += "."_idir;
        t.Protected += "include/drm"_idir; // public?

        t.Public += "HAVE_VISIBILITY"_def;

        t += "MAJOR_IN_SYSMACROS"_def;

        t.Public += "HAVE_LIBDRM_ATOMIC_PRIMITIVES"_def;

        t.Protected.LinkOptions.push_back("--no-undefined");
    }

    auto add_drm = [&drm](const String &name) -> decltype(auto)
    {
        auto &t = drm.addLibrary(name);
        t += FileRegex(name, ".*\\.[hc]", false);
        t += IncludeDirectory(name);
        t.Public += drm;
        return t;
    };

    auto &amdgpu = add_drm("amdgpu");
    amdgpu += Definition("AMDGPU_ASIC_ID_TABLE=\"" + normalize_path(amdgpu.SourceDir / "data/amdgpu.ids") + "\"");

    add_drm("etnaviv");
    add_drm("exynos");

    auto &freedreno = add_drm("freedreno");
    freedreno += "freedreno/msm/.*"_rr;

    add_drm("intel") += "org.sw.demo.xorg.pciaccess"_dep;
    add_drm("nouveau");
    add_drm("omap");
    add_drm("radeon");
    add_drm("tegra");
}
