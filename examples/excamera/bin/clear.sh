#!/bin/bash
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
cd $DIR/../

rm -rf ~/.cache/gg/remotes

rm -rf *.state *.ivf *.txt *.mgc .gg *.y4m
