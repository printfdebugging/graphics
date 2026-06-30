define clangd_config_string
	@touch $(CLANGD_CONFIG)
	@if ! grep -q $(1) $(CLANGD_CONFIG); then \
		{ echo "---"; \
		  echo "If:"; \
		  echo "  PathMatch: .*$(1).*"; \
		  echo "CompileFlags:"; \
		  echo "  CompilationDatabase: $(2)"; \
		} | tee -a $(CLANGD_CONFIG); \
	fi
endef


define build_wrapper_cmake
$(1)_CONFIGURE_FLAGS=$$(shell cat $$(EXTERNALS_CONFIG)/$(1)/{flags.common,flags.$$(PLATFORM_CONFIG_EXTENSION)} | xargs)

$(1): $(1).configure $(1).build $(1).install $(1).clangd

$(1).configure:
	cmake \
		-DCMAKE_BUILD_TYPE=$$(CMAKE_BUILD_TYPE) \
		-DCMAKE_INSTALL_PREFIX=$$(INSTALL_DIRECTORY) \
		-DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
		$$($(1)_CONFIGURE_FLAGS) \
		-B "$$(BUILD_DIRECTORY)/$(1)" \
		-S "$$(EXTERNALS)/$(1)"

$(1).build:
	cmake --build "$$(BUILD_DIRECTORY)/$(1)"

$(1).clangd:
	$$(call clangd_config_string,$(1),$$(BUILD_DIRECTORY)/$(1))

$(1).install:
	cmake --install "$$(BUILD_DIRECTORY)/$(1)" --prefix "$$(INSTALL_DIRECTORY)"

$(1).clean:
	rm -rf "$$(BUILD_DIRECTORY)/$(1)"
endef

define build_wrapper_meson
$(1)_CONFIGURE_FLAGS=$$(shell cat $$(EXTERNALS_CONFIG)/$(1)/{flags.common,flags.$$(PLATFORM_CONFIG_EXTENSION)} | xargs)

$(1): $(1).configure $(1).build $(1).install $(1).clangd

$(1).configure:
	meson setup \
		-Dbuildtype=$$(MESON_BUILD_TYPE) \
		$$($(1)_CONFIGURE_FLAGS) \
		--prefix=$$(INSTALL_DIRECTORY) \
		"$$(BUILD_DIRECTORY)/$(1)" "$$(EXTERNALS)/$(1)"

$(1).build:
	meson compile -C "$$(BUILD_DIRECTORY)/$(1)"

$(1).clangd:
	$$(call clangd_config_string,$(1),$$(BUILD_DIRECTORY)/$(1))

$(1).install:
	meson install -C "$$(BUILD_DIRECTORY)/$(1)"

$(1).clean:
	rm -rf "$$(BUILD_DIRECTORY)/$(1)"
endef

define build_wrapper_autotools
$(1)_CONFIGURE_FLAGS=$$(shell cat $$(EXTERNALS_CONFIG)/$(1)/{flags.common,flags.$$(PLATFORM_CONFIG_EXTENSION)} | xargs)

$(1): $(1).configure $(1).build $(1).install $(1).clangd

$(1).configure:
	mkdir -p "$$(BUILD_DIRECTORY)/$(1)"
	cd "$$(BUILD_DIRECTORY)/$(1)" && \
	"$$(EXTERNALS)/$(1)/configure" \
		$$($(1)_CONFIGURE_FLAGS) \
		--prefix=$$(INSTALL_DIRECTORY)

$(1).build:
	make -C "$$(BUILD_DIRECTORY)/$(1)" -j

$(1).clangd:
	$$(call clangd_config_string,$(1),$$(BUILD_DIRECTORY)/$(1))

$(1).install:
	make -C "$$(BUILD_DIRECTORY)/$(1)" install

$(1).clean:
	rm -rf "$$(BUILD_DIRECTORY)/$(1)"
endef
