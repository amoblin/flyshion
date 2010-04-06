#!/bin/sh

count(){ 
	for file in *
	do 
		if [ -d $file ]; then
			if  "$file" != "autom4te.cache" ;then
				(cd $file
				count)
			fi
		else
			if echo $file | grep "[^\.gif]$" | grep "[^.png]$" | grep "[^.sh]$" > /dev/null; then 
				echo -n $file" "
			fi
		fi
	done
}
count
