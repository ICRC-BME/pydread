#!/usr/bin/env bash

set -e

for PYVER in "cp35-cp35m" "cp36-cp36m" "cp37-cp37m"; do
  PYBIN="/opt/python/${PYVER}/bin"
  "${PYBIN}/pip" install numpy
  "${PYBIN}/pip" install -I wheel==0.30.0 #due to bug in wheel (https://github.com/pypa/wheel/issues/235)
  "${PYBIN}/python" setup.py bdist_wheel
done
find dist -name "*.whl" -exec auditwheel repair {} \;
