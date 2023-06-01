#!/usr/bin/bash

PIPE=mypipe
PLAYLIST_DIR=./buildroot/overlay/home/music

# start playlist_manager child
(
cd playlist_manager || exit
pwd
python3 app.py .$PLAYLIST_DIR
)

# start buttons receiver child
(
cd player || exit
pwd
./player .$PLAYLIST_DIR/$PIPE .$PLAYLIST_DIR
)

# start mpg123 child
(
cd $PLAYLIST_DIR || exit
pwd
mpg123 -R --fifo $PIPE
echo "V 50" > $PIPE
cd ../../../.. || exit
)

kill -7 "0"
