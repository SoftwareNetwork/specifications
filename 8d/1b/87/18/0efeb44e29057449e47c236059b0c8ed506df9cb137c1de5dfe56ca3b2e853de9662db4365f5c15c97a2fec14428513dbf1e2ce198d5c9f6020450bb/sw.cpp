void build(Solution &s)
{
    auto &murmurhash3 = s.addTarget<StaticLibraryTarget>("aappleby.smhasher.murmurhash3", "master");
    murmurhash3 += Git("https://github.com/aappleby/smhasher", "", "{v}");

    murmurhash3 +=
        "src/MurmurHash3.cpp",
        "src/MurmurHash3.h";
}