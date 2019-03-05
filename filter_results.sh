#!/bin/bash
egrep "$(cat results.regex)" <&0
