#!/bin/bash

if [ "$#" -ne 2 ]; then
    echo "Zła liczba parametrów"
    exit 1
fi

PROG=$1
DIR="$2"

if [ ! -x "$PROG" ] || [ ! -f "$PROG" ]; then #program ma mieć flagę wykonywalną i być plikiem a nie katalogiem
    echo "$PROG nie jest wykonywalny"
    exit 1
fi

if [ ! -d "$DIR" ]; then
    echo "$DIR nie jest katalogiem"
    exit 1
fi

DIR="${DIR%/}" #upewniam się, że na końcu nie ma slasha

FOUND_START=0
START=""

FILES="$DIR/*"
for file in $FILES; do
    #echo "Sprawdzam $file"
    FIRSTLINE=`head -1 "${file}"`
    if [ "$FIRSTLINE" = "START" ]; then
        START="${file}"
        FOUND_START=1
        break
    fi
done

if [ $FOUND_START -ne 1 ]; then
    echo "Nie znaleziono pliku zacznynającego się linią START"
    exit 1
fi

TRIMMED_START=`mktemp`
cat "$START" | tail -n+2 > "$TRIMMED_START"
CURRENT_INPUT="$TRIMMED_START"
PREV_RESULT=`mktemp`
RESULT=`mktemp`

while true
do
    NEXT_INPUT=`tail -n1 "$CURRENT_INPUT"`

    cat "$CURRENT_INPUT" | head -n-1 | cat "$PREV_RESULT" - | "./$PROG" > "$RESULT"

    if [ "$NEXT_INPUT" = "STOP" ]; then
        break
    else
        NEXT_FILE=`echo "$NEXT_INPUT" | sed "s/FILE //g"`
        mv "$RESULT" "$PREV_RESULT"
        CURRENT_INPUT="$DIR/$NEXT_FILE"
        
        if [ ! -f "$CURRENT_INPUT" ]; then
            echo "$CURRENT_INPUT nie jest plikiem!"
            break
        fi
    fi
done

cat "$RESULT"
rm -f "$RESULT" "$PREV_RESULT" "$TRIMMED_START"
