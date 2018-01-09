#!/bin/bash

read -p "What commit message would you like ? "  msg
git add .
git commit -m "$msg"
git push origin master
