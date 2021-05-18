FROM ubuntu:16.04

ADD ./ /codegen-bug/

RUN apt update && \
    apt install -y wget cmake build-essential tar xz-utils

ENV PREFIX=/
RUN cd /codegen-bug && ./scripts/install_llvm.sh

ENV PATH=/clang+llvm/bin:$PATH
ENV LD_LIBRARY_PATH=/clang+llvm/lib:$LD_LIBRARY_PATH

RUN cd /codegen-bug && ./scripts/build.sh