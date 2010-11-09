#!/bin/bash
export LANG=C
echo "<?xml version='1.0'?>"
echo " <result>" 
bSuccess=true;
for ((i=1; i<=$#; i=i+2))
do
	eval "diskpath=\$$i" 
	per=`df -P|grep $diskpath| tr -s ' '|cut -d " " -f5`
	if [ "$per"x = x ]
	then
		continue
	fi
	per=${per%'%'};
	ip=$((i + 1))
	eval "disklimit=\$$ip"; 
	if [ "$per" -ge "$disklimit" ]
	then
		if [ "$bSuccess" = "true" ]
		then
			echo "<status>-1</status>"
                        echo "<level>default</level>"
                        echo "<error>"
			bSuccess=false;
		fi
		printf "disk %s does not have enough space. (used: %d%% > alarm: %d%%)\n" $diskpath $per $disklimit 
	fi
done

if [ "$bSuccess" = "true" ]
then
        echo "<status>0</status>"
        echo "<level>default</level>"
        echo "<error>success</error>"
else
	echo "</error>"
fi

echo "<record>"
echo "<head>"
df -P|grep Filesystem
echo "</head>"
for ((i=1; i<=$#; i=i+2))
do
	eval "diskpath=\$$i"; 
echo "<disk>"
        echo "<name>"
        echo "$diskpath"
        echo "</name>"
        eval "rate=\$$(expr $i + 1)";
        echo "<alarm_rate> $rate</alarm_rate>"
        echo "<info>"
	df -P|grep $diskpath
        echo "</info>"
echo "</disk>"
done
echo "</record>"
echo "</result>"
