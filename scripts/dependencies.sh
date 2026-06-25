#!/bin/env bash

if [ -f "/etc/os-release" ]; then
	. /etc/os-release
	case "$ID" in
	'msys2') pacman -Sy --noconfirm clang64/mingw-w64-clang-x86_64-{gdb,cmake,ninja,clang,clang-tools-extra,ccache,git,vulkan-headers,vulkan-loader} make ;;
	'arch') sudo pacman -S --noconfirm cmake make clang gdb ccache vulkan-devel libasan ;;
	*) echo "distribution not supported" && exit 1 ;;
	esac
fi
