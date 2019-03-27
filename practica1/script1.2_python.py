#! /usr/bin/env python3

# Author: Joan Jara Bosch, Pau Treig Sole i Victor Castillo
# Date: 25/2/2019
# Version: 1
# Parameters: path del directori que analitzarem
# Description: script que serveix per analitzar els fitxers dun directori per saber si tenen els permisos que lusuari introduira. Nomes rep un parametre ja sigui -h per ajuda o el directori per analitzar. 

import os
import sys
from glob import glob

#Funcio a la qual li passem per parametre el directori, lambit i el tipus del permis. Busca en el directori tots els fitxers i analitza si els seus permisos cumpleixen els introduits.
def recursiuDirectori(directori, user, perm):
	if perm=="r":
		permNum=0
	elif perm=="w":
		permNum=1
	else:
		permNum=2
	fitxers=glob(directori+"**", recursive=True)    #utilitzem el modul glob en mode recursiu per trobar tots els fitxers i directoris
	for fitxerDir in fitxers:
		permisosF=bin(os.stat(fitxerDir).st_mode & 0o777)    #utilitzem el os.stat per agafar els permisos del fitxer|directori i el passem a binari
		try:
			if user=="user":
				permisosF=permisosF[2:5]    #retallem lstring amb els permisos segons el que ens interessi
			elif user =="group":
				permisosF=permisosF[5:8]
			else:
				permisosF=permisosF[8:11]

			if permisosF[permNum]=="1":
				if os.path.isdir(fitxerDir):    #si es un directori printem el primer argument, si no ho es printem el segon
					print(fitxerDir+"\tDirectori\t"+user+"\t"+perm)
				else:
					print(fitxerDir+"\tFitxer\t"+user+"\t"+perm)
		except IndexError:    #capturem aquesta excepcio en cas de que el fitxer no tingui el permis de lectura dusuari, ja que no podrem agafar els seus permisos amb la comanda stat
			pass



if len(sys.argv)== 2:
	if sys.argv[1]=="-h":    #proporcionem la comanda '-h' per a printar ajuda sobre els parametres a introduir
		print("Aquest script necessita un parametre, un path del directori que voldrem analitzar")
	else:
		directori=sys.argv[1]
		if directori[-1]!="/":    #en cas que lusuari no posi la barra del final, se li afegeix
			directori=directori+"/"
		if os.path.isdir(directori):    #si el directori existeix, passem a preguntar lambit i tipus del permis. Si estan mal introduits els tornara a preguntar
			user="joan"
			perm="cap"
			while(user!="others" and user!="group" and user!="user"):
				print("Per a qui vols mirar els permisos(user|group|others)?")
				user=input()
				user=user.lower()
				while(perm!="x" and perm!="w" and perm!="r"):
					print("Quin permis vols analitzar en els fitxers (x|r|w)?")
					perm=input()
					perm=perm.lower()
					
			recursiuDirectori(directori, user, perm)    #un cop els permisos estan ben introduits cridem a la funcio que fara la feina
		else:
			print("ERROR: El directori no existeix")
else:
	print("Aquest script nomes accepta 1 parametre (-h o el path del directori que volem analitzar)")
