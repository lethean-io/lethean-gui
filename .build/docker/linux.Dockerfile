FROM lthn/build:wallet-linux
ENV THREADS=1
ARG BRANCH=next

WORKDIR /wallet-gui

RUN git clone --branch ${BRANCH} --recursive --depth 1 https://gitlab.com/lthn.io/projects/chain/wallet-gui.git .

CMD make release-static -j$THREADS
