# Uses a pre configured ubuntu:16.04 image
FROM lthn/build:release-3.1.0 as builder

RUN apt-get update && apt-get install -y qtbase5-dev qt5-default qtdeclarative5-dev qml-module-qtquick-controls \
    qml-module-qtquick-xmllistmodel qttools5-dev-tools qml-module-qtquick-dialogs qml-module-qt-labs-settings \
    libqt5qml-graphicaleffects qtmultimedia5-dev qml-module-qtmultimedia libzbar-dev libicu-dev
# Where all the works done.
WORKDIR /usr/local/src/lthn/lethean-gui

COPY . .
# make type to use, to change --build-arg RELEASE_TYPE=release-test
ARG RELEASE_TYPE=release-static

# if you want to clear build, purge the runner cache/prune the builder
RUN rm -rf build/ && ./build.sh ${RELEASE_TYPE} && cd build && make deploy

ARG BUILD_VERSION="4.0.2"
ARG RELEASE_NAME="lethean-gui-linux-64bit-$BUILD_VERSION"

WORKDIR /usr/local/src/lthn/lethean-gui/build/release/bin

RUN mkdir -p $RELEASE_NAME \
    && cp letheand $RELEASE_NAME/ \
    && cp lethean-wallet-gui $RELEASE_NAME/ \
    && cp -R qml $RELEASE_NAME/ \
    && cp -R libs $RELEASE_NAME/ \
    && cp -R plugins $RELEASE_NAME/ \
    && cp -R translations $RELEASE_NAME/ \
    && cp start-gui.sh $RELEASE_NAME/ \
    && cp ../../../ci/package-artifacts/*.http $RELEASE_NAME/

RUN tar -cvjf $RELEASE_NAME.tar.bz2 $RELEASE_NAME && sha256sum $RELEASE_NAME.tar.bz2 > $RELEASE_NAME.tar.bz2.sha256.txt \
    && cp $RELEASE_NAME.tar.bz2 /$RELEASE_NAME.tar.bz2



