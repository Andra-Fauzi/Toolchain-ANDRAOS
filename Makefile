# ==============================================================================
# AndraOS Cross-Compiler Modular Makefile
# Target: x86_64-andraos 
# Environment: MSYS2 (MSYS/Purple Terminal)
# ==============================================================================

PREFIX ?= /opt/andraos
TARGET ?= x86_64-andraos

# Ensure PREFIX/bin is in PATH for the build process
export PATH := /usr/bin:$(PREFIX)/bin:$(PATH)

SRC_DIR := $(CURDIR)
BINUTILS_SRC := $(SRC_DIR)/src/binutils
GCC_SRC := $(SRC_DIR)/src/gcc
NEWLIB_SRC := $(SRC_DIR)/src/newlib

BUILD_DIR := $(SRC_DIR)/build

CORES ?= $(shell nproc)

.PHONY: all directories clean binutils gcc-stage1 newlib gcc-stage2 rebuild-all

all: gcc-stage2
	@echo "SUCCESS! Toolchain komplit terinstall di $(PREFIX)/bin"

directories:
	@mkdir -p $(PREFIX)
	@mkdir -p $(BUILD_DIR)/binutils
	@mkdir -p $(BUILD_DIR)/gcc-stage1
	@mkdir -p $(BUILD_DIR)/newlib
	@mkdir -p $(BUILD_DIR)/gcc-stage2

# --- 1. Binutils ---
binutils: directories $(BUILD_DIR)/.binutils_done

$(BUILD_DIR)/.binutils_done:
	@echo ">>> Membangun Binutils..."
	@cd $(BUILD_DIR)/binutils && rm -rf * && \
	$(BINUTILS_SRC)/configure --target=$(TARGET) --prefix=$(PREFIX) --with-sysroot --disable-nls --disable-werror && \
	$(MAKE) -j$(CORES) && \
	$(MAKE) install
	@touch $@

# --- 2. GCC Stage 1 ---
gcc-stage1: binutils directories $(BUILD_DIR)/.gcc-stage1_done

$(BUILD_DIR)/.gcc-stage1_done: $(BUILD_DIR)/.binutils_done
	@echo ">>> Membangun GCC Stage 1..."
	@cd $(BUILD_DIR)/gcc-stage1 && rm -rf * && \
	$(GCC_SRC)/configure --target=$(TARGET) --prefix=$(PREFIX) --without-headers --with-newlib \
		--enable-languages=c,c++ --disable-nls --disable-shared --disable-multilib \
		--disable-threads --disable-fixincludes "inhibit_libc=true" && \
	$(MAKE) -j$(CORES) all-gcc && \
	$(MAKE) -j$(CORES) all-target-libgcc && \
	$(MAKE) install-gcc && \
	$(MAKE) install-target-libgcc
	@touch $@

# --- 3. Newlib ---
newlib: gcc-stage1 directories $(BUILD_DIR)/.newlib_done

$(BUILD_DIR)/.newlib_done: $(BUILD_DIR)/.gcc-stage1_done
	@echo ">>> Membangun Newlib (C Standard Library)..."
	@cd $(BUILD_DIR)/newlib && rm -rf * && \
	$(NEWLIB_SRC)/configure --target=$(TARGET) --prefix=$(PREFIX) \
		--disable-newlib-supplied-syscalls --disable-nls && \
	$(MAKE) -j$(CORES) && \
	$(MAKE) install
	@touch $@

# --- 4. GCC Stage 2 ---
gcc-stage2: newlib directories $(BUILD_DIR)/.gcc-stage2_done

$(BUILD_DIR)/.gcc-stage2_done: $(BUILD_DIR)/.newlib_done
	@echo ">>> Membangun GCC Stage 2 (Final)..."
	@cd $(BUILD_DIR)/gcc-stage2 && rm -rf * && \
	$(GCC_SRC)/configure --target=$(TARGET) --prefix=$(PREFIX) --with-newlib \
		--enable-languages=c,c++ --disable-nls --disable-shared --disable-multilib \
		--disable-threads --disable-fixincludes --disable-lto \
		--with-sysroot=$(PREFIX)/$(TARGET) --with-native-system-header-dir=/include && \
	$(MAKE) -j$(CORES) all-gcc && \
	$(MAKE) -j$(CORES) all-target-libgcc && \
	$(MAKE) install-gcc && \
	$(MAKE) install-target-libgcc
	@touch $@

# --- Clean up ---
clean:
	rm -rf $(BUILD_DIR)

rebuild-all: clean all
