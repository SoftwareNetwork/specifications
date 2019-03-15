void build(Solution &s)
{
    auto &uv = s.addTarget<LibraryTarget>("libuv", "1.26.0");
    uv += Git("https://github.com/libuv/libuv", "v{v}");

    uv.Private << sw::Shared << "BUILDING_UV_SHARED"_d;
    uv.Interface << sw::Shared << "USING_UV_SHARED"_d;

    uv += "include/.*"_rr;
    uv -= "src/.*"_rr;
    uv += "src/.*"_r;
    if (s.Settings.TargetOS.Type == OSType::Windows)
    {
        uv += "src/win/.*"_rr;
        uv.Public +=
            "ws2_32.lib"_slib,
            "user32.lib"_slib,
            "Advapi32.lib"_slib,
            "iphlpapi.lib"_slib,
            "psapi.lib"_slib,
            "userenv.lib"_slib;
    }
    else
    {
        uv -= "src/win/.*"_rr;
        uv +=
            "src/unix/async.c",
            "src/unix/atomic-ops.h",
            "src/unix/core.c",
            "src/unix/dl.c",
            "src/unix/fs.c",
            "src/unix/getaddrinfo.c",
            "src/unix/getnameinfo.c",
            "src/unix/internal.h",
            "src/unix/loop-watcher.c",
            "src/unix/loop.c",
            "src/unix/pipe.c",
            "src/unix/poll.c",
            "src/unix/process.c",
            "src/unix/signal.c",
            "src/unix/spinlock.h",
            "src/unix/stream.c",
            "src/unix/tcp.c",
            "src/unix/thread.c",
            "src/unix/tty.c",
            "src/unix/udp.c";

        switch (s.Settings.TargetOS.Type)
        {
        case OSType::AIX:
            break;
        case OSType::Android:
            break;
        case OSType::Macos:
            uv +=
                "src/unix/darwin.c",
                "src/unix/darwin-proctitle.c",
                "src/unix/fsevents.c",
                "src/unix/kqueue.c",
                "src/unix/proctitle.c";
            break;
        case OSType::FreeBSD:
            break;
        case OSType::NetBSD:
            break;
        case OSType::OpenBSD:
            break;
        case OSType::SunOS:
            break;
        case OSType::Linux:
            uv +=
                "src/unix/linux-core.c",
                "src/unix/linux-inotify.c",
                "src/unix/linux-syscalls.c",
                "src/unix/linux-syscalls.h",
                "src/unix/proctitle.c";
            break;
        }
    }

    // windows patches that add extended startup info and attribute lists

    uv.patch("include/uv.h", R"(--- bef5/include/uv.h	2019-01-14 19:34:30.586129300 +0300
+++ bef51/include/uv.h	2019-01-14 04:33:15.209648100 +0300
@@ -931,6 +931,12 @@
    */
   uv_uid_t uid;
   uv_gid_t gid;
+  /*
+   * Represents a set of attributes that passed with STARTUPINFOEXW
+   * on Windows. This is not supported on unix; uv_spawn() will fail
+   * and set the error to UV_ENOTSUP.
+   */
+  void *attribute_list;
 } uv_process_options_t;
 
 /*
@@ -979,7 +985,11 @@
    * option is only meaningful on Windows systems. On Unix it is silently
    * ignored.
    */
-  UV_PROCESS_WINDOWS_HIDE_GUI = (1 << 6)
+  UV_PROCESS_WINDOWS_HIDE_GUI = (1 << 6),
+  /*
+   * Create new console.
+   */
+  UV_PROCESS_WINDOWS_ALLOC_CONSOLE = (1 << 7),
 };
 
 /*)");
 
    uv.patch("src/unix/process.c", R"(--- bef5/src/unix/process.c	2019-01-14 19:34:30.561194200 +0300
+++ bef51/src/unix/process.c	2019-01-14 02:04:56.462853100 +0300
@@ -426,6 +426,10 @@
   int i;
   int status;
 
+  if (options->attribute_list != NULL) {
+      return UV_ENOTSUP;
+  }
+
   assert(options->file != NULL);
   assert(!(options->flags & ~(UV_PROCESS_DETACHED |
                               UV_PROCESS_SETGID |)");
 
    uv.patch("src/win/process.c", R"(--- bef5/src/win/process.c	2019-01-14 19:34:30.683714200 +0300
+++ bef51/src/win/process.c	2019-01-14 04:54:15.061879400 +0300
@@ -943,7 +943,7 @@
   BOOL result;
   WCHAR* application_path = NULL, *application = NULL, *arguments = NULL,
          *env = NULL, *cwd = NULL;
-  STARTUPINFOW startup;
+  STARTUPINFOEXW startup;
   PROCESS_INFORMATION info;
   DWORD process_flags;
 
@@ -965,6 +965,7 @@
                               UV_PROCESS_SETUID |
                               UV_PROCESS_WINDOWS_HIDE |
                               UV_PROCESS_WINDOWS_HIDE_CONSOLE |
+                              UV_PROCESS_WINDOWS_ALLOC_CONSOLE |
                               UV_PROCESS_WINDOWS_HIDE_GUI |
                               UV_PROCESS_WINDOWS_VERBATIM_ARGUMENTS)));
 
@@ -1052,20 +1053,58 @@
     goto done;
   }
 
-  startup.cb = sizeof(startup);
-  startup.lpReserved = NULL;
-  startup.lpDesktop = NULL;
-  startup.lpTitle = NULL;
-  startup.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
-
-  startup.cbReserved2 = uv__stdio_size(process->child_stdio_buffer);
-  startup.lpReserved2 = (BYTE*) process->child_stdio_buffer;
-
-  startup.hStdInput = uv__stdio_handle(process->child_stdio_buffer, 0);
-  startup.hStdOutput = uv__stdio_handle(process->child_stdio_buffer, 1);
-  startup.hStdError = uv__stdio_handle(process->child_stdio_buffer, 2);
+  startup.StartupInfo.cb = sizeof(startup);
+  startup.StartupInfo.lpReserved = NULL;
+  startup.StartupInfo.lpDesktop = NULL;
+  startup.StartupInfo.lpTitle = NULL;
+  startup.StartupInfo.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
+
+  startup.StartupInfo.cbReserved2 = uv__stdio_size(process->child_stdio_buffer);
+  startup.StartupInfo.lpReserved2 = (BYTE*) process->child_stdio_buffer;
+
+  startup.StartupInfo.hStdInput = uv__stdio_handle(process->child_stdio_buffer, 0);
+  startup.StartupInfo.hStdOutput = uv__stdio_handle(process->child_stdio_buffer, 1);
+  startup.StartupInfo.hStdError = uv__stdio_handle(process->child_stdio_buffer, 2);
+
+  const auto cHandlesToInherit = 3;
+  HANDLE rgHandlesToInherit[] = {
+      startup.StartupInfo.hStdInput,
+      startup.StartupInfo.hStdOutput,
+      startup.StartupInfo.hStdError,
+  };
+  SIZE_T size = 0;
+  LPPROC_THREAD_ATTRIBUTE_LIST lpAttributeList = (LPPROC_THREAD_ATTRIBUTE_LIST)options->attribute_list;
+  BOOL fSuccess = TRUE;
+  BOOL fInitialized = FALSE;
+  if (!(options->flags & UV_PROCESS_WINDOWS_ALLOC_CONSOLE)) {
+    if (!lpAttributeList) {
+        fSuccess = InitializeProcThreadAttributeList(NULL, 1, 0, &size) ||
+        GetLastError() == ERROR_INSUFFICIENT_BUFFER;
+        if (fSuccess) {
+            lpAttributeList = (LPPROC_THREAD_ATTRIBUTE_LIST)
+                (HeapAlloc(GetProcessHeap(), 0, size));
+            fSuccess = lpAttributeList != NULL;
+        }
+        if (fSuccess) {
+            fSuccess = InitializeProcThreadAttributeList(lpAttributeList,
+                1, 0, &size);
+        }
+    }
+    if (fSuccess) {
+        fInitialized = TRUE;
+        fSuccess = UpdateProcThreadAttribute(lpAttributeList,
+            0, PROC_THREAD_ATTRIBUTE_HANDLE_LIST,
+            rgHandlesToInherit,
+            cHandlesToInherit * sizeof(HANDLE), NULL, NULL);
+        if (!fSuccess) {
+            err = GetLastError();
+            goto done;
+        }
+    }
+  }
+  startup.lpAttributeList = lpAttributeList;
 
-  process_flags = CREATE_UNICODE_ENVIRONMENT;
+  process_flags = CREATE_UNICODE_ENVIRONMENT | EXTENDED_STARTUPINFO_PRESENT;
 
   if ((options->flags & UV_PROCESS_WINDOWS_HIDE_CONSOLE) ||
       (options->flags & UV_PROCESS_WINDOWS_HIDE)) {
@@ -1080,12 +1119,17 @@
   if ((options->flags & UV_PROCESS_WINDOWS_HIDE_GUI) ||
       (options->flags & UV_PROCESS_WINDOWS_HIDE)) {
     /* Use SW_HIDE to avoid any potential process window. */
