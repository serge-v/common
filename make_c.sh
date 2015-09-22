#!/bin/bash

[ -z $3 ] && { echo usage: make_c.sh FILE HDR SRC ; exit ; }

name=$(basename $1)
name=${name/\./_}

(
	echo "extern const char ${name}[];"
	echo "extern const size_t ${name}_size;"
) > $2

(
	echo "#include <unistd.h>"
	echo
	echo "const char ${name}[] = \"\""
	sed 's/\\/\\\\/g; s/\"/\\"/g; s/^/\"/; s/$/\\n"/' $1
	echo '"";'
	echo
	echo "const size_t ${name}_size =" `wc $1 | awk '{ print \$3 }'`";"

) > $3
