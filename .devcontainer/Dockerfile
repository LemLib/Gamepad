FROM rust:1.67 as builder

RUN git clone https://github.com/vexide/vex-v5-qemu.git /vex-v5-qemu

WORKDIR /vex-v5-qemu
RUN cd packages/kernel; cargo build --target-dir /target/kernel

RUN cd packages/client-cli; cargo install --path . --root /target/client-cli

FROM mcr.microsoft.com/devcontainers/cpp:1.0-jammy
# ------------
# Install Required Packages
# ------------   
RUN sudo apt-get update
COPY ./.devcontainer/packagelist /packagelist
RUN sudo apt-get -y install $(cat /packagelist)
RUN rm /packagelist # Cleanup image
RUN sudo apt-get clean # Cleanup image

# ------------
# Install Clangd
# ------------
RUN curl -sLo clangd.zip $(                                                                 \
    curl -s https://api.github.com/repos/clangd/clangd/releases/latest                      \
    | jq -r '[.assets[] | select(.name | test ("^clangd-linux"))][0].browser_download_url'  \
  )                                                                                         \
  && unzip clangd.zip -d /usr/local/share                                                   \
  && mv /usr/local/share/clangd_*/ /usr/local/share/clangd                                  \
  && rm clangd.zip

ENV PATH="$PATH:/usr/local/share/clangd/bin"

# ------------
# Install PROS CLI
# ------------
RUN python3 -m pip install pros-cli

# ------------
# Install ARM Toolchain
# ------------
COPY --from=ghcr.io/lemlib/pros-build:v2.0.2 /gcc-arm-none-eabi-10.3-2021.10 /usr/local/share/arm-none-eabi
ENV PATH="$PATH:/usr/local/share/arm-none-eabi/bin"  

# Copy the simulator binary
COPY --from=builder /target/client-cli/bin/client-cli /usr/local/bin/simulator

# Clone pros kernel source so we can reference it when debugging
COPY ./project.pros /project.pros
ENV PROS_SOURCE_PATH="$HOME/.pros"
RUN git clone https://github.com/purduesigbots/pros.git $PROS_SOURCE_PATH   \
  --depth 1                                                                 \
  --branch $(</project.pros jq -r '.["py/state"].templates.kernel.version')
RUN rm /project.pros # Cleanup image

# Copy the sim kernel, as its required to run the simulator
ENV V5_SIM_PATH="$HOME/.vex-v5-qemu"
ENV V5_SIM_KERNEL_PATH="$V5_SIM_PATH/kernel"
COPY --from=builder /target/kernel/armv7a-none-eabi/debug/kernel $V5_SIM_KERNEL_PATH

# Copy the sim source, so we can reference it when debugging
ENV V5_SIM_SOURCE_PATH="$V5_SIM_PATH/source"
COPY --from=builder /vex-v5-qemu $V5_SIM_SOURCE_PATH