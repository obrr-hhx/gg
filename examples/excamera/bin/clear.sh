#!/bin/bash
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
cd $DIR/../

rm -rf *.state *.ivf *.y4m *.txt *.mgc .gg

aws s3 rm --recursive $AWS_S3