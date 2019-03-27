#! /bin/bash

# Author: Joan Jara Bosch, Pau Treig Sole i Victor Castillo
# Date: 25/2/2019
# Version: 1
# Parameters: path del directori que analitzarem
# Description: script en bash que serveix per analitzar els fitxers dun directori per saber si tenen els permisos que lusuari introduira. Nomes rep un parametre ja sigui -h per ajuda o el directori per analitzar. 

#condicio per a comprovar que el numero de parametres es 1
if [ $# -ne 1 ];
then	echo 'Numero de parametres incorrecte. Us: Directori'
	exit 1
fi

#condicio per a comprovar que els parametres estan introduits correctament
echo 'Introdueix l"ambit (user|group|others): '
read qui
echo 'Introdueix el tipus (r|w|x): '
read perm
qui=$(echo "$qui" | tr '[:upper:]' '[:lower:]')    #passem els parametres a minuscules
perm=$(echo "$perm" | tr '[:upper:]' '[:lower:]')
if [ "$qui" != 'user' ] && [ "$qui" != 'group' ] && [ "$qui" != 'others' ];    #comprovem que lambit esta ben introduit
then	echo -e 'Error: Parametres incorrectes.\nAmbit: (user|group|others)\nTipus: (r|w|x)'
	exit 1
fi
if [ "$perm" != 'r' ] && [ "$perm" != 'w' ] && [ "$perm" != 'x' ];    #comprovem que el tipus del permis esta ben introduit
then	echo -e 'Error: Parametres incorrectes.\nAmbit: (user|group|others)\nTipus: (r|w|x)'
	exit 1
fi

#comprovem que el directori existeixi. En cas de que l'usuari introdueixi el directori sense la barra final, se la afegeix
if [ -d "$1" ];
then	echo 'Directori correcte'
	lastchar=$(echo -n $1 | tail -c 1)
	if [ $lastchar != "/" ];
	then	set "$1/"
	fi
else	echo 'Directori incorrecte'
	exit 1
fi

#busquem tots els directoris i fitxers que pengen del directori introduit
IFS=$'\n'
for f in $(find $1 2> /dev/null) 
do
	permisos=$(stat -c %A "$f")    #guardem els permisos del fitxer amb la comanda stat
	if [ "$qui" == "user" ];
	then	permisos=$(echo $permisos | cut -c2-4)    #retallem el substring que ens interessa
	elif [ "$qui" == "group" ];
	then	permisos=$(echo $permisos | cut -c5-7)    #retallem el substring que ens interessa
	else	permisos=$(echo $permisos | cut -c8-10)    #retallem el substring que ens interessa
	fi
	caracter=$(echo $permisos | grep -c "$perm")    #amb la comanda grep -c sabrem si el tipus de permis apareix 1 vegada o cap vegada
	if [ $caracter -eq 1 ];
	then	if [ -d "$f" ];    #si es directori, printara el primer argument. Si es fitxer printara el segon
		then	echo -e "$f\tDirectori\t$qui\t$perm"
		else	echo -e "$f\tFitxer\t$qui\t$perm"
		fi
	fi
done
