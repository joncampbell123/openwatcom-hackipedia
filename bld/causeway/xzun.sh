#!/bin/bash
xz -c -d "$1" >"$2" || exit 1
exit 0

