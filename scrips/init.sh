#!/bin/bash

# rename to project specific values
NAME="gape"
CONST="GAPE"
STRUCT="Gape"
FN="gape"

# will be replaced
TMPL_NAME="__TEMPLATE__"
TMPL_STRUCT="Tmpl"
TMPL_CONST="TMPL"
TMPL_FN="tmpl"

function replace() {
	echo "Replacing $1 with $2"
	find ./ -type f -name '*.c' -exec sed -i "s/$1/$2/g" {} \;
	find ./ -type f -name '*.h' -exec sed -i "s/$1/$2/g" {} \;
	find ./ -type f -name '*.md' -exec sed -i "s/$1/$2/g" {} \;
	find ./ -type f -name '*.lua' -exec sed -i "s/$1/$2/g" {} \;
}

replace $TMPL_NAME $NAME
replace $TMPL_CONST $CONST
replace $TMPL_STRUCT $STRUCT
replace $TMPL_FN $FN

mv "src/lib$TMPL_NAME" "src/lib$NAME"
mv "include/lib$TMPL_NAME" "include/lib$NAME"
mv "include/lib$NAME/$TMPL_NAME.h" "include/lib$NAME/$NAME.h"
