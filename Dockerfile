FROM ubuntu:22.04

# 设置环境变量
ENV DEBIAN_FRONTEND=noninteractive

# 安装依赖
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    qt6-base-dev \
    qt6-base-dev-tools \
    qt6-svg-dev \
    git \
    wget \
    && rm -rf /var/lib/apt/lists/*

# 设置工作目录
WORKDIR /build

# 复制源代码
COPY . /build/

# 编译
RUN mkdir -p build && cd build && \
    cmake .. && \
    make -j$(nproc)

# 复制资源文件
RUN cp rules.json build/ && \
    cp resources/icon.svg build/

# 设置输出目录
VOLUME ["/output"]

# 复制编译结果到输出目录
RUN mkdir -p /output && \
    cp build/AutoSort /output/ && \
    cp build/rules.json /output/ && \
    cp build/icon.svg /output/

# 设置默认命令
CMD ["/bin/bash"]