struct Node { 
    void* data; 
    struct Node* next; // Pointer to next node in DLL 
};

typedef struct dll_{
    struct Node *head;
} dll_t;


/* Public Function declaration to create and return a new empty doubly linked list*/
dll_t * get_new_dll();

/*pulic function to know if the doubly linked list is empty or not*/
//int is_dll_empty(struct Node** head_ref);

/*declaration for element addition in douly linked list.*/
void At_front(struct Node** head_ref, void*app_data);
void insertAfter(struct Node* prev_node, void*new_data);
void At_end(struct Node** head_ref, void* new_data); 

  


  
