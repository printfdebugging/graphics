#!/bin/env bash

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
				'vulkan-headers'
				'vulkan-loader'
			)

			for package in "${packages[@]}"; do
				pacman -Sy "${prefix}-${package}"
			done
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
