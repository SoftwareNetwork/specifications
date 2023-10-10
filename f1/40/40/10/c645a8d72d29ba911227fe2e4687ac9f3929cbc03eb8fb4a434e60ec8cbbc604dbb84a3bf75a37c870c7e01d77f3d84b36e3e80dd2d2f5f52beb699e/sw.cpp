void build(Solution &s)
{
    auto &p = s.addProject("google.cloud", "2.16.0");
    p += Git("https://github.com/googleapis/google-cloud-cpp", "v{v}");

    auto &internal = p.addStaticLibrary("internal");
    internal += "google/cloud/internal/.*"_r;
    internal -= "google/cloud/internal/.*_test.cc"_rr;
    internal -= "google/cloud/internal/.*_benchmark.cc"_rr;
    internal.Public += "org.sw.demo.google.abseil"_dep;
    internal.Public += "org.sw.demo.nlohmann.json"_dep;
    internal.Public += "org.sw.demo.badger.curl.libcurl"_dep;
    internal.Public += "org.sw.demo.google.grpc.cpp"_dep;
    internal.Public += "org.sw.demo.google.googleapis.iam-master"_dep;

    auto &storage = p.addStaticLibrary("storage");
    storage += "google/cloud/storage/.*"_r;
    storage += "google/cloud/storage/internal/.*"_rr;
    storage += "google/cloud/storage/oauth2/.*"_rr;
    storage -= "google/cloud/storage/.*_test.cc"_rr;
    storage -= "google/cloud/storage/.*_benchmark.cc"_rr;
    storage.Public += "org.sw.demo.google.googleapis.storage-master"_dep;
    storage.Public += "org.sw.demo.google.crc32c"_dep;
    storage.Public += internal;
}
