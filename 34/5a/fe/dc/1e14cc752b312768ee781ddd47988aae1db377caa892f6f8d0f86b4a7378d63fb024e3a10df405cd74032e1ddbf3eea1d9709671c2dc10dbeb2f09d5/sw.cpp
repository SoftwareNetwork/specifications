void build(Solution &s)
{
    auto &t = s.addTarget<Library>("wjakob.nanogui", "master");
    t += Git("https://github.com/wjakob/nanogui", "", "{v}");
    t -= "src/example.*"_rr;
    t += "org.sw.demo.eigen"_dep;
    t += "org.sw.demo.glfw"_dep;
    t += "org.sw.demo.memononen.NanoVG-master"_dep;
}
