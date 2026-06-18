FROM ghcr.io/msys2/msys2-docker-experimental
ENV MSYSTEM=CLANG64
SHELL ["/bin/msys2", "-c"]
RUN pacman -Sy --noconfirm clang64/mingw-w64-clang-x86_64-{gdb,cmake,ninja,clang,ccache,git,vulkan-headers,vulkan-loader} make

COPY . .
RUN make debug
