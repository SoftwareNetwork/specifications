void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("khronos.vulkan", "1.1.103");
    t += Git("https://github.com/KhronosGroup/Vulkan-Headers", "v{v}");

    t += "include/.*"_rr;
    t += "registry/.*"_rr;
}
