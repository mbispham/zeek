#!/bin/sh

echo "Preparing macOS environment"
sysctl hw.model hw.machine hw.ncpu hw.physicalcpu hw.logicalcpu
set -e
set -x

brew update
brew upgrade cmake
brew install openssl@3 swig bison flex ccache libmaxminddb

if [ $(sw_vers -productVersion | cut -d '.' -f 1) -lt 14 ]; then
    python3 -m pip install --upgrade pip
fi

python3 -m pip install --user --break-system-packages websockets

# Brew doesn't create the /opt/homebrew/opt/openssl symlink if you install
# openssl@1.1, only with 3.0. Create the symlink if it doesn't exist.
#if [ ! -e /opt/homebrew/opt/openssl ]; then
#    if [ -d /opt/homebrew/opt/openssl@1.1 ]; then
#        ln -s /opt/homebrew/opt/openssl@1.1 /opt/homebrew/opt/openssl
#    fi
#fi
