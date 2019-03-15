void build(Solution &s)
{
    auto &vulkan = s.addTarget<LibraryTarget>("khronos.vulkan", "1.1.103");
    vulkan += Git("https://github.com/KhronosGroup/Vulkan-Docs", "v{v}");
}
