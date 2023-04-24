#!/bin/bash

mkdir peoplenet && cd peoplenet
echo "Downloading pruned tlt peoplenet model.... "
wget --content-disposition https://api.ngc.nvidia.com/v2/models/nvidia/tao/peoplenet/versions/pruned_quantized_v2.3.2/zip -O peoplenet.zip 
unzip peoplenet.zip
rm -r peoplenet.zip

