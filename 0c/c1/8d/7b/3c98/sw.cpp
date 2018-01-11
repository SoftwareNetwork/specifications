void build(Solution &sln)
{
    auto &syncqt = sln.addTarget<ExecutableTarget>("syncqt", "master");
    syncqt.Source = Git("https://github.com/egorpugin/syncqt", "", "{v}");

    syncqt.Public += "pub.cppan2.demo.boost.program_options-1"_dep;
    syncqt.Public += "pub.cppan2.demo.boost.algorithm-1"_dep;
    syncqt.Public += "pub.egorpugin.primitives.filesystem-master"_dep;
}

