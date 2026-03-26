#!/bin/env bash

function install_bear_on_windows() {
	bear_upstream=https://github.com/rizsotto/Bear
	bear_source="$HOME/Bear"
	[ ! -d "${bear_source}" ] && git clone "${bear_upstream}" "${bear_source}"
	(cd "${bear_source}" && cargo build --release)
	install -m 755 ${bear_source}/target/release/{bear.exe,exec.dll,wrapper.exe} /clang64/bin
}

if [ -f "/etc/os-release" ]; then
	. /etc/os-release
	case "$ID" in
	'msys2')
		{
			pacman -Sy \
				'clang64/mingw-w64-clang-x86_64-gdb' \
				'clang64/mingw-w64-clang-x86_64-cmake' \
				'clang64/mingw-w64-clang-x86_64-ninja' \
				'clang64/mingw-w64-clang-x86_64-clang' \
				'clang64/mingw-w64-clang-x86_64-clang-tools-extra' \
				'clang64/mingw-w64-clang-x86_64-extra-cmake-modules' \
				'clang64/mingw-w64-clang-x86_64-nsis' \
				'clang64/mingw-w64-clang-x86_64-emscripten' \
				'clang64/mingw-w64-clang-x86_64-ccache' \
				'clang64/mingw-w64-clang-x86_64-ripgrep' \
				'clang64/mingw-w64-clang-x86_64-nasm' \
				'clang64/mingw-w64-clang-x86_64-cc' \
				'clang64/mingw-w64-clang-x86_64-diffutils' \
				'clang64/mingw-w64-clang-x86_64-meson' \
				'clang64/mingw-w64-clang-x86_64-neovim' \
				'clang64/mingw-w64-clang-x86_64-rust' \
				'clang64/mingw-w64-clang-x86_64-git' \
				'clang64/mingw-w64-clang-x86_64-pkgconf' \
				'clang64/mingw-w64-clang-x86_64-vulkan-headers' \
				'clang64/mingw-w64-clang-x86_64-vulkan-loader' \
				'clang64/mingw-w64-clang-x86_64-amf-headers' \
				'clang64/mingw-w64-clang-x86_64-autotools' \
				'clang64/mingw-w64-clang-x86_64-dlfcn' \
				'clang64/mingw-w64-clang-x86_64-ffnvcodec-headers' \
				'make'

			# install_bear_on_windows
		}
		;;
	'arch')
		{
			sudo pacman -S --noconfirm \
				'cmake' \
				'make' \
				'clang' \
				'gdb' \
				'ccache' \
				'vulkan-devel'
		}
		;;
	*) echo "distribution not supported" && exit 1 ;;
	esac
fi
