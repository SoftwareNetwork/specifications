void build(Solution &s)
{
    auto &p = s.addStaticLibrary("vrogier.ocilib", "4.6.2");
    p += Git("https://github.com/vrogier/ocilib", "v{v}");

    // needs oci.h (oracle client interface)
    if (s.DryRun || s.PostponeFileResolving)
        return;
    throw SW_RUNTIME_ERROR("not implemented");
}
