#!/usr/bin/env bash

function check_url {
    echo "Check ${1}"
    curl -I -s -L "${1}" | head -n 1 | grep "200 OK" > /dev/null
}

function get_file {
    local FILE="$(basename ${1})"

    if [ -f $FILE ]; then
        echo "${FILE} already exists. Skipping"
    else
        echo "Downloading ${1}"
        curl --progress-bar -O "${1}"

        if [ ! $? -eq 0 ]; then
            echo "Download failed"
            exit 1
        fi
    fi
}

if [ "$#" -eq 0 ] || [ "$#" -gt 2 ]; then
    THIS=$(basename "$0")
    
    echo -e "usage: ${THIS} <collector> [D/M/Y]\n"
    echo "examples: '${THIS} rrc00 2016-03-08'"
    echo "          '${THIS} rrc14' (last day)"

    exit 0
fi

RIS_BASE="http://data.ris.ripe.net"
COLLECTOR=${1}
DATE=$(date -u -d yesterday +%F)

if [ "$#" -eq 2 ]; then
    if ! DATE=$(date -u -d ${2} +%F); then exit 1; fi
fi

# Check if collector exists
check_url "${RIS_BASE}/${COLLECTOR}/latest-bview.gz"

if [ ! $? -eq 0 ]; then
    echo "Unknown collector '${1}'"
    exit 1
fi

YEAR=$(date -u -d ${DATE} +%Y)
MONTH=$(date -u -d ${DATE} +%m)
DAY=$(date -u -d ${DATE} +%d)

MONTH_BASE="${RIS_BASE}/${COLLECTOR}/${YEAR}.${MONTH}"
UPDATES_BASE="${MONTH_BASE}/updates.${YEAR}${MONTH}${DAY}."
RIB_FILE="${MONTH_BASE}/bview.${YEAR}${MONTH}${DAY}.0000.gz"

# Check if midnight RIB file exists
check_url $RIB_FILE

if [ ! $? -eq 0 ]; then
    echo "Requested date not available"
    exit 1
fi

# Check if last update file of requested day exists
check_url "${UPDATES_BASE}2355.gz"

if [ ! $? -eq 0 ]; then
    echo "Updates for whole day not complete"
    exit 1
fi

echo "Getting data set for collector ${COLLECTOR} on ${DATE}"

TARGET_DIR="${PWD}/${COLLECTOR}-${YEAR}-${MONTH}-${DAY}"

mkdir -p ${TARGET_DIR} && cd ${TARGET_DIR}

get_file ${RIB_FILE}

START_TIMESTAMP=$(date -u -d ${DATE} +%s)
END_TIMESTAMP=$((START_TIMESTAMP + 300))
CSV_FILE="$(basename ${TARGET_DIR}).csv"

echo "$(basename ${RIB_FILE}),ris,ribs,${COLLECTOR},${START_TIMESTAMP},300,$(($END_TIMESTAMP))" > $CSV_FILE

for h in $(seq -w 0 23); do
    for m in $(seq -w 0 5 55); do
        UPDATES_FILE="${UPDATES_BASE}${h}${m}.gz"
        get_file $UPDATES_FILE
        echo "$(basename ${UPDATES_FILE}),ris,updates,${COLLECTOR},${START_TIMESTAMP},300,$(($END_TIMESTAMP))" >> $CSV_FILE
        START_TIMESTAMP=$END_TIMESTAMP
        END_TIMESTAMP=$((START_TIMESTAMP + 300))
    done
done
