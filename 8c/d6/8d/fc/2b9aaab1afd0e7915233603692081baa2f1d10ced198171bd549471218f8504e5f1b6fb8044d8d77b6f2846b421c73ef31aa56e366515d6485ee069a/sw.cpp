void build(Solution &s)
{
    auto &sdl = s.addProject("valve.sdl", "2.0.4");
    sdl += RemoteFile("https://www.libsdl.org/projects/SDL_mixer/release/SDL2_mixer-{v}.zip");

    auto &t = sdl.addTarget<LibraryTarget>("mixer");
    {
        t.ApiName = "DECLSPEC";

        t += ".*\\.[hc]"_r;
        t += "native_midi/.*"_rr;
        t += "timidity/.*"_rr;
        t -= "play.*"_r;

        t += "MUSIC_WAV"_def;
        t += "MUSIC_MID_TIMIDITY"_def;
        t += "MUSIC_MID_NATIVE"_def;

        t.Public += "org.sw.demo.valve.sdl.sdl"_dep;

        if (t.getBuildSettings().TargetOS.Type != OSType::Windows)
            t += "MUSIC_CMD"_def;

        t += "MUSIC_OPUS"_def;
        t += "org.sw.demo.xiph.opusfile"_dep;

        t += "MUSIC_FLAC"_def;
        t += "org.sw.demo.xiph.flac.libflac"_dep;

        t += "MUSIC_OGG"_def;
        t += "org.sw.demo.xiph.vorbis.vorbisfile"_dep;

        t += "MUSIC_MOD_MODPLUG"_def;
        t += "org.sw.demo.konstanty.libmodplug-master"_dep;

        t += "MUSIC_MOD_MIKMOD"_def;
        t += "org.sw.demo.sezero.mikmod"_dep;

        t += "MUSIC_MP3_MPG123"_def;
        t += "off_t=int"_def;
        t += "org.sw.demo.mpg123.mpg123"_dep;

        t += "MUSIC_MP3_MAD"_def;
        t += "org.sw.demo.libmad"_dep;

        t += "MUSIC_MID_FLUIDSYNTH"_def;
        t += "org.sw.demo.FluidSynth.fluidsynth"_dep;

        // install
        if (!t.DryRun)
        {
            auto d = t.BinaryDir / "SDL2";
            if (!fs::exists(d))
            {
                fs::create_directories(d);
                for (auto &[p, sf] : t["SDL_mixer.h"_r])
                    fs::copy_file(p, d / p.filename(), fs::copy_options::update_existing);
            }
        }
    }
}
