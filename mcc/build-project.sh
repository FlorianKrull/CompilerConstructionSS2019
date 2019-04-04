#!!/bin/bash

rm -rf builddir
meson builddir
cd builddir
ninja