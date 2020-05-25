#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef struct FRUIT_STRUCT
{
    char* name;
} fruit_T;

fruit_T* init_fruit(const char* name)
{
    fruit_T* fruit = calloc(1, sizeof(struct FRUIT_STRUCT));
    fruit->name = calloc(strlen(name) + 1, sizeof(char));
    strcpy(fruit->name, name);

    return fruit;
}

void fruit_serialize(fruit_T* fruit, FILE* fp)
{
    size_t len = strlen(fruit->name) + 1;
    fwrite(&len, sizeof(len), 1, fp);  // serialize length of string
    fwrite(fruit->name, 1, len, fp);  // serialize actual string
}

typedef struct FRUIT_LIST_STRUCT
{
    fruit_T** fruits;
    size_t size;
} fruit_list_T;

fruit_list_T* init_fruit_list()
{
    fruit_list_T* fruit_list = calloc(1, sizeof(struct FRUIT_LIST_STRUCT));
    fruit_list->fruits = (void*)0;
    fruit_list->size = 0;

    return fruit_list;
}

void fruit_list_add_fruit(fruit_list_T* list, fruit_T* fruit)
{
    list->size += 1;

    if (list->fruits == (void*)0)
        list->fruits = calloc(1, sizeof(struct FRUIT_STRUCT*));
    else
        list->fruits = realloc(list->fruits, list->size * sizeof(struct FRUIT_STRUCT*));

    list->fruits[list->size-1] = fruit;
}

void free_fruit_list(fruit_list_T* list)
{
    for (int i = 0; i < list->size; i++)
    {
        free(list->fruits[i]->name);
        free(list->fruits[i]);
    }

    free(list->fruits);
    free(list);
}

int main(int argc, char* argv[])
{
    fruit_list_T* list = init_fruit_list();

    fruit_T* apple = init_fruit("apple");
    fruit_T* pear = init_fruit("pear");
    fruit_T* banana = init_fruit("banana");

    fruit_list_add_fruit(list, apple);
    fruit_list_add_fruit(list, pear);
    fruit_list_add_fruit(list, banana);

    const char filename[] = "list.dat";

    // Write "list" to file.
    FILE *fp = fopen(filename, "wb");
    fwrite(&list->size, sizeof(int), 1, fp);

    for (int i = 0; i < list->size; i++)
        fruit_serialize(list->fruits[i], fp);

    fclose(fp);

    free_fruit_list(list); 

    fruit_list_T* loaded_list = init_fruit_list();

    // Load "list" from file.
    FILE *fp2 = fopen(filename, "rb");
    size_t final_size;
    fread(&final_size, sizeof(int), 1, fp2);
    
    for (int i = 0; i < final_size; i++)
    {
        // fetch length of string
        size_t len;
        fread(&len, sizeof(len), 1, fp2);
        
        // allocate memory for string
        char* name = malloc(len);

        // read & copy string
        fread(name, len, 1, fp2);

        // add fruit to list
        fruit_list_add_fruit(loaded_list, init_fruit(name));
        free(name);
    }

    fclose(fp2);

    // print all fruits in loaded list
    for (int i = 0; i < loaded_list->size; i++)
        printf("%s\n", loaded_list->fruits[i]->name);

    free_fruit_list(loaded_list);

    return 0;
}
