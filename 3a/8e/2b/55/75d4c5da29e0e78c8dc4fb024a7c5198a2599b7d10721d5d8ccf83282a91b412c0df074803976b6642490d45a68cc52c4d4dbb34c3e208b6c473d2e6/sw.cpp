void build(Solution &s)
{
    auto &libsodium = s.addTarget<LibraryTarget>("jedisct1.libsodium", "1.0.18");
    libsodium += Git("https://github.com/jedisct1/libsodium", "{v}");

    libsodium +=
        "src/.*\\.[hc]"_rr,
        "src/.*\\.in"_rr;

    libsodium.Private +=
        "src/libsodium/include/sodium"_id;

    libsodium.Public +=
        "src/libsodium/include"_id;

    libsodium.Public += sw::Shared, "SODIUM_DLL_EXPORT"_d;
    libsodium.Public += sw::Static, "SODIUM_STATIC"_d;

    libsodium.Variables["SODIUM_LIBRARY_VERSION_MAJOR"] = libsodium.Variables["PACKAGE_VERSION_MAJOR"];
    libsodium.Variables["SODIUM_LIBRARY_VERSION_MINOR"] = libsodium.Variables["PACKAGE_VERSION_MINOR"];

    libsodium.patch("src/libsodium/include/sodium/version.h.in", "#include \"export.h\"", "#include \"sodium/export.h\"");
    libsodium.configureFile("src/libsodium/include/sodium/version.h.in", "version.h");
    libsodium.configureFile("src/libsodium/include/sodium/version.h.in", "sodium/version.h");
}
