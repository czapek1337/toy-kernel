#!/bin/bash

if [[ -d "sysroot/system-root" ]]; then
  tar -cf $1 -C sysroot/system-root .
else
  echo "Please build the system root first:"
  echo " $ mkdir sysroot && cd sysroot"
  echo " $ xbstrap init .."
  echo " $ xbstrap install -u --all"
  echo ""

  exit 1
fi
