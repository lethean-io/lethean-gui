FROM lthn/build:lthn-wallet-windows as build
ARG THREADS=20
COPY . /lethean-gui

WORKDIR /lethean-gui

RUN  git submodule update --init --force --recursive
RUN set -x && make depends root=/depends target=x86_64-w64-mingw32 tag=win-x64 -j${THREADS}

FROM scratch AS export-stage
COPY --from=build /lethean-gui/build/release/bin /