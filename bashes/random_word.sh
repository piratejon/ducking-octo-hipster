#!/bin/bash

LINES=`wc -l /usr/share/dict/words | cut -d' ' -f1`
RND=$(($LINES * $RANDOM / 32767))
head -n $RND /usr/share/dict/words | tail -n 1
