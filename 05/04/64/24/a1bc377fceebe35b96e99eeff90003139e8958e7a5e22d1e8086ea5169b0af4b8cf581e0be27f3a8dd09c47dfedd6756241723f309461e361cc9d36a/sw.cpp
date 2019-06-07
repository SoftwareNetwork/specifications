void build(Solution &s)
{
    auto &p = s.addProject("ArtifexSoftware", "1.15.0");
    p += Git("git://git.ghostscript.com/mupdf.git");

	auto &bin2coff = p.addTarget<Executable>("bin2coff");
	bin2coff += "scripts/bin2coff.c";

    auto &t = p.addTarget<StaticLibrary>("mupdf");
	{
		t += "include/.*"_rr;
		t += "source/.*"_rr;
		t -= "source/tools/.*"_rr;
		t -= "source/tests/.*"_rr;

		t += "resources/fonts/droid/.*\\.[ot]tf"_rr;
		t += "resources/fonts/noto/.*\\.[ot]tf"_rr;
		t += "resources/fonts/urw/.*\\.cff"_rr;
		t += "resources/fonts/sil/.*\\.cff"_rr;
		t += "resources/fonts/han/.*\\.ttc"_rr;
		auto make_obj = [&t, &bin2coff](auto r)
		{
			for (auto &[fn, f] : t[r])
			{
				auto c = t.addCommand();
				c << cmd::prog(bin2coff)
					<< cmd::in(fn)
					<< cmd::out(fn.filename() += ".obj")
					<< "_binary_" + boost::replace_all_copy(fn.filename().u8string(), ".", "_")
					<< "x64"
					;
			}
		};
		make_obj("resources/fonts/droid/.*\\.[ot]tf"_rr);
		make_obj("resources/fonts/noto/.*\\.[ot]tf"_rr);
		make_obj("resources/fonts/urw/.*\\.cff"_rr);
		make_obj("resources/fonts/sil/.*\\.cff"_rr);
		make_obj("resources/fonts/han/.*\\.ttc"_rr);

		t.Public += "org.sw.demo.ArtifexSoftware.jbig2dec"_dep;
		t.Public += "org.sw.demo.mgk25.jbig.jbig"_dep;
		t.Public += "org.sw.demo.behdad.harfbuzz"_dep;
		t.Public += "org.sw.demo.jpeg"_dep;
		t.Public += "org.sw.demo.ghostscript.thirdparty.lcms2-lcms2mt"_dep;
		t.Public += "org.sw.demo.ghostscript.mujs-master"_dep;
		t.Public += "org.sw.demo.uclouvain.openjpeg.openjp2"_dep;

		t.patch("include/mupdf/fitz/system.h", "#define hypotf _hypotf", "//#define  hypotf _hypotf");
	}

	auto &mutool = p.addTarget<Executable>("mutool");
	mutool += "source/tools/.*"_rr;
	mutool -= "source/tools/muraster.c";
	mutool += t;
}
