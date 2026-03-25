#!/bin/env bash

function install_bear_on_windows() {
	bear_upstream=https://github.com/rizsotto/Bear
	bear_source="$HOME/Bear"
	[ ! -d "${bear_source}" ] && git clone "${bear_upstream}" "${bear_source}"
	(cd "${bear_source}" && cargo build --release)
}

if [ -f "/etc/os-release" ]; then
	. /etc/os-release
	case "$ID" in
	'msys2')
		{
			prefix='clang64/mingw-w64-clang-x86_64'
			packages=(
				'gdb'
				'cmake'
				'ninja'
				'clang'
				'clang-tools-extra'
				'extra-cmake-modules'
				'nsis'
				'emscripten'
				'ccache'
				'make'
				'ripgrep'
				'nasm'
				'diffutils'
				'meson'
				'neovim'
				'rust'
				'git'
				'vulkan-headers'
				'vulkan-loader'
			)

			for package in "${packages[@]}"; do
				pacman -Sy "${prefix}-${package}"
			done

			install_bear_on_windows
		}
		;;
	'arch')
		{
			packages=(
				'cmake'
				'make'
				'clang'
				'gdb'
				'ccache'
				'vulkan-devel'
			)
			for package in "${packages[@]}"; do
				sudo pacman -S "${package}" --noconfirm
			done
		}
		;;
	*) echo "distribution not supported" && exit 1 ;;
	esac
fi
