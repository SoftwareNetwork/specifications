void build(Solution &s)
{
    auto &t = s.addLibrary("Microsoft.WindowsAzure.Storage.cpp", "6.1.0");
    t += Git("https://github.com/Azure/azure-storage-cpp", "v{v}");
    t.setRootDirectory("Microsoft.WindowsAzure.Storage");

    t += "UNICODE"_def;

    t.Public += sw::Static, "_NO_WASTORAGE_API"_def;
    t += sw::Shared, "WASTORAGE_DLL"_def;

    t.Public += "org.sw.demo.microsoft.cpprestsdk"_dep;
    t.Public += "com.Microsoft.VisualStudio.VC.ATLMFC"_dep;

    t += "XmlLite.lib"_slib;
    t += "Rpcrt4.lib"_slib;
}
