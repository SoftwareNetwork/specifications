void build(Solution &s)
{
    auto &dbus = s.addLibrary("freedesktop.dbus", "1.13.6");
    dbus += RemoteFile("https://dbus.freedesktop.org/releases/dbus/dbus-{v}.tar.gz");

    dbus.ApiNames.insert({ "DBUS_EXPORT", "DBUS_PRIVATE_EXPORT" });
    dbus += "cmake/config.h.cmake";
    dbus += "dbus/.*"_rr;
    dbus -= "dbus/.*test.*\\.c"_rr;

    dbus += "DBUS_COMPILATION"_def;

    dbus -= "dbus/.*unix.*\\.c"_rr;
    dbus -= "dbus/.*pthread.*\\.c"_rr;
    dbus -= "dbus/.*wince.*\\.c"_rr;
    dbus -= "dbus/.*epoll.*\\.c"_rr;

    dbus.Variables["DBUS_INT16_TYPE"] = "short";
    dbus.Variables["DBUS_INT32_TYPE"] = "int";
    dbus.Variables["DBUS_INT64_TYPE"] = "long long";
    dbus.Variables["DBUS_INT64_CONSTANT"] = "\"LL\"";
    dbus.Variables["DBUS_UINT64_CONSTANT"] = "\"ULL\"";

    dbus.Variables["DBUS_VERSION"] = dbus.Variables["PACKAGE_VERSION"];
    dbus.Variables["DBUS_MAJOR_VERSION"] = dbus.Variables["PACKAGE_VERSION_MAJOR"];
    dbus.Variables["DBUS_MINOR_VERSION"] = dbus.Variables["PACKAGE_VERSION_MINOR"];
    dbus.Variables["DBUS_MICRO_VERSION"] = dbus.Variables["PACKAGE_VERSION_PATCH"];

    // TODO: add config vars
    dbus.configureFile("cmake/config.h.cmake", dbus.BinaryPrivateDir / "config.h");
    dbus.configureFile("dbus/dbus-arch-deps.h.in", "dbus/dbus-arch-deps.h");
}
