TARGET: server1 client
serialize.o:De_serialization/serialize.c
	gcc  -c -I De_serialization De_serialization/serialize.c -o De_serialization/serialize.o
sll.o:sll/sll1.c
	gcc  -c -I sll sll/sll1.c -o sll/sll.o
client.o:client.c
	gcc -c -g client.c -o client.o
server1.o:server1.c
	gcc -c -g server1.c -o server1.o 
client:client.o De_serialization/serialize.o sll/sll.o
	gcc -g -I De_serialization -I sll  De_serialization/serialize.o sll/sll.o client.o -o client
server1:server1.o De_serialization/serialize.o sll/sll.o
	gcc -g -I De_serialization -I sll De_serialization/serialize.o sll/sll.o server1.o -o server1
clean:
	rm client
	rm server
