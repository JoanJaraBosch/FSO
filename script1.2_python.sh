#! /bin/bash

# Author: Joan Jara Bosch ,Pau Treig Sole i Victor Castillo
# Date: 25/20/2019
# Version: 1
# Parameters: path del directori que analitzarem
# Description: script que serveix per a analitzar els fitxers dun directori per saber si tenen uns permisos o no pel grup|usuari|altres. Nomes rep un parametre ja sigui -h per ajuda o el directori per analitzar. 

if [ "$#" -eq 1 ];then
	if [ "$1" = "-h" ];then
		echo "Aquest script necessita un parametre, un path del directori que voldrem analitzar"
	else
		if [ -e "$1" ] && [ -d "$1" ];then
			user="joan"
			
			
			while [ "$user" != "others" ] && [ "$user" != "group" ] && [ "$user" != "user" ];do
					perm="cap"
					echo "Per a qui vols mirar els permisos(user|group|others)?"
					read user
					user=$(echo "$user" | tr '[:upper:]' '[:lower:]')
				while [ "$perm" != "r" ] && [ "$perm" != "w" ] && [ "$perm" != "x" ];do
					echo "Quin permis vols analitzar en els fitxers (x|r|w)?" 
					read perm
					perm=$(echo "$perm" | tr '[:upper:]' '[:lower:]')
				done
			done

			if [ "$perm" = "r" ];then
				permNum=0
			elif [ "$perm" = "w" ];then
				permNum=1
			else
				permNum=2
			fi

			fitxers=$(find "$1" 2> /dev/null)
			IFS=$'\n'
			for fitxerDir in $fitxers 
			do
				if [ $fitxerDir != "" ];then
					permisosF=$(stat -c %A "$fitxerDir" | tr - "0" | tr "[:alpha:]" "1")
					if [ "$user" = "user" ];then
						permisosF=$(echo "$permisosF" | cut -c 1-4)
					elif [ "$user" = "group" ];then
						permisosF=$(echo "$permisosF" | cut -c 4-7)
					else
						permisosF=$(echo "$permisosF" | cut -c 7-10)
					fi
					permisosF=$(echo "$permisosF" | cut -c $permNum)
					if [ "$permisosF" -eq "1" ];then
						if [ -d "$fitxerDir" ];then
							echo -e "$fitxerDir\tDirectori\t$user\t$perm"
						else
							echo -e "$fitxerDir\tFitxer\t$user\t$perm"
						fi
					fi
				fi
			done
			unset IFS
		else
			echo "ERROR: El directori no existeix"
		fi
	fi
else
	echo "Aquest script nomes accepta 1 parametre (-h o el path del directori que volem analitzar)"
fi
