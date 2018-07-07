#!/bin/bash

set -x

DIR=/data/SHiP_testBeam/Data
inotifywait -r -m "$DIR" -e create -e moved_to |
    while read path action FILE; do
        echo "The file '$FILE' appeared in directory '$path' via '$action'"
	[[ $(basename $FILE) =~ ^RUN_ ]] && RUNDIR=$(basename $FILE) && RUN=${RUNDIR:4} && RUN=$((10#$RUN)) && ./elog.py --text "New run $RUN being uploaded" --subject "Start conversion run $RUN" --run $RUN && continue
	RUNDIR=$(basename $path)
	RUN=${RUNDIR:4}
	RUN=$((10#$RUN))
	OUTPUTPATH=/eos/experiment/ship/data/muflux/rawdata/$RUNDIR
	xrdfs $EOSSHIP stat $OUTPUTPATH || xrdfs $EOSSHIP mkdir $OUTPUTPATH
	xrdcp $FILE $EOSSHIP$OUTPUTPATH && rm $FILE
	flock files_to_convert.lock printf "%s\\n" "$EOSSHIP$OUTPUTPATH$(basename "$FILE")" >> files_to_convert.txt
    done
