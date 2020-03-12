#!/bin/bash

docker run --rm -v `pwd`/:/share -v ${lan_base_dir}/demos/common_header_demo/common:/common freelw/lan_make:latest
