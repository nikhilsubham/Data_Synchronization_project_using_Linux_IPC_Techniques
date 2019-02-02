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
#include <signal.h>

#define SOCKET_NAME "/tmp/DemoSocket"
#define BUFFER_SIZE 128

#define MAX_CLIENT_SUPPORTED    32

/*application specific structure*/
typedef struct person_{

    char name[32];
    int age;
    int weight;
} person_t;

typedef struct Mac_address{

   char address[60];
} M_addre;


void serialize_struct_Node(struct Node*obj, ser_buff_t *b);
void serialize_dll_t(dll_t * obj, ser_buff_t *b);

struct Node* de_serialize_struct_Node(ser_buff_t *b);
dll_t* de_serialize_dll_t(ser_buff_t *b);

void serialize_void(void* obj, ser_buff_t *b);
person_t* de_serialize_void(ser_buff_t *b);

void serialize_dll_t_mac(dll_t * obj, ser_buff_t *b);
void serialize_struct_Node_mac(struct Node*obj, ser_buff_t *b);

dll_t* de_serialize_dll_t_mac(ser_buff_t *b);
struct Node* de_serialize_struct_Node_mac(ser_buff_t *b);

void serialize_void_mac(void* obj1, ser_buff_t *b);
M_addre* de_serialize_void_mac(ser_buff_t *b);


static void
print_person_details(person_t *person){

    printf("Name = %s\t",   person->name);
    printf("Age = %d\t",    person->age);
    printf("weight = %d\t", person->weight);
    printf("\n");
}

static void print_person_db(dll_t* person_db) 
{
    if(!person_db || !person_db->head)
     { 
        printf("\nEntire structure is deleted\n");
        printf("*********************************************************\n");
        return;
     }
    
    struct Node *node = person_db->head;
    person_t *data = NULL;
  
     while (node != NULL) { 
        data = node->data;
        print_person_details(data);
        node = node->next; 
    } 
}


static void print_Mac_address(dll_t* Mac) 
{
    if(!Mac || !Mac->head) 
      {
         printf("\nEntire structure is deleted\n");
         printf("*********************************************************\n");
         return;
      }
    struct Node *node = Mac->head;
    M_addre *data = NULL;
  
     while (node != NULL) { 
        data = node->data;
        printf("%s\n",data->address);
        node = node->next; 
    } 
}


struct Node* find_person_db(dll_t* person_db, char*name) 
{
    if(!person_db || !person_db->head) return;
    
    struct Node *node = person_db->head;
    person_t *data = NULL;
   
     while (node != NULL) { 
        data = node->data;
        if(!strncmp(data->name, name,sizeof(name)))
            return node;
        node = node->next; 
    } 
    return NULL;
}

struct Node* find_Mac(dll_t* person_db, char*name) 
{
    if(!person_db || !person_db->head) 
       return;
      
    struct Node *node = person_db->head;
    M_addre *data = NULL;
   
     while (node != NULL) { 
        data = node->data;
        if(!strncmp(data->address, name,sizeof(name)))
            return node;
        node = node->next; 
    } 
    return NULL;
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
    de_serialize_data((char *)&obj->age, b, sizeof(int));
    de_serialize_data((char *)&obj->weight, b, sizeof(int));
    return obj;
}



void serialize_dll_t_mac(dll_t * obj, ser_buff_t *b)
{
  SENTINEL_INSERTION_CODE(obj,b);
  serialize_struct_Node_mac(obj->head, b);
}

void serialize_struct_Node_mac(struct Node*obj, ser_buff_t *b)
{
   SENTINEL_INSERTION_CODE(obj,b);
   serialize_void_mac(obj->data,b);
   serialize_struct_Node_mac(obj->next,b);
}


dll_t* de_serialize_dll_t_mac(ser_buff_t *b)
{
  SENTINEL_DETECTION_CODE(b);
  dll_t* obj = calloc(1, sizeof(dll_t));
  obj->head = de_serialize_struct_Node_mac(b);
  return obj;
}


struct Node* de_serialize_struct_Node_mac(ser_buff_t *b)
{
  SENTINEL_DETECTION_CODE(b);
  struct Node*obj = calloc(1,sizeof(struct Node));
  obj->data = de_serialize_void_mac(b);
  obj->next = de_serialize_struct_Node_mac(b);
  return obj;
}


