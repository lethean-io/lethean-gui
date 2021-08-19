FROM lthn/build:lthn-wallet-linux as build
ARG THREADS=1
RUN apt-get install -y libc6-dev
COPY . /lethean-gui

WORKDIR /lethean-gui

RUN git submodule update --init --force --recursive
RUN make release-static -j${THREADS}

FROM scratch AS export-stage
COPY --from=build /lethean-gui/build/release/bin /