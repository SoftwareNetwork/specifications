//#pragma 4sw require header org.sw.demo.google.grpc.cpp.plugin

//#define addProtoLibrary addStaticLibrary

void build(Solution &s)
{
    auto &p = s.addProject("google.cloud", "2.16.0");
    p += Git("https://github.com/googleapis/google-cloud-cpp", "v{v}");

    auto &storage = p.addStaticLibrary("storage");
    storage += "google/cloud/storage/.*"_r;
    storage -= "google/cloud/storage/.*_test.cc"_r;
    storage.Public += "org.sw.demo.google.abseil"_dep;
    storage.Public += "org.sw.demo.nlohmann.json"_dep;
    storage.Public += "org.sw.demo.badger.curl.libcurl"_dep;
    storage.Public += "org.sw.demo.google.googleapis.storage-master"_dep;
}
