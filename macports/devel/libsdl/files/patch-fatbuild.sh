--- build-scripts/fatbuild.sh	2008-09-02 13:32:35.000000000 +0200
+++ build-scripts/fatbuild-fixed2.sh	2008-09-04 03:10:54.000000000 +0200
@@ -17,58 +17,32 @@ if [ ! -d $SDK_PATH ]; then
     exit 1
 fi
 
-# See if we can use 10.2 or 10.3 runtime compatibility
-if [ -d "$SDK_PATH/MacOSX10.2.8.sdk" ]; then
-    # PowerPC configure flags (10.2 runtime compatibility)
-    # We dynamically load X11, so using the system X11 headers is fine.
-    CONFIG_PPC="--build=`uname -p`-apple-darwin --host=powerpc-apple-darwin \
---x-includes=/usr/X11R6/include --x-libraries=/usr/X11R6/lib"
-
-    # PowerPC compiler flags
-    CC_PPC="gcc-3.3 -arch ppc"
-    CXX_PPC="g++-3.3 -arch ppc"
-    CFLAGS_PPC=""
-    CPPFLAGS_PPC="-DMAC_OS_X_VERSION_MIN_REQUIRED=1020 \
--nostdinc \
--F$SDK_PATH/MacOSX10.2.8.sdk/System/Library/Frameworks \
--I$SDK_PATH/MacOSX10.2.8.sdk/usr/include/gcc/darwin/3.3 \
--isystem $SDK_PATH/MacOSX10.2.8.sdk/usr/include"
-
-    # PowerPC linker flags 
-    LFLAGS_PPC="-arch ppc \
--L$SDK_PATH/MacOSX10.2.8.sdk/usr/lib/gcc/darwin/3.3 \
--F$SDK_PATH/MacOSX10.2.8.sdk/System/Library/Frameworks \
--Wl,-syslibroot,$SDK_PATH/MacOSX10.2.8.sdk"
-
-else # 10.2 or 10.3 SDK
-
-    # PowerPC configure flags (10.3 runtime compatibility)
-    # We dynamically load X11, so using the system X11 headers is fine.
-    CONFIG_PPC="--build=`uname -p`-apple-darwin --host=powerpc-apple-darwin \
---x-includes=/usr/X11R6/include --x-libraries=/usr/X11R6/lib"
-
-    # PowerPC compiler flags
-    CC_PPC="gcc-4.0 -arch ppc"
-    CXX_PPC="g++-4.0 -arch ppc"
-    CFLAGS_PPC=""
-    CPPFLAGS_PPC="-DMAC_OS_X_VERSION_MIN_REQUIRED=1030 \
+if test x$prefix = x; then
+    prefix=/usr/local
+fi
+
+# PowerPC configure flags (10.4 runtime compatibility)
+CONFIG_PPC="--build=`uname -p`-apple-darwin --host=powerpc-apple-darwin \
+--x-includes=/usr/X11R6/include --x-libraries=/usr/X11R6/lib --prefix=$prefix"
+
+# PowerPC compiler flags
+CC_PPC="gcc-4.0 -arch ppc"
+CXX_PPC="g++-4.0 -arch ppc"
+CFLAGS_PPC="-mmacosx-version-min=10.4"
+CPPFLAGS_PPC="-DMAC_OS_X_VERSION_MIN_REQUIRED=1040 \
 -nostdinc \
--F$SDK_PATH/MacOSX10.3.9.sdk/System/Library/Frameworks \
--I$SDK_PATH/MacOSX10.3.9.sdk/usr/lib/gcc/powerpc-apple-darwin9/4.0.1/include \
--isystem $SDK_PATH/MacOSX10.3.9.sdk/usr/include"
-
-    # PowerPC linker flags
-    LFLAGS_PPC="-arch ppc -mmacosx-version-min=10.3 \
--L$SDK_PATH/MacOSX10.3.9.sdk/usr/lib/gcc/powerpc-apple-darwin9/4.0.1 \
--F$SDK_PATH/MacOSX10.3.9.sdk/System/Library/Frameworks \
--Wl,-syslibroot,$SDK_PATH/MacOSX10.3.9.sdk"
+-F$SDK_PATH/MacOSX10.4u.sdk/System/Library/Frameworks \
+-I$SDK_PATH/MacOSX10.4u.sdk/usr/lib/gcc/powerpc-apple-darwin8/4.0.1/include \
+-isystem $SDK_PATH/MacOSX10.4u.sdk/usr/include"
 
-fi # 10.2 or 10.3 SDK
+# PowerPC linker flags
+LFLAGS_PPC="-arch ppc -mmacosx-version-min=10.4 \
+-Wl,-syslibroot,$SDK_PATH/MacOSX10.4u.sdk"
 
 # Intel configure flags (10.4 runtime compatibility)
 # We dynamically load X11, so using the system X11 headers is fine.
 CONFIG_X86="--build=`uname -p`-apple-darwin --host=i386-apple-darwin \
---x-includes=/usr/X11R6/include --x-libraries=/usr/X11R6/lib"
+--x-includes=/usr/X11R6/include --x-libraries=/usr/X11R6/lib --prefix=$prefix"
 
 # Intel compiler flags
 CC_X86="gcc-4.0 -arch i386"
