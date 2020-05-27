#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef struct FRUIT_STRUCT
{
    char* name;
} fruit_T;

/**
 * Initialize a new fruit with a name.
 */
fruit_T* init_fruit(const char* name)
{
    fruit_T* fruit = calloc(1, sizeof(struct FRUIT_STRUCT));
    fruit->name = calloc(strlen(name) + 1, sizeof(char));
    strcpy(fruit->name, name);

    return fruit;
}

/**
 * Deallocate a fruit.
 */
void free_fruit(fruit_T* fruit)
{
    free(fruit->name);
    free(fruit);
}

/**
 * Serialize a fruit into file.
 */
void fruit_serialize(fruit_T* fruit, FILE* fp)
{
    size_t len = strlen(fruit->name) + 1;
    fwrite(&len, sizeof(len), 1, fp);  // serialize length of string
    fwrite(fruit->name, 1, len, fp);  // serialize actual string
}

/**
 * Unserialize / read fruit from file.
 */
fruit_T* fruit_unserialize(FILE* fp)
{
    // fetch length of string
    size_t len;
    fread(&len, sizeof(len), 1, fp);
    
    // allocate memory for string
    char* name = malloc(len);

    // read & copy string
    fread(name, len, 1, fp);

    fruit_T* fruit = init_fruit(name);
    free(name);

    return fruit;
}

typedef struct FRUIT_LIST_STRUCT
{
    fruit_T** fruits;
    size_t size;
} fruit_list_T;

/**
 * Initialize a new fruit list.
 */
fruit_list_T* init_fruit_list()
{
    fruit_list_T* fruit_list = calloc(1, sizeof(struct FRUIT_LIST_STRUCT));
    fruit_list->fruits = (void*)0;
    fruit_list->size = 0;

    return fruit_list;
}

/**
 * Serialize a complete fruit list into a file
 */
void fruit_list_serialize(fruit_list_T* list, const char* filename)
{
    // Write "list" to file.
    FILE *fp = fopen(filename, "wb");
    fwrite(&list->size, sizeof(size_t), 1, fp);

    for (int i = 0; i < list->size; i++)
        fruit_serialize(list->fruits[i], fp);

    fclose(fp);
}

/**
 * Add a fruit to a fruit list.
 */
void fruit_list_add_fruit(fruit_list_T* list, fruit_T* fruit)
{
    list->size += 1;

    if (list->fruits == (void*)0)
        list->fruits = calloc(1, sizeof(struct FRUIT_STRUCT*));
    else
        list->fruits = realloc(list->fruits, list->size * sizeof(struct FRUIT_STRUCT*));

    list->fruits[list->size-1] = fruit;
}

/**
 * Deallocate memory for a fruit list.
 */
void free_fruit_list(fruit_list_T* list)
{
    for (int i = 0; i < list->size; i++)
        free_fruit(list->fruits[i]); 

    free(list->fruits);
    free(list);
}

fruit_list_T* fruit_list_unserialize(const char* filename)
{
    fruit_list_T* loaded_list = init_fruit_list();

    FILE *fp2 = fopen(filename, "rb");
    size_t final_size;
    fread(&final_size, sizeof(size_t), 1, fp2);
    
    for (int i = 0; i < final_size; i++)
        fruit_list_add_fruit(loaded_list, fruit_unserialize(fp2));

    fclose(fp2);

    return loaded_list;
}

int main(int argc, char* argv[])
{
    fruit_list_T* list = init_fruit_list();

    fruit_list_add_fruit(list, init_fruit("apple"));
    fruit_list_add_fruit(list, init_fruit("pear"));
    fruit_list_add_fruit(list, init_fruit("banana"));

    const char filename[] = "list.bin";

    fruit_list_serialize(list, filename);

    free_fruit_list(list); 


    // Load "list" from file.
    fruit_list_T* loaded_list = fruit_list_unserialize(filename);

    // print all fruits in loaded list
    for (int i = 0; i < loaded_list->size; i++)
        printf("%s\n", loaded_list->fruits[i]->name);

    free_fruit_list(loaded_list);

    return 0;
}
