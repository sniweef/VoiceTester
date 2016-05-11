#!/bin/bash

memMax=0
memMin=99999999
count=0

# Get the pid of vc_shell
resfile=${2:-'memusage'}
logfile=$resfile.log

log()
{
	echo "$1" >> $logfile
}

initPid()
{
	pid=`ps aux | grep $1 | head -1 | awk '{print $2}'`
	vc_pidfile=/proc/$pid/status
	log "Pid file: $vc_pidfile"
}

updateMem()
{
	if [ -f $vc_pidfile ]; then
		mem=`grep VmRSS $vc_pidfile | awk -F':' '{print $2}' | awk '{print $1}'`

		echo "$count $mem" >> $resfile
		
		if test $mem -gt $memMax; then
			log "Update memMax to $mem"
			memMax=$mem
		fi

		if test $mem -lt $memMin; then
			log "Update memMin to $mem"
			memMin=$mem
		fi
	fi
}

outputMem()
{
	echo "memMax=$memMax, memMin=$memMin"
	exit
}

main()
{
	# we truncate the log file to 0 size
	truncate $logfile --size 0
	truncate $resfile --size 0
	
	# Catch SIGINT signal, SIGINT txt isnot supported by sh
	trap outputMem SIGINT

	initPid $1

	sleepTime=${2:-'0.5'}

	while true; do 
		sleep $sleepTime
		count=$(echo "$count+$sleepTime" | bc)

		updateMem 
	done
}

echo "Begin calculating memory usage..."
echo "Usage: $0 program [outputfile interval]"

main $1 $3