void serialize_void_mac(void* obj1, ser_buff_t *b)
{
    int loop_var = 0;
    unsigned int sentinel = 0xFFFFFFFF;
    M_addre*obj = (M_addre*)obj1;
    
    SENTINEL_INSERTION_CODE(obj,b);
    serialize_data(b, (char *)obj->address, 60);
}


M_addre* de_serialize_void_mac(ser_buff_t *b)
{
   int loop_var = 0;
    unsigned int sentinel = 0xFFFFFFFF;
    SENTINEL_DETECTION_CODE(b);

    M_addre *obj =  calloc(1, sizeof(M_addre)); 
    de_serialize_data((char *)obj->address, b, 60);
    return obj;
}



int monitored_fd_set[MAX_CLIENT_SUPPORTED];
int process_fd_ids[MAX_CLIENT_SUPPORTED];
int client_result[MAX_CLIENT_SUPPORTED] = {0};


static void intitiaze_monitor_fd_set(){

    int i = 0;
    for(; i < MAX_CLIENT_SUPPORTED; i++)
        {
          monitored_fd_set[i] = -1;
          process_fd_ids[i] = -1;
        }
}


static void add_to_monitored_fd_set(int skt_fd){

    int i = 0;
    for(; i < MAX_CLIENT_SUPPORTED; i++){

        if(monitored_fd_set[i] != -1)
            continue;
        monitored_fd_set[i] = skt_fd;
        break;
    }
}

static void add_to_process_fd_ids(int process_id){

    int i = 0;
    for(; i < MAX_CLIENT_SUPPORTED; i++){

        if(process_fd_ids[i] != -1)
            continue;
        process_fd_ids[i]= process_id;
        break;
    }
}


/*Remove the FD from monitored_fd_set array*/
static void
remove_from_monitored_fd_set(int skt_fd){

    int i = 0;
    for(; i < MAX_CLIENT_SUPPORTED; i++){

        if(monitored_fd_set[i] != skt_fd)
            continue;

        monitored_fd_set[i] = -1;
        break;
    }
}

/* Clone all the FDs in monitored_fd_set array into fd_set Data structure*/
static void
refresh_fd_set(fd_set *fd_set_ptr){

    FD_ZERO(fd_set_ptr);
    int i = 0;
    for(; i < MAX_CLIENT_SUPPORTED; i++){
        if(monitored_fd_set[i] != -1){
            FD_SET(monitored_fd_set[i], fd_set_ptr);
        }
    }
}



static int
get_max_fd(){

    int i = 0;
    int max = -1;

    for(; i < MAX_CLIENT_SUPPORTED; i++){
        if(monitored_fd_set[i] > max)
            max = monitored_fd_set[i];
    }
    return max;
}

static void 
send_signals(){
  
   int i = 0;
    for(; i < MAX_CLIENT_SUPPORTED; i++){
        if(process_fd_ids[i] != -1){

            kill(process_fd_ids[i], SIGUSR1);
        }
    }
}



