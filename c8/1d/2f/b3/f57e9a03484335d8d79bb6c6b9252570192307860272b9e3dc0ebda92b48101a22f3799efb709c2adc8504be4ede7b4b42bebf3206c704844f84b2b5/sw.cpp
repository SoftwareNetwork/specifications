void build(Solution &s)
{
    auto &gperf = s.addTarget<ExecutableTarget>("gnu.gperf", "3.1.0");
    gperf += RemoteFile("https://ftp.gnu.org/gnu/gperf/gperf-{M}.{m}.tar.gz");

    gperf +=
        "lib/.*\\.c.*"_rr,
        "lib/.*\\.h"_rr,
        "src/.*\\.c.*"_rr,
        "src/.*\\.h"_rr,
        "src/.*\\.icc"_rr;

    gperf.Public +=
        "lib"_id;
        
    gperf.writeFileOnce(gperf.BinaryPrivateDir / "config.h");
}