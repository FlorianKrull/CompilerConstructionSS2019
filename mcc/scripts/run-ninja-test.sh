#!!/bin/bash

cd ..
rm -rf builddir
meson builddir
cd builddir
ninja test