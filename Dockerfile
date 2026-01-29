FROM ubuntu:22.04 AS builder

# Install dependencies
ENV DEBIAN_FRONTEND=noninteractive

# Install required packages
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    ffmpeg \
    libssl-dev \
    zlib1g-dev \
    && rm -rf /var/lib/apt/lists/*

# Set working directory
WORKDIR /workspace
# Copy source code: all the files in the current directory to /app in the container
COPY . .

# check sdk
RUN echo "validating sdk and source file..." && \
    ls -la lib/ && \
    [ -f "lib/libconversation.so" ] || (echo "no sdk file found!" && exit 1)

# Build the application
RUN mkdir -p build && cd build && \
    cmake .. && \
    make -j$(nproc)

# ===== stage 2 ======
FROM ubuntu:22.04

RUN apt-get update && apt-get install -y \
    libssl-dev \
    zlib1g-dev \
    ffmpeg \
    && rm -rf /var/lib/apt/lists/*

# Set working directory
WORKDIR /app

# Copy the built application from the builder stage
COPY --from=builder /workspace/build/conv_demo .

COPY --from=builder /workspace/lib/libconversation.so /usr/lib/
RUN ldconfig

COPY --from=builder /workspace/resources ./resources

COPY --from=builder /workspace/audio_16k.pcm .

COPY --from=builder /workspace/test_img.jpg .

COPY --from=builder /workspace/resources_aec_kws_vad_android ./resources_aec_kws_vad_android

CMD ["./conv_demo"]