-    startup.wShowWindow = SW_HIDE;
+    startup.StartupInfo.wShowWindow = SW_HIDE;
   } else {
-    startup.wShowWindow = SW_SHOWDEFAULT;
+    startup.StartupInfo.wShowWindow = SW_SHOWDEFAULT;
   }
 
-  if (options->flags & UV_PROCESS_DETACHED) {
+  BOOL inheritHandles = TRUE;
+  if (options->flags & UV_PROCESS_WINDOWS_ALLOC_CONSOLE) {
+    process_flags |= CREATE_NEW_CONSOLE;
+    inheritHandles = FALSE;
+  }
+  else if (options->flags & UV_PROCESS_DETACHED) {
     /* Note that we're not setting the CREATE_BREAKAWAY_FROM_JOB flag. That
      * means that libuv might not let you create a fully daemonized process
      * when run under job control. However the type of job control that libuv
@@ -1103,11 +1147,11 @@
                      arguments,
                      NULL,
                      NULL,
-                     1,
+                     inheritHandles,
                      process_flags,
                      env,
                      cwd,
-                     &startup,
+                     (STARTUPINFOW*)&startup,
                      &info)) {
     /* CreateProcessW failed. */
     err = GetLastError();
@@ -1169,7 +1213,12 @@
   uv__handle_start(process);
 
   /* Cleanup, whether we succeeded or failed. */
- done:
+done:
+  if (!options->attribute_list) {
+      if (fInitialized) DeleteProcThreadAttributeList(lpAttributeList);
+      if (lpAttributeList) HeapFree(GetProcessHeap(), 0, lpAttributeList);
+  }
+
   uv__free(application);
   uv__free(application_path);
   uv__free(arguments);)");
 
}
