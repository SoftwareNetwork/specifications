void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("amazon.awslabs.c_mqtt", "0.13.3");
    t += Git("https://github.com/awslabs/aws-c-mqtt", "v{v}");

    t.Public += "AWS_MQTT_WITH_WEBSOCKETS"_def;

    t.Private += sw::Shared, "AWS_MQTT_EXPORTS"_d;
    t.Public += sw::Shared, "AWS_MQTT_USE_IMPORT_EXPORT"_d;

    t.Public += "org.sw.demo.amazon.awslabs.c_http"_dep;
    t.Public += "org.sw.demo.amazon.awslabs.c_io"_dep;
}
