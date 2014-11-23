#!/bin/sh

IMAGE=${1:?"Missing parameter IMAGE"}
WIDTH=${2:?"Missing parameter WIDTH"}
HEIGHT=${3:?"Missing parameter HEIGHT"}
NUMSPRITE=${4:?"Missing parameter NUMSPRITES"}

TEMPDIR=$(mktemp -d)

IMAGENAME=${IMAGE%.*}
convert ${IMAGE} -crop 8x8 BMP3:${TEMPDIR}/${IMAGENAME}_%d.bmp

CONCATLIST=""
CURINDEX=0
IMAGELEN=$(echo "${WIDTH} * ${NUMSPRITE} " | bc)
for i in $(seq ${IMAGELEN}); do
    CURINDEX=$(echo "${i} - 1" | bc)
    for j in $(seq ${HEIGHT}); do
        CONCATLIST="${CONCATLIST} ${TEMPDIR}/${IMAGENAME}_${CURINDEX}.bmp"
        CURINDEX=$(echo "${CURINDEX} + ${WIDTH}*${NUMSPRITE}" | bc)
    done
done

montage -mode concatenate -tile ${IMAGELEN}x${HEIGHT} ${CONCATLIST} BMP3:${IMAGENAME}_sprite.bmp
convert ${IMAGENAME}_sprite.bmp -remap ${IMAGE} BMP3:${IMAGENAME}_sprite.bmp
rm -rf ${TEMPDIR}
