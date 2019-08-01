#pragma sw require header org.sw.demo.google.protobuf.protoc-3

void build(Solution &s)
{
    auto &t = s.addTarget<StaticLibraryTarget>("google.libphonenumber.cpp", "8.10.15");
    t += Git("https://github.com/google/libphonenumber", "v{v}");

    gen_protobuf("org.sw.demo.google.protobuf-3"_dep, t, "resources/phonenumber.proto");
    gen_protobuf("org.sw.demo.google.protobuf-3"_dep, t, "resources/phonemetadata.proto");

    t.setRootDirectory("cpp");
    t += "org.sw.demo.unicode.icu.i18n"_dep;
}
