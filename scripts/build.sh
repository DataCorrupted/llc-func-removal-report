# Build pass
cd ./pass/ && \
    rm build -rf && mkdir build -p && cd build && \
    cmake .. && make

# Build main and run pass
cd ../../src && make