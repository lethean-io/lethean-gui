FROM lthn/build:lthn-wallet-linux as builder
WORKDIR /home/lthn/wallet
COPY . .
RUN bash ci/run.sh