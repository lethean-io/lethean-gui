FROM lthn/build:lthn-wallet-linux
ARG THREADS=10
COPY . /lethean-gui

WORKDIR /lethean-gui
RUN  git submodule update --init --force --recursive
RUN make release-static -j${THREADS}