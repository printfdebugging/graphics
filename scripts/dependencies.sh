#!/bin/bash

if [ -f "/etc/os-release" ]; then
	. /etc/os-release
	case "$ID" in
	'msys2')
		{
			pacman -S clang64/mingw-w64-clang-x86_64-gdb \
				clang64/mingw-w64-clang-x86_64-cmake \
				clang64/mingw-w64-clang-x86_64-ninja \
				clang64/mingw-w64-clang-x86_64-clang \
				clang64/mingw-w64-clang-x86_64-clang-tools-extra \
				clang64/mingw-w64-clang-x86_64-extra-cmake-modules \
				clang64/mingw-w64-clang-x86_64-nsis \
				clang64/mingw-w64-clang-x86_64-emscripten \
				clang64/mingw-w64-clang-x86_64-ccache \
				clang64/mingw-w64-clang-x86_64-make \
				clang64/mingw-w64-clang-x86_64-ripgrep \
                clang64/mingw-w64-clang-x86_64-neovim \
                git
		}
		;;
	'arch')
		{
			sudo pacman -S --noconfirm \
				'cmake' \
				'make' \
				'clang' \
				'gdb' \
				'ccache'
		}
		;;
	*) echo "distribution not supported" && exit 1 ;;
	esac
fi