@@ -77,12 +51,11 @@ CFLAGS_X86="-mmacosx-version-min=10.4"
 CPPFLAGS_X86="-DMAC_OS_X_VERSION_MIN_REQUIRED=1040 \
 -nostdinc \
 -F$SDK_PATH/MacOSX10.4u.sdk/System/Library/Frameworks \
--I$SDK_PATH/MacOSX10.4u.sdk/usr/lib/gcc/i686-apple-darwin9/4.0.1/include \
+-I$SDK_PATH/MacOSX10.4u.sdk/usr/lib/gcc/i686-apple-darwin8/4.0.1/include \
 -isystem $SDK_PATH/MacOSX10.4u.sdk/usr/include"
 
 # Intel linker flags
 LFLAGS_X86="-arch i386 -mmacosx-version-min=10.4 \
--L$SDK_PATH/MacOSX10.4u.sdk/usr/lib/gcc/i686-apple-darwin9/4.0.1 \
 -Wl,-syslibroot,$SDK_PATH/MacOSX10.4u.sdk"
 
 #
@@ -248,9 +221,6 @@ do_install()
     echo $*
     $* || exit 5
 }
-if test x$prefix = x; then
-    prefix=/usr/local
-fi
 if test x$exec_prefix = x; then
     exec_prefix=$prefix
 fi
@@ -267,35 +237,35 @@ if test x$datadir = x; then
     datadir=$prefix/share
 fi
 if test x$mandir = x; then
-    mandir=$prefix/man
+    mandir=$datadir/man
 fi
 if test x$install_bin = xyes; then
-    do_install sh $auxdir/mkinstalldirs $bindir
-    do_install /usr/bin/install -c -m 755 build/$native_path/sdl-config $bindir/sdl-config
+    do_install sh $auxdir/mkinstalldirs $DESTDIR$bindir
+    do_install /usr/bin/install -c -m 755 build/$native_path/sdl-config $DESTDIR$bindir/sdl-config
 fi
 if test x$install_hdrs = xyes; then
-    do_install sh $auxdir/mkinstalldirs $includedir/SDL
+    do_install sh $auxdir/mkinstalldirs $DESTDIR$includedir/SDL
     for src in $srcdir/include/*.h; do \
         file=`echo $src | sed -e 's|^.*/||'`; \
-        do_install /usr/bin/install -c -m 644 $src $includedir/SDL/$file; \
+        do_install /usr/bin/install -c -m 644 $src $DESTDIR$includedir/SDL/$file; \
     done
-    do_install /usr/bin/install -c -m 644 $srcdir/include/SDL_config_macosx.h $includedir/SDL/SDL_config.h
+    do_install /usr/bin/install -c -m 644 $srcdir/include/SDL_config_macosx.h $DESTDIR$includedir/SDL/SDL_config.h
 fi
 if test x$install_lib = xyes; then
-    do_install sh $auxdir/mkinstalldirs $libdir
-    do_install sh build/$native_path/libtool --mode=install /usr/bin/install -c  build/libSDL.la $libdir/libSDL.la
-    do_install /usr/bin/install -c -m 644 build/libSDLmain.a $libdir/libSDLmain.a
-    do_install ranlib $libdir/libSDLmain.a
+    do_install sh $auxdir/mkinstalldirs $DESTDIR$libdir
+    do_install sh build/$native_path/libtool --mode=install /usr/bin/install -c  build/libSDL.la $DESTDIR$libdir/libSDL.la
+    do_install /usr/bin/install -c -m 644 build/libSDLmain.a $DESTDIR$libdir/libSDLmain.a
+    do_install ranlib $DESTDIR$libdir/libSDLmain.a
 fi
 if test x$install_data = xyes; then
-    do_install sh $auxdir/mkinstalldirs $datadir/aclocal
-    do_install /usr/bin/install -c -m 644 $srcdir/sdl.m4 $datadir/aclocal/sdl.m4
+    do_install sh $auxdir/mkinstalldirs $DESTDIR$datadir/aclocal
+    do_install /usr/bin/install -c -m 644 $srcdir/sdl.m4 $DESTDIR$datadir/aclocal/sdl.m4
 fi
 if test x$install_man = xyes; then
-    do_install sh $auxdir/mkinstalldirs $mandir/man3
+    do_install sh $auxdir/mkinstalldirs $DESTDIR$mandir/man3
     for src in $srcdir/docs/man3/*.3; do \
         file=`echo $src | sed -e 's|^.*/||'`; \
-        do_install /usr/bin/install -c -m 644 $src $mandir/man3/$file; \
+        do_install /usr/bin/install -c -m 644 $src $DESTDIR$mandir/man3/$file; \
     done
 fi
 
