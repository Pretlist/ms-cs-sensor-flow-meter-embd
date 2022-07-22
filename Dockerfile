########## DO NOT REMOVE BUILD STAGGING AREA#########
# 1. Stagging Area for build
FROM --platform=$TARGETPLATFORM us-central1-docker.pkg.dev/pret-registry/ci/ci-clang-llvm-arm:latest as target-lib
FROM --platform=$BUILDPLATFORM us-central1-docker.pkg.dev/pret-registry/ci/ci-clang-llvm-arm:latest as baking-stage
ARG TARGETPLATFORM
ARG BUILDPLATFORM
COPY --from=target-lib /root/pkgmgr/vcpkg/installed/ /root/pkgmgr/vcpkg/installed/
COPY --from=target-lib /root/pkgmgr/collection/ /root/pkgmgr/collection/

WORKDIR /workspace
COPY package.yaml package.yaml

# 2. Installing Package from vcpkg
RUN /root/pkgmgr/vcpkg/vcpkg install protobuf:arm64-linux
RUN set -o xtrace && \
    if [ -e package.yaml ]; then \
        /root/.local/bin/package-installer -t resolve package.yaml ;\
    fi

# 3. Install Source
ARG INSTALL_DIR="/staging-out"


COPY . .
# Start Change From Here
RUN set -o xtrace && \
    cmake -H. -Bbuild -DCMAKE_INSTALL_PREFIX="${INSTALL_DIR}" -DCMAKE_BUILD_TYPE=Release && \
    cd build && \
    make
# End Changes here

############## FINAL IMAGE BAKING  ###########
FROM --platform=$TARGETPLATFORM debian:stable-slim AS baking-stage1
#FROM scratch

#COPY --from=baking-stage1 /lib /lib
#COPY --from=baking-stage1 /lib64 /lib64
#COPY --from=baking-stage1 /usr/lib /usr/lib
COPY --from=baking-stage /usr/lib/x86_64-linux-gnu/libatomic.so.1 /usr/lib/x86_64-linux-gnu/libatomic.so
COPY --from=baking-stage /usr/lib/x86_64-linux-gnu/libatomic.so.1 /usr/lib/x86_64-linux-gnu/libatomic.so.1
#COPY --from=baking-stage1 /usr/lib/aarch64-linux-gnu/libstdc++.so.6.0.28 /usr/lib/aarch64-linux-gnu/libstdc++.so.6.0.28
#COPY --from=baking-stage1 /usr/lib/aarch64-linux-gnu/libstdc++.so.6 /usr/lib/aarch64-linux-gnu/libstdc++.so.6

COPY --from=baking-stage /workspace/build/bin/ms_sensor_flow_meter /ms/ms_sensor_flow_meter

CMD ["/ms/ms_sensor_flow_meter"]
