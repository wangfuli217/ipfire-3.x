#!/bin/sh

MODULES=$(/bin/kudzu -qps  -t 30 -c NETWORK | grep driver | cut -d ' ' -f 2 | sort)

if [ "$1" == "count" ]; then
	/bin/kudzu -qps  -t 30 -c NETWORK | grep driver | wc -l | awk '{ print $1 }' > /tmp/drivercount
	exit 0
else
	NUMBER=$1
fi

if [ "$NUMBER" ]; then
	echo "$(echo $MODULES | grep -n $NUMBER | cut -c 1-2 )" > /tmp/nicdriver
else
	echo "$MODULES" > /tmp/nicdriver
fi

#  kudzu -qps -c NETWORK | egrep "desc|network.hwaddr|driver" | awk -F': ' '{print $2}' | sed -e '/..:..:..:..:..:../a\\' -e "s/$/\;/g" | tr "\n" "XX" | sed -e "s/XX/\n/g" -e "s/\;X/\;/g"
