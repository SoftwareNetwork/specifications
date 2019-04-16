void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("khronos.vulkan.headers", "1.1.105");
    t += Git("https://github.com/KhronosGroup/Vulkan-Headers", "v{v}");

    t += "include/.*"_rr;
    t += "registry/.*"_rr;
}
