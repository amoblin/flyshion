#!/bin/sh

number=0
count()
{
	for file in *
	do
		if [ -d $file ]; then
			cd $file
			count
			cd ..
		elif echo $file | grep "\.[ch]$" > /dev/null; then
			pot=$(wc -l $file | cut -d" " -f1)
			let number+=$pot
			echo $pot $file
		fi
	done
}
count
echo line total:$number
