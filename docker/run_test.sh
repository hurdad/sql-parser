#!/bin/bash

cd "$(dirname "$0")"
cd ..
make mode=${MODE} test