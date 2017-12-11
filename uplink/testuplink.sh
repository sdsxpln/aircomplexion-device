#!/bin/sh
# this sole purpose for this file is to test the uplink functionality


gcc ./uplink.c -o./uplink -lcurl
./uplink
