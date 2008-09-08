CFLAGS="-I/opt/local/include -L/opt/local/lib -mmacosx-version-min=10.4 \
-isysroot /Developer/SDKs/MacOSX10.4u.sdk -arch i386 -arch ppc -force_cpusubtype_ALL" \
./configure --disable-dependency-tracking
