FROM ubuntu:16.04

ADD ./ /codegen-bug/

RUN apt update && \
    apt install -y wget cmake build-essential tar xz-utils

ENV LINUX_VER=${LINUX_VER:-ubuntu-16.04}
ENV LLVM_VER=${LLVM_VER:-11.1.0}
ENV LLVM_DEP_URL=https://github.com/llvm/llvm-project/releases
ENV TAR_NAME=clang+llvm-${LLVM_VER}-x86_64-linux-gnu-${LINUX_VER}

RUN wget -q ${LLVM_DEP_URL}/download/llvmorg-${LLVM_VER}/${TAR_NAME}.tar.xz && \
    tar -C / -xf ${TAR_NAME}.tar.xz && \
    rm ${TAR_NAME}.tar.xz && \
    mv /${TAR_NAME} /clang+llvm

ENV PATH=/clang+llvm/bin:$PATH
ENV LD_LIBRARY_PATH=/clang+llvm/lib:$LD_LIBRARY_PATH

RUN cd /codegen-bug/pass/ && \
    rm build -rf && mkdir build -p && cd build && \
    cmake .. & make 

RUN cd /codegen-bug/src && \
    make