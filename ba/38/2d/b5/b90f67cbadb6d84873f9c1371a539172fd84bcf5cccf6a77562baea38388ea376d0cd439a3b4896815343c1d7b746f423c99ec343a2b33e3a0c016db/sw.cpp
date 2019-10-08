void build(Solution &s)
{
    auto &t = s.addLibrary("Wohlstand.libOPNMIDI", "master");
    t += Git("https://github.com/Wohlstand/libOPNMIDI");

    t.ApiName = "OPNMIDI_DECLSPEC";
    t.Public += "OPNMIDI_BUILD"_def;

    t.writeFileOnce(t.BinaryPrivateDir / "unistd.h");
}
