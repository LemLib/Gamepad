#!/bin/bash
pip install pros-cli
pushd $PWD
cd ~
curl https://armkeil.blob.core.windows.net/developer/Files/downloads/gnu-rm/10.3-2021.10/gcc-arm-none-eabi-10.3-2021.10-x86_64-linux.tar.bz2 -L -O
tar -xjvf gcc-arm-none-eabi-10.3-2021.10-x86_64-linux.tar.bz2
echo \"export PATH=$PATH:/home/codespace/gcc-arm-none-eabi-10.3-2021.10/bin/\" >> ~/.bashrc
source ~/.bashrc
popd
