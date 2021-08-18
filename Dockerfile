FROM debian:11-slim

#COPY --chown=root:root --from=lthn/chain:next /home/lthn/chain/cli /usr/bin
COPY --chown=root:root --from=lthn/chain:latest /home/lthn/chain/bin /usr/bin

COPY lthn.sh /usr/bin/lthn
RUN chmod +x /usr/bin/lethean*
RUN chmod +x /usr/bin/lthn
RUN adduser  --disabled-password --gecos "" --shell /bin/bash lthn
USER lthn
RUN mkdir /home/lthn/wallet
WORKDIR /home/lthn/wallet

ENTRYPOINT ["lthn"]
