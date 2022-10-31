#!/bin/env bash
# echo $1
PATH=$1
YOUR_BUCKET=myawsbucket4serverless-storage
if [[ -n $1 ]] 
then
    /usr/bin/aws s3 cp --recursive s3://${YOUR_BUCKET}/timelog ${PATH}/
else
    echo "need to input path"
fi