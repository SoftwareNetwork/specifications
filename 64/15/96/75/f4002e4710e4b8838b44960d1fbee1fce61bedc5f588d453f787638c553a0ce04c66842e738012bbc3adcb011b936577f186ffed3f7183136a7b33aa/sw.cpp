void build(Solution &s)
{
    auto &t = s.addTarget<StaticLibraryTarget>("amazon.awslabs.c_mqtt", "0.3.7");
    t += Git("https://github.com/awslabs/aws-c-mqtt", "v{v}");

    t += "org.sw.demo.amazon.awslabs.c_io"_dep;
}
