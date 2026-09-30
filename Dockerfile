FROM debian:bookworm-slim
RUN apt-get update && apt-get install -y --no-install-recommends \
    gcc \
    make \
    iproute2 \
    tcpdump \
    libpcap-dev \
 && rm -rf /var/lib/apt/lists/*
WORKDIR /app
COPY . /app
RUN make