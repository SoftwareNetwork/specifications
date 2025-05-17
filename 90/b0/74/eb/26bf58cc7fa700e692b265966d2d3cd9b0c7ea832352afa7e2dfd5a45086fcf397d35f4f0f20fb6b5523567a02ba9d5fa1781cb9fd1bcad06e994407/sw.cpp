void build(Solution &s)
{
    auto &t = s.addLibrary("gpuopen.D3D12MemoryAllocator", "3.0.1");
    t += Git("https://github.com/GPUOpen-LibrariesAndSDKs/D3D12MemoryAllocator", "v{v}");

    t += "include/.*"_rr;
    t += "src/D3D12MemAlloc.cpp";
    t.Public += "src/D3D12MemAlloc.natvis";

    t += sw::Shared, "D3D12MA_EXPORTS"_def;
    t.Interface += sw::Shared, "D3D12MA_IMPORTS"_def;

    t += "uuid.lib"_slib;
}
