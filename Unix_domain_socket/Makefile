TARGET: server client
serialize.o:De_serialization/serialize.c
	gcc  -c -I De_serialization De_serialization/serialize.c -o De_serialization/serialize.o
sll.o:sll/sll1.c
	gcc  -c -I sll sll/sll1.c -o sll/sll.o
client.o:client.c
	gcc -c -g client.c -o client.o
server.o:server.c
	gcc -c -g server.c -o server.o 
client:client.o De_serialization/serialize.o sll/sll.o
	gcc -g -I De_serialization -I sll  De_serialization/serialize.o sll/sll.o client.o -o client
server:server.o De_serialization/serialize.o sll/sll.o
	gcc -g -I De_serialization -I sll De_serialization/serialize.o sll/sll.o server.o -o server
clean:
	rm client
	rm server
