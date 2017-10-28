#!/bin/bash

read -p "What commit message would you like ? "  msg
git add .
git commit -m "$msg"
git push http://kneerunjun:kneeru2git@github.com/PiFarm/co2sensing.git master
