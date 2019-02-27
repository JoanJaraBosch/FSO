#! /usr/bin/env python3

# Author: Joan Jara Bosch ,Pau Treig Sole i Victor Castillo
# Date: 25/20/2019
# Version: 1
# Parameters: path del directori que analitzarem
# Description: script que serveix per a analitzar els fitxers dun directori per saber si tenen uns permisos o no pel grup|usuari|altres. Nomes rep un parametre ja sigui -h per ajuda o el directori per analitzar. 

import os
import sys
from glob import glob

#Funcio la qual li passem el directori passsat al script i els permis que vol comprovar l'usuari amb el seu propietari ja sigui user|group|others
def recursiuDirectori(directori, user, perm):
	if perm=="r":
		permNum=0
	elif perm=="w":
		permNum=1
	else:
		permNum=2
	#utilitzem el modul glob amb mode recursiu per trobar tots els fitxers i directoris
	fitxers=glob(directori+"**", recursive=True)
	for fitxerDir in fitxers:
		#utilitzem el os.stat per agafar els permisos del fitxer|directori i el passem a binari
		permisosF=bin(os.stat(fitxerDir).st_mode & 0o777)
		if user=="user":
			permisosF=permisosF[2:5]
		elif user =="group":
			permisosF=permisosF[5:8]
		else:
			permisosF=permisosF[8:11]

		if permisosF[permNum]=="1":
			if os.path.isdir(fitxerDir):
				print(fitxerDir+"\tDirectori\t"+user+"\t"+perm)
			else:
				print(fitxerDir+"\tFitxer\t"+user+"\t"+perm)



if len(sys.argv)== 2:
	if sys.argv[1]=="-h":
		print("Aquest script necessita un parametre, un path del directori que voldrem analitzar")
	else:
		directori=sys.argv[1]
		if directori[-1]!="/":
			directori=directori+"/"
		if os.path.isdir(directori):
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
					
			recursiuDirectori(directori, user, perm)
		else:
			print("ERROR: El directori no existeix")
else:
	print("Aquest script nomes accepta 1 parametre (-h o el path del directori que volem analitzar)")
