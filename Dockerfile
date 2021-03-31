# Uses a pre configured ubuntu:16.04 image
FROM registry.gitlab.com/lethean.io/devops:latest as builder

RUN apt-get install -y qtbase5-dev qt5-default qtdeclarative5-dev qml-module-qtquick-controls \
    qml-module-qtquick-xmllistmodel qttools5-dev-tools qml-module-qtquick-dialogs qml-module-qt-labs-settings \
    libqt5qml-graphicaleffects lsb-core libunwind8-dev
# Where all the works done.
WORKDIR /usr/local/src/lethean.io/blockchain/lethean-gui

COPY . .
# make type to use, to change --build-arg RELEASE_TYPE=release-test
ARG RELEASE_TYPE=release-static

# if you want to clear build, purge the runner cache/prune the builder
RUN rm -rf build/ && ./build.sh ${RELEASE_TYPE}

# New image, changes output image to a fresh Ubuntu image.
FROM ubuntu:16.04

# grab the files made in the builder stage
COPY --from=builder /usr/local/src/lethean.io/blockchain/lethean-gui/build/release/bin /usr/local/bin
COPY --from=builder /usr/local/src/lethean.io/blockchain/lethean-gui/build/release/bin /home/leathean/bin

# clean up this new ubuntu
RUN apt-get update && \
    apt-get --no-install-recommends --yes install ca-certificates libreadline6 && \
    apt-get clean && \
    rm -rf /var/lib/apt

# Create lethean user
RUN adduser --system --group --disabled-password lethean && \
	mkdir -p /wallet /home/lethean/.lethean /home/lethean/.intensecoin /home/lethean/bin && \
	chown -R lethean:lethean /home/lethean && \
	chown -R lethean:lethean /wallet

# a copy of the binaries for extraction.
VOLUME /home/lethean/bin

# switch to lethean
USER lethean

