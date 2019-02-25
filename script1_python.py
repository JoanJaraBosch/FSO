#! /usr/bin/env python

# Author: Joan Jara Bosch
# Date: 19/20/2019
# Version: 1

import os.path as path
import sys

def test_perms(tupla, qui, perm):
    if qui=="user":
        aux=tupla[0]
        aux=set(aux[0:4])
        if perm in aux:
            print("El usuari te permisos de ",perm," en el fitxer ",tupla[1])
        else:
            print("El usuari no te permisos de ",perm," en el fitxer ",tupla[1])
    elif qui=="group":
        aux=tupla[0]
        aux=set(aux[4:7])
        if perm in aux:
            print("El grup te permisos de ",perm," en el fitxer ",tupla[1])
        else:
            print("El grup no te permisos de ",perm," en el fitxer ",tupla[1])
    elif qui=="others":
        aux=tupla[0]
        aux=set(aux[7:10])
        if perm in aux:
            print("Els altres te permisos de ",perm," en el fitxer ",tupla[1])
        else:
            print("Els altres no te permisos de ",perm," en el fitxer ",tupla[1])

        

if len(sys.argv)== 4:
    if path.exists(sys.argv[1]):
        llistaPar=[]
        f=open(sys.argv[1],"r")
        i=0
        for l in f:
            if i>=1:
                dades=l.split(" ")
                llistaPar.append(dades[0])
                llistaPar.append(dades[-1].replace("\n",""))
            i=i+1
        f.close()
        i=0
        while i<len(llistaPar):
            ret=test_perms(llistaPar[i:i+2],sys.argv[2],sys.argv[3])
            i=i+2
    else:
        print("ERROR: El fitxer no existeix")
elif len(sys.argv)== 2:
    if sys.argv[1]=="-h":
        print("Aquest script necessita un fitxer per parametre, uns grups|users i uns permisos")
    else:
        print("Error: si passes nomes un paramtre ha de ser el -h")
else:
    print("Aquest script nomes accepta 1 parametre (-h) o tres parametres (nom fitxer, propietaris, permisos")