int
main(int argc, char *argv[])
{
    struct sockaddr_un name;
    int number= 0;
    char name1[32] = "\0";
    int age,weight =0;
    char address1[60]="\0";
    char *key, *intro, ipaddress[50];

    person_t *person1 = calloc(1, sizeof(person_t));
    strncpy(person1->name, "Abhishek", strlen("Abhishek"));
    person1->age = 31;
    person1->weight = 75;

    person_t *person2 = calloc(1, sizeof(person_t));
    strncpy(person2->name, "Joseph", strlen("Joseph"));
    person2->age = 41;
    person2->weight = 70;

    
    dll_t *person_db = get_new_dll();
    At_front(&person_db->head, person1);
    At_front(&person_db->head, person2);

    M_addre *first = calloc(1, sizeof(M_addre));
    strncpy(first->address, "08:3e:8e:04:8e:08", strlen("08:3e:8e:04:8e:08"));
    key = first->address;
    intro = "192.56.89.23";
    create_and_write_shared_memory(key, intro, strlen(intro));

    M_addre *second = calloc(1, sizeof(M_addre));
    strncpy(second->address, "01:6b:5g:04:8e:08", strlen("01:6b:5g:04:8e:08"));
    key = second->address;
    intro = "125.78.90.45";
    create_and_write_shared_memory(key, intro, strlen(intro));

    dll_t *Mac = get_new_dll();
    At_front(&Mac->head, first);
    At_front(&Mac->head, second);
    
    int ret;
    int connection_socket;
    int data_socket;
    int result;
    int data;
    char buffer[BUFFER_SIZE];
    fd_set readfds;
    int comm_socket_fd, i;
    intitiaze_monitor_fd_set();
    add_to_monitored_fd_set(0);

    unlink(SOCKET_NAME);

    connection_socket = socket(AF_UNIX, SOCK_STREAM, 0);

    if (connection_socket == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    printf("Master socket created\n");

    memset(&name, 0, sizeof(struct sockaddr_un));

    name.sun_family = AF_UNIX;
    strncpy(name.sun_path, SOCKET_NAME, sizeof(name.sun_path) - 1);

    
    ret = bind(connection_socket, (const struct sockaddr *) &name,
            sizeof(struct sockaddr_un));

    if (ret == -1) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    printf("bind() call succeed\n");
    

    ret = listen(connection_socket, 20);
    if (ret == -1) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    add_to_monitored_fd_set(connection_socket);
    
    ser_buff_t *Doubly_buffer =NULL;
    init_serialized_buffer(&Doubly_buffer);
    serialize_dll_t(person_db, Doubly_buffer);

    ser_buff_t *Mac_buffer =NULL;
    init_serialized_buffer(&Mac_buffer);
    serialize_dll_t_mac(Mac, Mac_buffer);

    printf("\nRouting table data\n");
    print_person_db(person_db);
    printf("\nARP table data\n");
    print_Mac_address(Mac);
    printf("\n****************************************************************************\n");

    for (;;) {

        refresh_fd_set(&readfds); 
        
        printf("\nWaiting on select() sys call\n");
        printf("Hey admin can give his choice from the below one\n");
        printf("1. Add data to the ARP table\n");
        printf("2. Delete data from the ARP table\n");
        printf("3. Add data to Mac table\n");
        printf("4. delete data from Mac table\n");
        printf("5. Send the signals to all the connected processes\n");
        
        select(get_max_fd() + 1, &readfds, NULL, NULL, NULL);
        
        
        if(FD_ISSET(connection_socket, &readfds)){

            /*Data arrives on Master socket only when new client connects with the server (that is, 'connect' call is invoked on client side)*/
            printf("New connection recieved recvd, accept the connection\n");

            data_socket = accept(connection_socket, NULL, NULL);

            if (data_socket == -1) {
                perror("accept");
                exit(EXIT_FAILURE);
            }

            printf("Connection accepted from client\n");
            
            ret = send(data_socket, Doubly_buffer->b, get_serialize_buffer_data_size(Doubly_buffer),0);
            printf("%s() : %d bytes sent\n", __FUNCTION__, ret);
            
            add_to_monitored_fd_set(data_socket);
            //usleep(500);
            memset(buffer, 0, 200);
            ret = read(data_socket, buffer, 200);
            int n = atoi(buffer);
            add_to_process_fd_ids(n);
            
            ret = write(data_socket, Mac_buffer->b, get_serialize_buffer_data_size(Mac_buffer));
            printf("%s() : %d bytes sent\n", __FUNCTION__, ret);

         }
         else if(FD_ISSET(0, &readfds)){
            memset(buffer, 0, 200);
            ret = read(0, buffer, 200);
            number =  atoi(buffer);
            
          if(number ==1)
            {
            	printf("Enter your name\n");
            	scanf("%s", name1);
            	
                printf("Enter your age\n");
            	scanf("%d", &age);
           
                printf("Enter your weight\n");
            	scanf("%d", &weight);
            	
               person_t *person3 = calloc(1, sizeof(person_t));
               //person3->name = name1;
               strcpy(person3->name, name1);
               person3->age = age;
               person3->weight = weight;
               At_front(&person_db->head, person3);
               printf("\nNew Routing table data\n");
               print_person_db(person_db);
               printf("*********************************************************\n");
            }
            else if(number ==2)
		{
                   printf("Enter your name\n");
            	   scanf("%s", name1);
            	   
                   struct Node* new_node=NULL;
                   new_node= find_person_db(person_db, name1); 
                   deleteNode(&person_db->head, new_node);
                   printf("\nNew Routing table data\n");
                   print_person_db(person_db);
                   printf("*****************************************************\n");
		}
            else if(number == 3)
                {
                  printf("Enter the Address\n");
            	  scanf("%s", address1);
                  printf("\nPlease enter corresponding IP address");
                  scanf("%s", ipaddress);
                  create_and_write_shared_memory(address1, ipaddress, strlen(ipaddress));

                  M_addre* addre = calloc(1, sizeof(M_addre));
                  strcpy(addre->address, address1);
                  At_front(&Mac->head, addre);
                  printf("\nNew ARP table data\n");
                  print_Mac_address(Mac);
                  printf("*********************************************************\n");
                }

             else if(number == 4)
                {
                  printf("Enter the Address\n");
            	  scanf("%s", address1);
                  
                  struct Node* new_node=NULL;
                  new_node= find_Mac(Mac, address1); 
                  deleteNode(&Mac->head, new_node);
                  printf("\nNew ARP table data\n");
                  print_Mac_address(Mac);
                  printf("*********************************************************\n");
                }
              else if(number ==5)
               {
                  deleteList(&Mac->head);
                  deleteList(&person_db->head);
                  init_serialized_buffer(&Doubly_buffer);
                  serialize_dll_t(person_db, Doubly_buffer);
                  init_serialized_buffer(&Mac_buffer);
                  serialize_dll_t_mac(Mac, Mac_buffer);
                  send_signals();
                  printf("\nNew Routing table data\n");
                  print_person_db(person_db);
                  printf("\nNew ARP table data\n");
                  print_Mac_address(Mac);
                  printf("*****************************************************\n");
               }
           
   
            if(number == 1 || number ==2)
            {
              memset(buffer, 0, 200);
              //ret = read(0, buffer, 200);
              strcpy(buffer, "12");
              i = 0;
              for(; i < MAX_CLIENT_SUPPORTED; i++){
                  if(monitored_fd_set[i] != -1){
                    data_socket = monitored_fd_set[i];
                      ret = write(data_socket, buffer, 200);
                  }
                }
   
              init_serialized_buffer(&Doubly_buffer);
              serialize_dll_t(person_db, Doubly_buffer);

              i = 0;
              for(; i < MAX_CLIENT_SUPPORTED; i++){
                  if(monitored_fd_set[i] != -1){
                     data_socket = monitored_fd_set[i];
                     ret = send(data_socket, Doubly_buffer->b, get_serialize_buffer_data_size(Doubly_buffer),0);
                 }
              }
           }

           if(number ==3 || number ==4)
           {
              memset(buffer, 0, 200);
              //ret = read(0, buffer, 200);
              strcpy(buffer, "34");
              i = 0;
              for(; i < MAX_CLIENT_SUPPORTED; i++){
                  if(monitored_fd_set[i] != -1){
                    data_socket = monitored_fd_set[i];
                    ret = write(data_socket, buffer, 200);
                  }
              }

              init_serialized_buffer(&Mac_buffer);
              serialize_dll_t_mac(Mac, Mac_buffer);
              i = 0;
              for(; i < MAX_CLIENT_SUPPORTED; i++){
                  if(monitored_fd_set[i] != -1){
                     data_socket = monitored_fd_set[i];
                     ret = write(data_socket, Mac_buffer->b, get_serialize_buffer_data_size(Mac_buffer));
                 }
              }

           }
        
           
        }
        else /* Data srrives on some other client FD*/
        {
          /*Find the client which has send us the data request*/
            i = 0, comm_socket_fd = -1;
            for(; i < MAX_CLIENT_SUPPORTED; i++){

                if(FD_ISSET(monitored_fd_set[i], &readfds)){
                    comm_socket_fd = monitored_fd_set[i];

                    memset(buffer, 0, BUFFER_SIZE);

                    printf("\nWaiting for data from the client\n");
                    ret = read(comm_socket_fd, buffer, BUFFER_SIZE);

                    if (ret == -1) {
                        perror("read");
                        exit(EXIT_FAILURE);
                    }

                    /* Add received summand. */
                    //memcpy(&data, buffer, sizeof(int));
                    int data = atoi(buffer);
                    printf("process %d is terminating\n",data);
                    monitored_fd_set[i] = -1;
            
                     
               }
            }
            
        }
    } /*go to select() and block*/

    
    close(connection_socket);
    remove_from_monitored_fd_set(connection_socket);
    printf("connection closed..\n");

    unlink(SOCKET_NAME);
    exit(EXIT_SUCCESS);
}
