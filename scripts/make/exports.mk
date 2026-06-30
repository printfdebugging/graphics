EXTERNALS 			= $(shell realpath ./external)
EXTERNALS_CONFIG 	= $(shell realpath ./config/external)
INSTALL_DIRECTORY = $(shell realpath ./install)
BUILD_DIRECTORY 	= $(shell realpath ./build)
PLATFORM 			= $(shell uname -s)
MESON_BUILD_TYPE 	= debug
CMAKE_BUILD_TYPE  = Debug

ifeq ($(PLATFORM),Linux)
	PLATFORM_CONFIG_EXTENSION = linux
else ifeq ($(PLATFORM),Darwin)
	PLATFORM_CONFIG_EXTENSION = apple
else ifeq ($(PLATFORM),MINGW64*)
	PLATFORM_CONFIG_EXTENSION = windows
else
	PLATFORM_CONFIG_EXTENSION = unknown
endif
