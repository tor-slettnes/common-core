## -*- Makefile -*-
#===============================================================================
## @file Makefile
## @brief Build via CMake
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

MAKEFLAGS    += --no-print-directory
SHARED_DIR   ?= $(dir $(realpath $(lastword $(MAKEFILE_LIST))))
OUT_DIR      ?= $(CURDIR)/out
BUILD_DIR    ?= $(OUT_DIR)/build
INSTALL_DIR  ?= $(OUT_DIR)/install
PACKAGE_DIR  ?= $(OUT_DIR)/packages
PYTHON_VENV  ?= $(CURDIR)/venv

ifdef TARGET
    export CMAKE_TOOLCHAIN_FILE ?= $(SHARED_DIR)cmake/toolchain-$(TARGET).cmake
	BUILD_DIR := $(BUILD_DIR)-$(TARGET)
	INSTALL_DIR := $(BUILD_DIR)-$(TARGET)
	PACKAGE_DIR := $(BUILD_DIR)-$(TARGET)
else
	TARGET := $(shell uname -s)-$(shell uname -m)

    ifneq ("$(wildcard $(SHARED_DIR)cmake/toolchain-$(TARGET).cmake)","")
        export CMAKE_TOOLCHAIN_FILE ?= $(SHARED_DIR)cmake/toolchain-$(TARGET).cmake
    endif
endif

ifdef BUILD_TYPE
	BUILD_DIR := $(BUILD_DIR)-$(BUILD_TYPE)
else
	BUILD_TYPE = Release
endif

export CMAKE_BUILD_TYPE ?= $(BUILD_TYPE)
ifeq ($(shell uname), Linux)
   export CMAKE_BUILD_PARALLEL_LEVEL = $(shell nproc)
endif

CONFIG_ARGS = $(if $(PRODUCT),-D PRODUCT="$(PRODUCT)")
CONFIG_ARGS += $(if $(VERSION), -D VERSION="$(VERSION)")
CONFIG_ARGS += $(if $(BUILD_NUMBER), -D BUILD_NUMBER="$(BUILD_NUMBER)")
CONFIG_ARGS += $(if $(PACKAGE_NAME), -D PACKAGE_NAME_PREFIX="$(PACKAGE_NAME)")

CMAKE_TAG = $(BUILD_DIR)/Makefile

### Check for a target-specific toolchain and use that if available

.PHONY: install
install: test local

.PHONY: release
release: test package

.PHONY: package
package: deb

.PHONY: deb
deb: build
	@echo
	@echo "#############################################################"
	@echo "Creating release packages in ${PACKAGE_DIR}"
	@echo "#############################################################"
	@echo
	@cpack --config "${BUILD_DIR}/CPackConfig.cmake" -B "${PACKAGE_DIR}" -G DEB

.PHONY: local
local: build
	@echo
	@echo "#############################################################"
	@echo "Installing locally in ${INSTALL_DIR}"
	@echo "#############################################################"
	@echo
	@cmake --install $(BUILD_DIR) --prefix $(INSTALL_DIR)

.PHONY: local/strip
local/strip: build
	@echo
	@echo "#############################################################"
	@echo "Installing and stripping in ${INSTALL_DIR}"
	@echo "#############################################################"
	@echo
	@cmake --install $(BUILD_DIR) --prefix $(INSTALL_DIR) --strip

.PHONY: uninstall
uninstall:
	@rm -rfv "$(INSTALL_DIR)"

.PHONY: doc
doc: cmake
	@echo
	@echo "#############################################################"
	@echo "Generating documentation"
	@echo "#############################################################"
	@echo
	@cmake --build "$(BUILD_DIR)" --target doxygen

.PHONY: test
test: build
	@echo
	@echo "#############################################################"
	@echo "Testing in ${BUILD_DIR}"
	@echo "#############################################################"
	@echo
	@cmake --build "$(BUILD_DIR)" --target test

.PHONY: build
build: cmake
	@echo
	@echo "#############################################################"
	@echo "Building in ${BUILD_DIR}"
	@echo "#############################################################"
	@echo
	@cmake --build "$(BUILD_DIR)"

.PHONY: cmake
cmake: $(CMAKE_TAG)

ifneq ($(strip $(CONFIG_ARGS)),)
.PHONY: $(CMAKE_TAG)
endif

$(CMAKE_TAG):
	@echo
	@echo "#############################################################"
	@echo "Generating build files in ${BUILD_DIR}"
	@echo "#############################################################"
	@echo
	@cmake -B "$(BUILD_DIR)" $(CONFIG_ARGS) -D PYTHON_VENV=$(PYTHON_VENV)

.PHONY: cmake_clean
cmake_clean:
	@[ -d "$(BUILD_DIR)" ] && cmake --build "$(BUILD_DIR)" --target clean

.PHONY: pkg_clean package_clean
pkg_clean package_clean:
	@rm -rfv "$(PACKAGE_DIR)"

.PHONY: clean
clean: pkg_clean cmake_clean

.PHONY: realclean
realclean: clean
	@rm -rfv "$(BUILD_DIR)" "$(INSTALL_DIR)" "$(PACKAGE_DIR)"

.PHONY: realclean_all
realclean_all:
	@rm -rfv "$(OUT_DIR)/build" "$(OUT_DIR)/install" "$(OUT_DIR)/packages"

.PHONY: distclean
distclean:
	@echo "Removing all build outputs: ${OUT_DIR}"
	@rm -rf "$(OUT_DIR)"

.PHONY: pristine
pristine: distclean venv_clean

# .PHONY: venv
venv: cmake
	@cmake --build "$(BUILD_DIR)" --target python_venv

.PHONY: venv_clean
venv_clean:
	@echo "Removing Python Virtual Environment: $(PYTHON_VENV)"
	@rm -rf "$(PYTHON_VENV)"

$(BUILD_DIR):
	@mkdir -p "$(BUILD_DIR)"

### Delegate docker_ targets to its own Makefile
docker_%:
	@$(MAKE) -C $(SHARED_DIR)/scripts/docker $(MAKECMDGOALS) HOST_DIR=$(CURDIR)

### Delegate any other target to CMake
%:
	@[ -f "$(CMAKE_TAG)" ] || cmake -B "$(BUILD_DIR)" $(CONFIG_ARGS)
	@cmake --build "$(BUILD_DIR)" --target $(patsubst cmake_%,%,$@)

