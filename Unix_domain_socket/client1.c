#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include "sll/sll1.h"
#include "De_serialization/sentinel.h"
#include "De_serialization/serialize.h"
#include <memory.h>

#define SOCKET_NAME "/tmp/DemoSocket"
#define BUFFER_SIZE 12

typedef struct person_{

    char name[32];
    int age;
    int weight;
} person_t;


void serialize_struct_Node(struct Node*obj, ser_buff_t *b);
void serialize_dll_t(dll_t * obj, ser_buff_t *b);

struct Node* de_serialize_struct_Node(ser_buff_t *b);
dll_t* de_serialize_dll_t(ser_buff_t *b);

void serialize_void(void* obj, ser_buff_t *b);
person_t* de_serialize_void(ser_buff_t *b);


static void
print_person_details(person_t *person){

    printf("Name = %s\t",   person->name);
    printf("Age = %d\t",    person->age);
    printf("weight = %d\t", person->weight);
    printf("\n");
}

static void print_person_db(dll_t* person_db) 
{
    if(!person_db || !person_db->head) return;
    
    struct Node *node = person_db->head;
    person_t *data = NULL;
  
     while (node != NULL) { 
        data = node->data;
        print_person_details(data);
        node = node->next; 
    } 
}

void serialize_dll_t(dll_t * obj, ser_buff_t *b)
{
  SENTINEL_INSERTION_CODE(obj,b);
  serialize_struct_Node(obj->head, b);
}

void serialize_struct_Node(struct Node*obj, ser_buff_t *b)
{
   SENTINEL_INSERTION_CODE(obj,b);
   serialize_void(obj->data,b);
   serialize_struct_Node(obj->next,b);
}


dll_t* de_serialize_dll_t(ser_buff_t *b)
{
  SENTINEL_DETECTION_CODE(b);
  dll_t* obj = calloc(1, sizeof(dll_t));
  obj->head = de_serialize_struct_Node(b);
  return obj;
}


struct Node* de_serialize_struct_Node(ser_buff_t *b)
{
  SENTINEL_DETECTION_CODE(b);
  struct Node*obj = calloc(1,sizeof(struct Node));
  obj->data = de_serialize_void(b);
  obj->next = de_serialize_struct_Node(b);
  return obj;
}


void serialize_void(void* obj1, ser_buff_t *b)
{
   int loop_var = 0;
    unsigned int sentinel = 0xFFFFFFFF;
    person_t *obj = (person_t*)obj1;
    
    SENTINEL_INSERTION_CODE(obj,b);
    serialize_data(b, (char *)obj->name, 32);
    serialize_data(b, (char *)&obj->age, sizeof(int));
    serialize_data(b, (char *)&obj->weight, sizeof(int));
}


person_t* de_serialize_void(ser_buff_t *b)
{
   int loop_var = 0;
    unsigned int sentinel = 0xFFFFFFFF;
    SENTINEL_DETECTION_CODE(b);

    person_t *obj =  calloc(1, sizeof(person_t)); 
    de_serialize_data((char *)obj->name, b, 32);
    printf("%s", obj->name);
    de_serialize_data((char *)&obj->age, b, sizeof(int));
    de_serialize_data((char *)&obj->weight, b, sizeof(int));
    return obj;
}

int
main(int argc, char *argv[])
{
    struct sockaddr_un addr;
    int i;
    int ret;
    int data_socket;
    char buffer[BUFFER_SIZE];

    data_socket = socket(AF_UNIX, SOCK_STREAM, 0);

    if (data_socket == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    memset(&addr, 0, sizeof(struct sockaddr_un));

    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_NAME, sizeof(addr.sun_path) - 1);

    ret = connect (data_socket, (const struct sockaddr *) &addr,
            sizeof(struct sockaddr_un));

    if (ret == -1) {
        fprintf(stderr, "The server is down.\n");
        exit(EXIT_FAILURE);
    }
    
    ser_buff_t *Doubly_coming_buffer= NULL;
    init_serialized_buffer(&Doubly_coming_buffer);
    reset_serialize_buffer(Doubly_coming_buffer);
    ret = recv(data_socket, Doubly_coming_buffer->b, get_serialize_buffer_length(Doubly_coming_buffer),0);
    
    /* Recieving machine has recieved the serialized buffer, Lets derialize it and reconstruct the object from it*/
        
        dll_t *person_db1 = de_serialize_dll_t(Doubly_coming_buffer);
        printf("after serialiation\n\n");
        print_person_db(person_db1);

    
    do{
        
        init_serialized_buffer(&Doubly_coming_buffer);
        reset_serialize_buffer(Doubly_coming_buffer);
        ret = recv(data_socket, Doubly_coming_buffer->b, get_serialize_buffer_length(Doubly_coming_buffer),0);

        dll_t *person_db1 = de_serialize_dll_t(Doubly_coming_buffer);
        printf("after New data given\n\n");
        print_person_db(person_db1);
        printf("No of bytes sent = %d, data sent = %d\n", ret, i); 
    } while(1);

    /* Request result. */
    
    memset(buffer, 0, BUFFER_SIZE);
    strncpy (buffer, "RES", strlen("RES"));
    buffer[strlen(buffer)] = '\0';
    printf("buffer = %s\n", buffer);

    ret = write(data_socket, buffer, strlen(buffer));
    if (ret == -1) {
        perror("write");
        exit(EXIT_FAILURE);
    }

    /* Receive result. */
    memset(buffer, 0, BUFFER_SIZE);
    
    ret = read(data_socket, buffer, BUFFER_SIZE);
    if (ret == -1) {
        perror("read");
        exit(EXIT_FAILURE);
    }

    /* Ensure buffer is 0-terminated. */

    buffer[BUFFER_SIZE - 1] = 0;

    printf("Result = %s\n", buffer);

    /* Close socket. */

    close(data_socket);

    exit(EXIT_SUCCESS);
}
