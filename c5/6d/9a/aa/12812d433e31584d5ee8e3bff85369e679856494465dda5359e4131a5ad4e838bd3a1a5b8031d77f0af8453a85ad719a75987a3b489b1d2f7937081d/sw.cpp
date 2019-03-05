void build(Solution &s)
{
    auto &t = s.addTarget<Library>("apache.log4cxx", "master");
	t += Git("https://github.com/apache/logging-log4cxx", "", "{v}");

	t.setRootDirectory("src/main");

	t += "include/.*"_rr;
	t += "cpp/.*"_rr;

	t -= "cpp/asyncappender_nonblocking.cpp";

	t += "LOG4CXX"_d;
	t.Public += sw::Static, "LOG4CXX_STATIC"_d;

	if (s.Settings.TargetOS.Type == OSType::Windows)
		t += "odbc32.lib"_slib;

	/*t.Variables["LOGCHAR_IS_UNICHAR"] = 0;
	t.Variables["LOGCHAR_IS_UTF8"] = 1;
	t.Variables["LOGCHAR_IS_WCHAR"] = 0;

	t.Variables["CHAR_API"] = 0;
	t.Variables["WCHAR_T_API"] = 0;
	t.Variables["UNICHAR_API"] = 0;
	t.Variables["CFSTRING_API"] = 0;*/

	//t.configureFile("include/log4cxx/log4cxx.h.in", "log4cxx/log4cxx.h");
	t.configureFile("include/log4cxx/log4cxx.hw", "log4cxx/log4cxx.h");

	//t.configureFile("include/log4cxx/private/log4cxx_private.h.in", "log4cxx/private/log4cxx_private.h");
	t.configureFile("include/log4cxx/private/log4cxx_private.hw", "log4cxx/private/log4cxx_private.h");

	t.Public += "org.sw.demo.apache.apr.util"_dep;
}
