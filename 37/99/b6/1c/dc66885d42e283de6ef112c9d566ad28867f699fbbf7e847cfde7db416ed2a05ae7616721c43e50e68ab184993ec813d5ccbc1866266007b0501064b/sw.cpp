void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("kisli.vmime", "master");
    t += Git("https://github.com/kisli/vmime");

    t.ApiName = "VMIME_EXPORT";

    if (t.getBuildSettings().TargetOS.is(OSType::Windows))
    {
        t.Variables["VMIME_PLATFORM_IS_WINDOWS"] = 1;
    }
    else
    {
        t.Variables["VMIME_PLATFORM_IS_POSIX"] = 1;
        t.Variables["VMIME_HAVE_PTHREAD"] = 1;
    }

    t.Variables["VMIME_8BIT_TYPE"] = "std::int8_t";
    t.Variables["VMIME_16BIT_TYPE"] = "std::int16_t";
    t.Variables["VMIME_32BIT_TYPE"] = "std::int32_t";
    t.Variables["VMIME_64BIT_TYPE"] = "std::int64_t";
    t.Variables["VMIME_HAVE_CSTDINT"] = 1;
    t.Variables["VMIME_HAVE_SIZE_T"] = 1;

    t.Variables["VMIME_HAVE_TLS_SUPPORT"] = 1;
    t.Variables["VMIME_TLS_SUPPORT_LIB_IS_OPENSSL"] = 1;
    t.Variables["VMIME_CHARSETCONV_LIB_IS_ICU"] = 1;

    t.Variables["VMIME_BYTE_ORDER_LITTLE_ENDIAN"] = 1;

    t.configureFile("cmake/config.hpp.cmake", "vmime/config.hpp");
    t.writeFileOnce("vmime/export-shared.hpp");
}
