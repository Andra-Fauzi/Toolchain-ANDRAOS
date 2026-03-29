#!/bin/bash

# ==============================================================================
# AndraOS Cross-Compiler Build Script (Unified Source)
# Target: x86_64-andraos 
# Environment: MSYS2 (MSYS/Purple Terminal)
# ==============================================================================

set -e # Berhenti jika ada error

# 1. KONFIGURASI
export PREFIX="/opt/andraos"
export TARGET="x86_64-andraos"
export PATH="/usr/bin:$PREFIX/bin:$PATH"

# Lokasi Source di dalam folder gabungan ini
SRC_DIR=$(pwd)
BINUTILS_SRC="$SRC_DIR/src/binutils"
GCC_SRC="$SRC_DIR/src/gcc"
NEWLIB_SRC="$SRC_DIR/src/newlib"

# Persiapan Folder Build
mkdir -p "$PREFIX"
mkdir -p build/binutils build/gcc-stage1 build/newlib build/gcc-stage2

echo "--- MEMULAI BUILD TOOLCHAIN YAHUD ANDRAOS ($TARGET) ---"

# 2. BUILD BINUTILS
echo ">>> Membangun Binutils..."
cd "$SRC_DIR/build/binutils"
rm -rf *
"$BINUTILS_SRC/configure" --target=$TARGET --prefix="$PREFIX" --with-sysroot --disable-nls --disable-werror
make -j$(nproc)
make install

# 3. BUILD GCC STAGE 1
echo ">>> Membangun GCC Stage 1..."
cd "$SRC_DIR/build/gcc-stage1"
rm -rf *
"$GCC_SRC/configure" --target=$TARGET --prefix="$PREFIX" --without-headers --with-newlib \
    --enable-languages=c,c++ --disable-nls --disable-shared --disable-multilib \
    --disable-threads --disable-fixincludes "inhibit_libc=true"
make -j$(nproc) all-gcc
make -j$(nproc) all-target-libgcc
make install-gcc
make install-target-libgcc

# 4. BUILD NEWLIB
echo ">>> Membangun Newlib (C Standard Library)..."
cd "$SRC_DIR/build/newlib"
rm -rf *
"$NEWLIB_SRC/configure" --target=$TARGET --prefix="$PREFIX" \
    --disable-newlib-supplied-syscalls --disable-nls
make -j$(nproc)
make install

# 5. BUILD GCC STAGE 2 (FINAL)
echo ">>> Membangun GCC Stage 2 (Final)..."
cd "$SRC_DIR/build/gcc-stage2"
rm -rf *
"$GCC_SRC/configure" --target=$TARGET --prefix="$PREFIX" --with-newlib \
    --enable-languages=c,c++ --disable-nls --disable-shared --disable-multilib \
    --disable-threads --disable-fixincludes --disable-lto \
    --with-sysroot="$PREFIX/$TARGET" --with-native-system-header-dir=/include
make -j$(nproc) all-gcc
make -j$(nproc) all-target-libgcc
make install-gcc
make install-target-libgcc

echo "SUCCESS! Toolchain komplit terinstall di $PREFIX/bin"
