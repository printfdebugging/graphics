#!/bin/env bash

pkgconfig_path="$(pwd)/install/lib/pkgconfig"
application_source_dir="$(pwd)/application"
application_build_dir="$(pwd)/build/application"

cmake \
	-DPKG_CONFIG_PATH="${pkgconfig_path}" \
	-S "${application_source_dir}" \
	-B "${application_build_dir}"

cmake --build "${application_build_dir}"
