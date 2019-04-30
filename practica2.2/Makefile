

cocos0 : cocos0.c winsuport.o winsuport.h
	gcc -Wall cocos0.c winsuport.o -o cocos0 -lcurses

cocos1 : cocos1.c winsuport.o winsuport.h
	gcc -Wall cocos1.c winsuport.o -o cocos1 -lcurses -lpthread


cocos2 : cocos2.c winsuport.o winsuport.h
	gcc -Wall cocos2.c winsuport.o -o cocos2 -lcurses -lpthread

cocos3 : cocos3.c winsuport2.o winsuport2.h
	gcc -Wall cocos3.c winsuport2.o memoria.o -o cocos3 -lcurses -lpthread

cocos4 : cocos4.c winsuport2.o winsuport2.h
	gcc -Wall cocos4.c winsuport2.o memoria.o semafor.o missatge.o -o cocos4 -lcurses -lpthread

fantasma3 : fantasma3.c winsuport2.o winsuport2.h
	gcc -Wall fantasma3.c winsuport2.o memoria.o -o fantasma3 -lcurses

fantasma4 : fantasma4.c winsuport2.o winsuport2.h
	gcc -Wall fantasma4.c winsuport2.o memoria.o semafor.o missatge.o -o fantasma4 -lcurses

winsuport.o : winsuport.c winsuport.h
	gcc -Wall -c winsuport.c -o winsuport.o

winsuport2.o : winsuport2.c winsuport2.h
	gcc -Wall -c winsuport2.c -o winsuport2.o

semafor.o : semafor.c semafor.h
	gcc -Wall -c semafor.c -o semafor.o

missatge.o : missatge.c missatge.h
	gcc -Wall -c missatge.c -o missatge.o

memoria.o : memoria.c memoria.h
	gcc -Wall -c memoria.c -o memoria.o

clean :
	rm winsuport.o winsuport2.o memoria.o cocos0 cocos1 cocos2 cocos3 cocos4 fantasma3 fantasma4
