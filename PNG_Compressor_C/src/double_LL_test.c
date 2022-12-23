#include <stddef.h> // required for NULL
#include <stdio.h>
#include <stdlib.h>
#include "LinkedListElements.h"


#define APPEND(linkedList,NewString) list_append((list_double_node**)&linkedList,&(NEW_STRING_NODE(NewString))->node)

int main(int argc,char** args)
{
    string_item_double* my_linked_list = NULL;
    
  
    APPEND(my_linked_list,"Hello World");
    APPEND(my_linked_list,"LET'S GOOOOOOOO");
    APPEND(my_linked_list,"Test01");

    string_item_double* newString =  string_item_new("Test002");
    list_append((list_double_node**) &my_linked_list, &newString->node);
    
    string_item_double* node_to_add_after_another = string_item_new("ADD THIS AFTER TEST 02");
    add_after((list_double_node**) &my_linked_list, &node_to_add_after_another->node, &newString->node);


    string_item_double* node_to_add_before_another = string_item_new("ADD THIS BEFORE TEST 02");
    add_before((list_double_node**) &my_linked_list, &node_to_add_before_another->node, &newString->node);

    
    //remove_node((list_double_node**) &my_linked_list,&newString->node);

    APPEND(my_linked_list,"Last Item of the LinkedList");
    

    string_item_double *string_item = my_linked_list;
    while (string_item)
    {
        printf("%s\n", string_item->string);
        string_item = (struct string_item_double *)string_item->node.next;
    }
    printf("\nProgram executed succesfully");
    return 0;
}