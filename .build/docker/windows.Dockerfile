FROM lthn/build:wallet-windows as build
ENV THREADS=1
ARG BRANCH=next
WORKDIR /wallet-gui

RUN  git clone --branch ${BRANCH} --recursive --depth 1 https://gitlab.com/lthn.io/projects/chain/wallet-gui.git .

RUN cp -r /wallet-gui/lethean/chain/contrib/depends/* /depends
COPY --from=lthn/build:depends-x86_64-w64-mingw32 / /depends

CMD make depends root=/depends target=x86_64-w64-mingw32 tag=win-x64 -j${THREADS}
