#! /usr/bin/env python

# Author: Joan Jara Bosch i Pau Treig Sole
# Date: 25/20/2019
# Version: 1
# Parameters: path del directori que analitzarem
# Description: script que serveix per a analitzar els fitxers dun directori per saber si tenen uns permisos o no pel grup|usuari|altres. Nomes rep un parametre ja sigui -h per ajuda o el directori per analitzar. 

import os
import sys

def test_perms(tupla, qui, perm):
	if qui=="user":
		aux=tupla[0]
		aux=set(aux[0:4])
		if perm in aux:
			print("El usuari te permisos de ",perm," en el fitxer ",tupla[1])
	elif qui=="group":
		aux=tupla[0]
		aux=set(aux[4:7])
		if perm in aux:
			print("El grup te permisos de ",perm," en el fitxer ",tupla[1])
	elif qui=="others":
		aux=tupla[0]
		aux=set(aux[7:10])
		if perm in aux:
			print("Els altres te permisos de ",perm," en el fitxer ",tupla[1])

def recursiuDirectori(directori):
	tupla=[]
	fitxers=os.popen('ls -l '+directori).read()
	fitxers=fitxers.split("\n")
	i=0
	for fitxer in fitxers:
		if i>=1:
			if fitxer !="":
				dades=fitxer.replace("  ", " ").replace("  ", " ").replace("  ", " ").split(" ")
				
				if len(dades)>9:
					valor=""
					j=8
					while j<len(dades):
						if j==8:
							valor=dades[j]
						elif j<len(dades):
							valor=valor+" "+dades[j]
						j=j+1
					valor=valor.replace("\n","")
				else:
					valor=dades[-1].replace("\n","")

				auxiliar=dades[0]
				auxiliar=set(auxiliar[0:4])
				if "d" in auxiliar:
					recursiuDirectori(directori+valor+"/")
				else:
					tupla.append(dades[0])
					tupla.append(directori+valor)
		i=i+1
	i=0
	while i<len(tupla):
		test_perms(tupla[i:i+2],"others","x")
		i=i+2


if len(sys.argv)== 2:
	if sys.argv[1]=="-h":
		print("Aquest script necessita un parametre, un path del directori que voldrem analitzar")
	else:
		directori=sys.argv[1]
		if os.path.exists(directori):
			if directori[-1]=="/":
				recursiuDirectori(directori)
			else:
				recursiuDirectori(directori+"/")
		else:
			print("ERROR: El directori no existeix")
else:
	print("Aquest script nomes accepta 1 parametre (-h o el path del directori que volem analitzar)")
