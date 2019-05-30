void build(Solution &s)
{
    auto &fb = s.addProject("firebird", "4.0.0");
	fb += Git("https://github.com/FirebirdSQL/firebird", "", "master");

    auto &tools = fb.addDirectory("extern");
    tools.setSourceDirectory("extern");

    auto &tommath = tools.addStaticLibrary("tommath");
    tommath.setRootDirectory("libtommath");

    auto &tomcrypt = tools.addStaticLibrary("tomcrypt");
    tomcrypt.setRootDirectory("libtomcrypt");
    tomcrypt.Public += "src/headers"_idir;
    tomcrypt += "LTM_DESC"_def;
    //tomcrypt += "LTC_NO_ROLC"_def;
    tomcrypt += "LTC_SOURCE"_def;
	tomcrypt += tommath;

    auto &decNumber = tools.addStaticLibrary("decNumber");
    decNumber.setRootDirectory("decNumber");
    decNumber += ".*"_rr;
    decNumber -= "decBasic.c";
    decNumber -= "decCommon.c";

    auto &btyacc = tools.addExecutable("btyacc");
    btyacc.setRootDirectory("btyacc");
	btyacc += "WIN32"_def;

    auto &cloop = tools.addExecutable("cloop");
    cloop.setRootDirectory("cloop");
    cloop += "src/cloop/.*\\.[ch]p?p?"_rr;

	auto setup = [](auto &t)
	{
		t.Public += "src/include"_idir;
	};

    auto &common = fb.addStaticLibrary("common");
	{
		setup(common);
		common.setRootDirectory("src/common");
		common +=
			".*\\.cpp"_r,
			"classes/.*\\.cpp"_r,
			"config/.*\\.cpp"_r,
			"sha2/.*\\.cpp"_r,
			".*\\.h"_rr;
		common += "classes"_idir, "config"_idir;
		if (common.getSettings().TargetOS.is(OSType::Windows))
		{
			common += "os/win32/.*\\.cpp"_rr;
			common.Public += "ws2_32.lib"_lib;
			common.Public += "User32.lib"_lib;
			common.Public += "shell32.lib"_lib;
			common.Public += "ole32.lib"_lib;
			common.Public += "mpr.lib"_lib;
		}
		else
			common += "os/posix/.*\\.cpp"_rr;
		if (common.getSettings().TargetOS.is(OSType::Macos))
		{
			common += "os/darwin/.*\\.cpp"_rr;
			common -= "os/posix/mod_loader.cpp";
		}
		common.Public += tommath, tomcrypt, decNumber,
			"org.sw.demo.madler.zlib"_dep;
		auto icuuc = common.Public + "org.sw.demo.unicode.icu.common"_dep;
		auto icuin = common.Public + "org.sw.demo.unicode.icu.i18n"_dep;
		common.Public += icuin;
		s.registerCallback([&common, icuuc, icuin](auto &t, auto cbt)
		{
			if (cbt != sw::CallbackType::EndPrepare)
				return;
			if (t.getPackage() == common.getPackage())
			{
				common.patch("unicode_util.cpp", "icuuc%s.dll", ((NativeExecutedTarget*)icuuc->target)->getOutputFile().filename().u8string());
				common.patch("unicode_util.cpp", "icuin%s.dll", ((NativeExecutedTarget*)icuin->target)->getOutputFile().filename().u8string());
			}
		});
	}

	auto &gpre_common = fb.addStaticLibrary("gpre.common");
	{
		gpre_common.setRootDirectory("src/gpre");
		gpre_common += ".*\\.cpp"_r, ".*\\.h"_rr;
		gpre_common += "languages/...\\.cpp"_rr;
		gpre_common +=
			"GPRE_ADA"_def,
			"GPRE_COBOL"_def,
			"GPRE_FORTRAN"_def,
			"GPRE_PASCAL"_def;
		gpre_common.Public += common; // for icu and idirs
	}

    auto &boot = fb.addExecutable("gpre.boot");
    boot += "src/gpre/boot/gpre_meta_boot.cpp", "src/yvalve/gds.cpp";
	boot += gpre_common;

	//
	auto gpre_boot_cmd = [&boot](auto &t, const Strings &args, const path &in)
	{
		auto c = t.addCommand();
		c << cmd::prog(boot);
		for (auto &a : args)
			c << a;
		c << cmd::in(in) << cmd::out(in.filename() += ".boot.cpp");
		return c;
	};

	auto &boot_alice = boot.addStaticLibrary("alice");
	{
		setup(boot_alice);
		boot_alice += "src/alice/.*"_r;
		gpre_boot_cmd(boot_alice, { "-n", "-m" }, "src/alice/alice_meta.epp");
	}

	auto &boot_burp = boot.addStaticLibrary("burp");
	{
		setup(boot_burp);
		boot_burp += "src/burp/.*"_r;
		boot_burp += "src/burp"_idir;
		for (auto f : {"src/burp/backup.epp","src/burp/OdsDetection.epp","src/burp/restore.epp"})
			gpre_boot_cmd(boot_burp, { "-lang_internal", "-n", "-ids", "-ocxx", "-m" }, f);
		boot_burp.Public += common; // for icu and idirs
	}

	auto &parse = boot.addStaticLibrary("parse");
	{
		setup(parse);
		parse += "org.sw.demo.unicode.icu.i18n"_dep;
		{
			auto c = parse.addCommand();
			c << cmd::prog("org.sw.demo.gnu.sed.sed"_dep)
				<< "-n" << "/%type .*/p" << cmd::in("src/dsql/parse.y")
				<< cmd::std_out("types.y");
		}
		{
			auto c = parse.addCommand();
			c << cmd::prog("org.sw.demo.gnu.sed.sed"_dep)
				<< "s/%type .*//" << cmd::in("src/dsql/parse.y")
				<< cmd::std_out("y.y");
		}
		{
			auto c = parse.addCommand();
			c << cmd::prog(btyacc)
				<< cmd::wdir(parse.BinaryDir)
				<< "-l" << "-d" << "-S" << cmd::in("src/dsql/btyacc_fb.ske")
				<< cmd::in("y.y")
				<< cmd::end()
				<< cmd::in("types.y")
				<< cmd::out("y_tab.h")
				<< cmd::out("y_tab.c", cmd::Skip)
				;
		}
		{
			auto c = parse.addCommand();
			c << cmd::prog("org.sw.demo.gnu.sed.sed"_dep)
				<<
				"s/#define \\([A-Z].*\\)/#define TOK_\\1/g;"
				"s/#define TOK_YY\\(.*\\)/#define YY\\1/g;"
				<< cmd::in("y_tab.h")
				<< cmd::std_out("gen/parse.h");
		}
		{
			path in = parse.BinaryDir / "y_tab.c";
			path out = parse.BinaryDir / "parse.cpp";

			SW_MAKE_EXECUTE_BUILTIN_COMMAND_AND_ADD(copy_cmd, parse, "sw_copy_file", nullptr);
			copy_cmd->args.push_back(in.u8string());
			copy_cmd->args.push_back(out.u8string());
			copy_cmd->addInput(in);
			copy_cmd->addOutput(out);
			parse += out;
		}
	}

	auto &remote = fb.addStaticLibrary("remote");
	{
		setup(remote);
		remote +=
			"src/remote/.*\\.h"_rr,
			"src/remote/merge.cpp",
			"src/remote/parser.cpp",
			"src/remote/protocol.cpp",
			"src/remote/remote.cpp",
			"src/remote/inet.cpp",

			"src/auth/SecureRemotePassword/srp.cpp",
			"src/auth/SecureRemotePassword/srp.h",
			"src/auth/SecureRemotePassword/Message.h",
			"src/auth/trusted/AuthSspi.cpp"
			;
		if (remote.getSettings().TargetOS.is(OSType::Windows))
			remote += "src/remote/os/win32/.*"_rr;
		remote.Public += tommath;
		remote.Public += "org.sw.demo.madler.zlib"_dep;
	}

	auto &ib_util = fb.addLibrary("ib_util");
	{
		setup(ib_util);
		ib_util += "builds/win32/defs/ib_util.def";
		ib_util += "src/extlib/ib_util.cpp";
		ib_util += "src/extlib/ib_util.h";
	}

	auto add_yvalve = [&setup, &common, &remote, &parse](auto &base, const auto &gpre_cmd) -> decltype(auto)
	{
		auto &boot_yvalve = base.addLibrary("yvalve");
		setup(boot_yvalve);
		boot_yvalve += "builds/win32/defs/firebird.def";
		boot_yvalve += "src/yvalve/.*\\.h"_rr;
		boot_yvalve += "src/yvalve/.*\\.cpp"_rr;
		boot_yvalve -= "src/yvalve/config/os/.*"_rr;
		if (boot_yvalve.getSettings().TargetOS.is(OSType::Windows))
			boot_yvalve += "src/yvalve/config/os/win32/.*\\.c.*"_rr;
		else if (boot_yvalve.getSettings().TargetOS.is(OSType::Macos))
			boot_yvalve += "src/yvalve/config/os/darwin/.*\\.c.*"_rr;
		else
			boot_yvalve += "src/yvalve/config/os/posix/.*\\.c.*"_rr;
		boot_yvalve +=
			"src/auth/SecureRemotePassword/client/SrpClient.cpp",
			"src/auth/SecurityDatabase/LegacyClient.cpp",
			"src/plugins/crypt/arc4/Arc4.cpp",
			"src/remote/client/BlrFromMessage.cpp",
			"src/remote/client/interface.cpp"
			;
		if (boot_yvalve.getSettings().TargetOS.is(OSType::Windows))
			boot_yvalve += "src/jrd/os/win32/ibinitdll.cpp";
		for (auto f : {"src/yvalve/array.epp","src/yvalve/blob.epp"})
			gpre_cmd(boot_yvalve, { "-n", "-m" }, f);
		boot_yvalve.Public += common, remote, parse;
		boot_yvalve.Public += "org.sw.demo.unicode.icu.i18n"_dep;
		return boot_yvalve;
	};

	auto &engine_common = fb.addStaticLibrary("engine_common");
	{
		setup(engine_common);
		engine_common +=
			"src/include/gen/iberror.h",
			"src/dsql/.*\\.h"_rr,
			"src/dsql/.*\\.cpp"_r,
			"src/jrd/.*\\.h"_rr,
			"src/jrd/.*\\.cpp"_r,
			"src/jrd/extds/.*\\.cpp"_rr,
			"src/jrd/recsrc/.*\\.cpp"_rr,
			"src/jrd/replication/.*\\.cpp"_rr,
			"src/jrd/trace/.*\\.cpp"_rr
			;
		engine_common -= "src/jrd/jrd.cpp";
		engine_common += "src/include/gen"_id;
		engine_common -= "src/jrd/os/.*"_rr;
		if (engine_common.getSettings().TargetOS.is(OSType::Windows))
			engine_common += "src/jrd/os/win32/.*\\.c.*"_rr;
		else
			engine_common += "src/jrd/os/posix/.*\\.c.*"_rr;
		engine_common +=
			"src/lock/lock.cpp",
			"src/utilities/gsec/gsec.cpp",
			"src/utilities/gstat/ppg.cpp",
			"src/utilities/nbackup/nbackup.cpp"
			;
		engine_common.Public += common, parse;
	}

	auto &boot_yvalve = add_yvalve(boot, gpre_boot_cmd);

	auto &boot_engine = boot.addLibrary("engine");
	{
		setup(boot_engine);
		boot_engine.setOutputDir("plugins");
		boot_engine += "builds/win32/defs/plugin.def";
		boot_engine += engine_common, boot_alice, boot_burp, boot_yvalve;
		(boot_engine + ib_util)->Dummy = true;

		boot_engine += "src/jrd/jrd.cpp";
		boot_engine.patch("src/jrd/jrd.cpp", "CURRENT_ENGINE", "SW_CURRENT_ENGINE");
		boot_engine += Definition("SW_CURRENT_ENGINE=\"" + boot_engine.getOutputFile().filename().u8string() + "\"");

		for (auto f : {
			"src/dsql/DdlNodes.epp",
			"src/dsql/metd.epp",
			"src/dsql/PackageNodes.epp",
			"src/jrd/dfw.epp",
			"src/jrd/dpm.epp",
			"src/jrd/dyn_util.epp",
			"src/jrd/fun.epp",
			"src/jrd/Function.epp",
			"src/jrd/grant.epp",
			"src/jrd/ini.epp",
			"src/jrd/met.epp",
			"src/jrd/scl.epp",
			})
		{
			gpre_boot_cmd(boot_engine, { "-n", "-ids", "-gds_cxx" }, f);
		}
		gpre_boot_cmd(boot_engine, { "-n", "-m" }, "src/utilities/gstat/dba.epp");
		boot_engine.patch("src/jrd/fun.epp", "\"ib_util\"", "\"" + ib_util.getOutputFile().filename().u8string() + "\"");
	}

	auto &boot_isql = boot.addExecutable("isql");
	{
		setup(boot_isql);
		boot_isql += "src/isql/.*\\.cpp"_rr;
		boot_isql += "src/isql/.*\\.h"_rr;
		boot_isql += boot_yvalve;
		for (auto f : {"src/isql/extract.epp","src/isql/isql.epp","src/isql/show.epp"})
			gpre_boot_cmd(boot_isql, { "-lang_internal", "-n", "-ids", "-ocxx" }, f);
	}

	auto &boot_gfix = boot.addExecutable("gfix");
	{
		setup(boot_gfix);
		boot_gfix += "src/alice/main/aliceMain.cpp";
		boot_gfix += boot_yvalve, boot_alice;
	}

	auto &boot_gbak = boot.addExecutable("gbak");
	{
		setup(boot_gbak);
		boot_gbak += "src/burp/main/burpMain.cpp";
		boot_gbak += boot_yvalve, boot_burp;
	}

	auto &boot_gpre = boot.addExecutable("gpre");
	{
		setup(boot_gpre);
		boot_gpre += gpre_common, boot_yvalve;
		gpre_boot_cmd(boot_gpre, { "-n", "-m" }, "src/gpre/std/gpre_meta.epp");

		//boot_gpre.writeFileOnce(boot_gpre.BinaryDir / "firebird.conf",
			//"Providers = Remote," + boot_engine.getOutputFile().filename().u8string() + ",Loopback");

		// msg.fdb
		{
			auto &t = boot_gpre;
			SW_MAKE_CUSTOM_COMMAND_AND_ADD(CommandSequence, c, t);
			c->addInput(boot_isql.getOutputFile());
			c->addInput(boot_gfix.getOutputFile());
			c->addOutput(t.BinaryDir / "msg.fdb");
			t += "msg.fdb";

			auto c1 = c->addCommand<sw::ExecuteBuiltinCommand>("sw_remove_file", nullptr);
			c1->args.push_back((t.BinaryDir / "msg.fdb").u8string());

			auto c2 = c->addCommand<sw::ExecuteBuiltinCommand>("sw_remove_file", nullptr);
			c2->args.push_back((t.BinaryDir / "msg_.fdb").u8string());

			auto c3 = c->addCommand();
			//c3->working_directory = t.BinaryDir;
			c3->args.push_back(boot_isql.getOutputFile().u8string());
			c3->args.push_back("-q");
			c3->args.push_back("-i");
			c3->args.push_back(t.BinaryDir / "create_db_msg.sql");
			t.writeFileOnce("create_db_msg.sql", "create database '" + (t.BinaryDir / "msg_.fdb").u8string() + "';");

			auto c4 = c->addCommand();
			c4->args.push_back(boot_gfix.getOutputFile().u8string());
			c4->args.push_back("-write");
			c4->args.push_back("async");
			c4->args.push_back(t.BinaryDir / "msg_.fdb");

			auto add_message = [&c, &t, &boot_isql](const auto &p)
			{
				auto c4 = c->addCommand();
				c4->args.push_back(boot_isql.getOutputFile());
				c4->args.push_back("-q");
				c4->args.push_back(t.BinaryDir / "msg_.fdb");
				c4->args.push_back("-i");
				c4->args.push_back(t.SourceDir / "src" / "msgs" / p);
				c->addInput(t.SourceDir / "src" / "msgs" / p);
				t += t.SourceDir / "src" / "msgs" / p;
			};

			add_message("msg.sql");
			add_message("facilities2.sql");
			add_message("sqlstates.sql");
			add_message("locales.sql");
			add_message("history2.sql");
			add_message("messages2.sql");
			add_message("symbols2.sql");
			add_message("system_errors2.sql");
			add_message("transmsgs.fr_FR2.sql");
			add_message("transmsgs.de_DE2.sql");

			auto c5 = c->addCommand<sw::ExecuteBuiltinCommand>("sw_copy_file", nullptr);
			c5->args.push_back((t.BinaryDir / "msg_.fdb").u8string());
			c5->args.push_back((t.BinaryDir / "msg.fdb").u8string());
		}

		// security.fdb
		{
			auto &t = boot_gpre;
			SW_MAKE_CUSTOM_COMMAND_AND_ADD(CommandSequence, c, t);
			c->addInput(boot_isql.getOutputFile());
			c->addInput(boot_gfix.getOutputFile());
			c->addOutput(t.BinaryDir / "security.fdb");
			t += "security.fdb";

			auto c1 = c->addCommand<sw::ExecuteBuiltinCommand>("sw_remove_file", nullptr);
			c1->args.push_back((t.BinaryDir / "security.fdb").u8string());

			auto c2 = c->addCommand<sw::ExecuteBuiltinCommand>("sw_remove_file", nullptr);
			c2->args.push_back((t.BinaryDir / "security_.fdb").u8string());

			auto c3 = c->addCommand();
			//c3->working_directory = t.BinaryDir;
			c3->args.push_back(boot_isql.getOutputFile().u8string());
			c3->args.push_back("-q");
			c3->args.push_back("-i");
			c3->args.push_back(t.BinaryDir / "create_db_security.sql");
			t.writeFileOnce("create_db_security.sql", "create database '" + (t.BinaryDir / "security_.fdb").u8string() + "';");

			auto c4 = c->addCommand();
			c4->args.push_back(boot_gfix.getOutputFile().u8string());
			c4->args.push_back("-write");
			c4->args.push_back("async");
			c4->args.push_back(t.BinaryDir / "security_.fdb");

			auto add_message = [&c, &t, &boot_isql](const auto &p)
			{
				auto c4 = c->addCommand();
				c4->args.push_back(boot_isql.getOutputFile());
				c4->args.push_back("-q");
				c4->args.push_back(t.BinaryDir / "security_.fdb");
				c4->args.push_back("-i");
				c4->args.push_back(t.SourceDir / "src" / p);
				c->addInput(t.SourceDir / "src" / p);
				t += t.SourceDir / "src" / p;
			};

			add_message("dbs/security.sql");

			{
				auto c4 = c->addCommand();
				c4->args.push_back(boot_gfix.getOutputFile().u8string());
				c4->args.push_back("-write");
				c4->args.push_back("sync");
				c4->args.push_back(t.BinaryDir / "security_.fdb");
			}

			auto c5 = c->addCommand<sw::ExecuteBuiltinCommand>("sw_copy_file", nullptr);
			c5->args.push_back((t.BinaryDir / "security_.fdb").u8string());
			c5->args.push_back((t.BinaryDir / "security.fdb").u8string());
		}

		// metadata.fdb
		{
			auto &t = boot_gpre;
			SW_MAKE_CUSTOM_COMMAND_AND_ADD(CommandSequence, c, t);
			c->addInput(boot_gbak.getOutputFile());
			c->addOutput(t.BinaryDir / "metadata.fdb");
			t += "metadata.fdb";

			auto c1 = c->addCommand<sw::ExecuteBuiltinCommand>("sw_remove_file", nullptr);
			c1->args.push_back((t.BinaryDir / "metadata.fdb").u8string());

			auto c2 = c->addCommand<sw::ExecuteBuiltinCommand>("sw_remove_file", nullptr);
			c2->args.push_back((t.BinaryDir / "metadata_.fdb").u8string());

			auto c3 = c->addCommand();
			//c3->working_directory = t.BinaryDir;
			c3->args.push_back(boot_gbak.getOutputFile().u8string());
			c3->args.push_back("-r");
			c3->args.push_back(t.SourceDir / "builds/misc/metadata.gbak");
			c3->args.push_back(t.BinaryDir / "metadata_.fdb");
			t += t.SourceDir / "builds/misc/metadata.gbak";

			auto c5 = c->addCommand<sw::ExecuteBuiltinCommand>("sw_copy_file", nullptr);
			c5->args.push_back((t.BinaryDir / "metadata_.fdb").u8string());
			c5->args.push_back((t.BinaryDir / "metadata.fdb").u8string());
		}

		// help.fdb
		{
			auto &t = boot_gpre;
			SW_MAKE_CUSTOM_COMMAND_AND_ADD(CommandSequence, c, t);
			c->addInput(boot_gbak.getOutputFile());
			c->addOutput(t.BinaryDir / "help.fdb");
			t += "help.fdb";

			auto c1 = c->addCommand<sw::ExecuteBuiltinCommand>("sw_remove_file", nullptr);
			c1->args.push_back((t.BinaryDir / "help.fdb").u8string());

			auto c2 = c->addCommand<sw::ExecuteBuiltinCommand>("sw_remove_file", nullptr);
			c2->args.push_back((t.BinaryDir / "help_.fdb").u8string());

			auto c3 = c->addCommand();
			//c3->working_directory = t.BinaryDir;
			c3->args.push_back(boot_gbak.getOutputFile().u8string());
			c3->args.push_back("-r");
			c3->args.push_back(t.SourceDir / "builds/misc/help.gbak");
			c3->args.push_back(t.BinaryDir / "help_.fdb");
			t += t.SourceDir / "builds/misc/help.gbak";

			auto c5 = c->addCommand<sw::ExecuteBuiltinCommand>("sw_copy_file", nullptr);
			c5->args.push_back((t.BinaryDir / "help_.fdb").u8string());
			c5->args.push_back((t.BinaryDir / "help.fdb").u8string());
		}
	}

	//
	auto gpre_master_cmd = [&boot_gpre](auto &t, const Strings &args, const path &in)
	{
		auto dir = t.BinaryDir / in.filename() += ".master";
		auto out = dir / in.filename() += ".master.cpp";

		SW_MAKE_CUSTOM_COMMAND_AND_ADD(CommandSequence, c, t);
		c->addInput(boot_gpre.getOutputFile());
		c->addInput(t.SourceDir / in);
		c->addOutput(out);
		(t + boot_gpre)->Dummy = true;

		auto copy_file = [&c, &boot_gpre, &dir](const auto &in, const auto &out)
		{
			auto c5 = c->addCommand<sw::ExecuteBuiltinCommand>("sw_copy_file", nullptr);
			c5->args.push_back(boot_gpre.BinaryDir / in);
			c5->args.push_back(dir / out);
		};

		copy_file("metadata.fdb", "yachts.lnk");
		copy_file("security.fdb", "security.fdb");
		copy_file("msg.fdb", "msg.fdb");
		copy_file("help.fdb", "help.fdb");

		auto c3 = c->addCommand();
		//c3->working_directory = t.BinaryDir;
		c3->args.push_back(boot_gpre.getOutputFile());
		for (auto &a : args)
			c3->args.push_back(a);
		c3->args.push_back("-b");
		c3->args.push_back(dir.u8string() + "/");
		c3->args.push_back(t.SourceDir / in);
		c3->args.push_back(out);
		t += in;
		t += out;

		return c;
	};

	auto &alice = fb.addStaticLibrary("alice");
	{
		setup(alice);
		alice += "src/alice/.*"_r;
		gpre_master_cmd(alice, { "-n", "-m" }, "src/alice/alice_meta.epp");
	}

	auto &burp = fb.addStaticLibrary("burp");
	{
		setup(burp);
		burp += "src/burp/.*"_r;
		burp += "src/burp"_idir;
		for (auto f : {"src/burp/backup.epp","src/burp/OdsDetection.epp","src/burp/restore.epp"})
			gpre_master_cmd(burp, { "-n", "-m", "-ids", "-ocxx", "-m" }, f);
		burp.Public += common; // for icu and idirs
	}

	auto &yvalve = add_yvalve(fb, gpre_master_cmd);

	auto &engine = fb.addLibrary("engine");
	{
		setup(engine);
		engine.setOutputDir("plugins");
		engine += "builds/win32/defs/plugin.def";
		engine += engine_common, alice, burp, yvalve;
		(engine + ib_util)->Dummy = true;

		engine += "src/jrd/jrd.cpp";
		engine.patch("src/jrd/jrd.cpp", "CURRENT_ENGINE", "SW_CURRENT_ENGINE");
		engine += Definition("SW_CURRENT_ENGINE=\"" + engine.getOutputFile().filename().u8string() + "\"");

		for (auto f : {
			"src/dsql/DdlNodes.epp",
			"src/dsql/metd.epp",
			"src/dsql/PackageNodes.epp",
			"src/jrd/dfw.epp",
			"src/jrd/dpm.epp",
			"src/jrd/dyn_util.epp",
			"src/jrd/fun.epp",
			"src/jrd/Function.epp",
			"src/jrd/grant.epp",
			"src/jrd/ini.epp",
			"src/jrd/met.epp",
			"src/jrd/scl.epp",
			})
		{
			gpre_master_cmd(engine, { "-n", "-ids", "-gds_cxx" }, f);
		}
		gpre_master_cmd(engine, { "-n", "-m" }, "src/utilities/gstat/dba.epp");
		engine.patch("src/jrd/fun.epp", "\"ib_util\"", "\"" + ib_util.getOutputFile().filename().u8string() + "\"");
	}

	auto &intl = fb.addLibrary("fbintl");
	{
		setup(intl);
		intl.setOutputDir("intl");
		intl +=
			"src/intl/.*\\.cpp"_r,
			"src/intl/.*\\.h"_r;
		intl += yvalve;
	}

	auto &legacy_usermanager = fb.addLibrary("Legacy_UserManager");
	{
		setup(legacy_usermanager);
		legacy_usermanager.setOutputDir("plugins");
		legacy_usermanager += "builds/win32/defs/plugin.def";
		gpre_master_cmd(legacy_usermanager, { "-n", "-m" }, "src/auth/SecurityDatabase/LegacyManagement.epp");
		legacy_usermanager += yvalve;
	}

	auto &legacy_auth = fb.addLibrary("Legacy_Auth");
	{
		setup(legacy_auth);
		legacy_auth.setOutputDir("plugins");
		legacy_auth += "builds/win32/defs/plugin.def";
		legacy_auth += "src/auth/SecurityDatabase/LegacyServer.cpp";
		legacy_auth += yvalve;
	}

	auto &udr_engine = fb.addLibrary("udr_engine");
	{
		setup(udr_engine);
		udr_engine.setOutputDir("plugins");
		udr_engine += "builds/win32/defs/plugin.def";
		udr_engine += "src/plugins/udr_engine/UdrEngine.cpp";
		udr_engine += yvalve;
	}

	auto &srp = fb.addLibrary("Srp");
	{
		setup(srp);
		srp.setOutputDir("plugins");
		srp += "builds/win32/defs/plugin.def";
		srp += "src/auth/SecureRemotePassword/manage/SrpManagement.cpp";
		srp += yvalve;
	}

	auto &fbrmclib = fb.addLibrary("fbrmclib");
	{
		setup(fbrmclib);
		fbrmclib += "builds/win32/defs/fbrmclib.def";
		fbrmclib += "src/gpre/languages/fbrmclib.cpp";
		fbrmclib += "src/include/gen"_id;
		fbrmclib += yvalve;
	}

	// executables

	auto &gpre = fb.addExecutable("gpre");
	{
		setup(gpre);
		gpre_master_cmd(gpre, { "-n", "-m" }, "src/gpre/std/gpre_meta.epp");
		gpre += gpre_common, yvalve;
	}

	auto &build_msg = fb.addExecutable("build_msg");
	{
		setup(build_msg);
		gpre_master_cmd(build_msg, { "-n", "-m" }, "src/msgs/build_file.epp");
		build_msg += boot_yvalve;
	}

	auto &codes = fb.addExecutable("codes");
	{
		setup(codes);
		gpre_master_cmd(codes, { "-n", "-m" }, "src/misc/codes.epp");
		codes += boot_yvalve;
	}

	auto &gstat = fb.addExecutable("gstat");
	{
		setup(gstat);
		gstat +=
			"src/jrd/btn.cpp",
			"src/jrd/ods.cpp",
			"src/utilities/gstat/main/gstatMain.cpp",
			"src/utilities/gstat/ppg.cpp"
			;
		gpre_master_cmd(gstat, { "-n", "-m" }, "src/utilities/gstat/dba.epp");
		gstat += yvalve;
	}

	auto &fb_lock_print = fb.addExecutable("fb_lock_print");
	{
		setup(fb_lock_print);
		fb_lock_print += "src/lock/print.cpp";
		fb_lock_print += yvalve;
	}

	auto &fbguard = fb.addExecutable("fbguard");
	{
		setup(fbguard);
		if (common.getSettings().TargetOS.is(OSType::Windows))
		{
			fbguard +=
				"src/iscguard/cntl_guard.cpp",
				"src/iscguard/iscguard.cpp",
				"src/iscguard/iscguard.rc",
				"src/remote/server/os/win32/chop.cpp"
				;
			fbguard += "comctl32.lib"_slib;
			fbguard += "Gdi32.lib"_slib;
			fbguard += "version.lib"_slib;
			if (auto L = fbguard.getSelectedTool()->as<VisualStudioLinker>(); L)
				L->Subsystem = vs::Subsystem::Windows;
		}
		else
		{
			fbguard +=
				"src/utilities/guard/guard.cpp",
				"src/utilities/guard/util.cpp"
				;
		}
		fbguard += yvalve;
	}

	auto &fbtracemgr = fb.addExecutable("fbtracemgr");
	{
		setup(fbtracemgr);
		fbtracemgr +=
			"src/jrd/trace/TraceCmdLine.cpp",
			"src/utilities/fbtracemgr/traceMgrMain.cpp"
			;
		fbtracemgr += yvalve;
	}

	auto &gfix = fb.addExecutable("gfix");
	{
		setup(gfix);
		gfix += "src/alice/main/aliceMain.cpp";
		gfix += alice, yvalve;
	}

	auto &gbak = fb.addExecutable("gbak");
	{
		setup(gbak);
		gbak += "src/burp/main/burpMain.cpp";
		gbak += burp, yvalve;
	}

	auto &gsplit = fb.addExecutable("gsplit");
	{
		setup(gsplit);
		gsplit += "src/burp/split/spit.[hc]p?p?"_rr;
		gsplit += burp, yvalve;
	}

	auto &isql = fb.addExecutable("isql");
	{
		setup(isql);
		isql += "src/isql/.*\\.cpp"_rr;
		isql += "src/isql/.*\\.h"_rr;
		isql += yvalve;
		for (auto f : {"src/isql/extract.epp","src/isql/isql.epp","src/isql/show.epp"})
			gpre_master_cmd(isql, { "-n", "-ids", "-ocxx" }, f);
	}

	auto &qli = fb.addExecutable("qli");
	{
		setup(qli);
		qli += "src/qli/.*\\.cpp"_rr;
		qli += "src/qli/.*\\.h"_rr;
		qli += yvalve;
		for (auto f : {"src/qli/help.epp","src/qli/meta.epp","src/qli/proc.epp","src/qli/show.epp"})
			gpre_master_cmd(qli, { "-n", "-m" }, f);
	}

	auto &fbserver = fb.addExecutable("firebird");
	{
		setup(fbserver);
		fbserver +=
			"src/auth/SecureRemotePassword/server/SrpServer.cpp",
			"src/auth/SecureRemotePassword/server/SrpServer.h"
			;
		fbserver += "src/remote/server"_id;
		if (common.getSettings().TargetOS.is(OSType::Windows))
		{
			fbserver +=
				"src/remote/server/os/win32/chop.cpp",
				"src/remote/server/os/win32/cntl.cpp",
				"src/remote/server/os/win32/property.cpp",
				"src/remote/server/os/win32/window.cpp",
				"src/remote/server/os/win32/srvr_w32.cpp",
				"src/remote/server/os/win32/server.ico",
				"src/remote/server/os/win32/caution.ico",
				"src/remote/server/os/win32/window.rc",
				"src/remote/server/ReplServer.cpp",
				"src/remote/server/server.cpp"
				;
			fbserver += "Gdi32.lib"_slib;
			fbserver += "Comctl32.lib"_slib;
			if (auto L = fbserver.getSelectedTool()->as<VisualStudioLinker>(); L)
				L->Subsystem = vs::Subsystem::Windows;
		}
		else
		{
			fbserver +=
				"src/auth/SecureRemotePassword/srp.cpp",
				"src/remote/server/os/posix/inet_server.cpp",
				"src/remote/server/server.cpp",

				"src/remote/inet.cpp",
				"src/remote/merge.cpp",
				"src/remote/parser.cpp",
				"src/remote/protocol.cpp",
				"src/remote/remote.cpp"
				;
		}
		fbserver += yvalve, engine_common;
	}

	// TODO: src/utilities/CMakeLists.txt
}
