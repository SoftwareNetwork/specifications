void build(Solution &s)
{
    auto &t = s.addStaticLibrary("dmlc.core", "master");
    t += Git("https://github.com/dmlc/dmlc-core");

    t -= "src/io/hdfs_filesys.cc";

    t += "NOMINMAX"_def;
    //t += "UNICODE"_def;
    t += "org.sw.demo.badger.curl.libcurl"_dep;

    t -= "src/io/azure_filesys.cc";
    //t += "org.sw.demo.Microsoft.WindowsAzure.Storage.cpp-6.1.0"_dep;

    //t.writeFileOnce(t.BinaryPrivateDir / "stdafx.h");
}
