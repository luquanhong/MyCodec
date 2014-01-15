PREBUILT=/root/tools/android-ndk-r8b/toolchains/arm-linux-androideabi-4.4.3/prebuilt/linux-x86
PLATFORM=/root/tools/android-ndk-r8b/platforms/android-14/arch-arm

CFLAGS="-O3 -fPIC -DANDROID -DHAVE_SYS_UIO_H=1 -Dipv6mr_interface=ipv6mr_ifindex -fasm -Wno-psabi -fno-short-enums -fno-strict-aliasing -finline-limit=300 -mfloat-abi=softfp -mfpu=neon -marm -march=armv7-a -mtune=cortex-a8"

LDFLAGS="-Wl,-rpath-link=$PLATFORM/usr/lib -L$PLATFORM/usr/lib -nostdlib -lc -lm -ldl -llog"

./configure \
	--enable-memalign-hack \
	--disable-static \
	--enable-shared \
	--disable-debug \
	--disable-doc \
	--disable-ffmpeg \
	--disable-ffplay \
	--disable-ffprobe \
	--disable-ffserver \
	--disable-avdevice \
	--disable-avfilter \
	--disable-pthreads \
	--disable-encoders \
	--disable-decoders \
	--disable-hwaccels \
	--disable-muxers \
	--disable-demuxers \
	--disable-parsers \
	--disable-protocols \
	--disable-network \
	--disable-filters \
	--disable-bsfs \
	--disable-indevs \
	--disable-yasm \
	--disable-sse \
	--disable-amd3dnow \
	--disable-amd3dnowext \
	--enable-decoder=aac \
	--enable-decoder=h264 \
	--enable-decoder=h263 \
	--enable-decoder=mpeg4 \
	--enable-decoder=mpeg2video \
	--prefix=/media/e/work/ffmpeg/release/android_arm_trunk_r22628 \
	--arch=arm \
	--target-os=linux \
	--enable-cross-compile \
	--cross-prefix=$PREBUILT/bin/arm-linux-androideabi- \
	--cc=$PREBUILT/bin/arm-linux-androideabi-gcc \
	--nm=$PREBUILT/bin/arm-linux-androideabi-nm \
	--sysroot=$PLATFORM \
	--extra-cflags="$CFLAGS" \
	--extra-ldflags="$LDFLAGS" \
	--extra-libs="-lgcc" \
	--build_suffix="_neon_ub"
