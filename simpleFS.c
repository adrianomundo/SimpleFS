#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/******************************************************
 *          MACRO DEFINITIONS                         *
 ******************************************************/

#define MAX 3000

/******************************************************
 *          TYPE DEFINITIONS                         *
 ******************************************************/

typedef struct directory {
    char *name;
    char *path;
    char *contenuto;
    int flag;                                   //0 cartella 1 file
    struct directory *fratello;
    struct directory *figlio;
    struct directory *padre;
} dir;

typedef dir* dir_ptr;

/******************************************************
 *          HELPERS PROTOTYPES                        *
 ******************************************************/

char *scan(char *string);
dir_ptr new_root(void);
void bye_root(dir_ptr root);
char *gnu_basename(char *path);
char *gnu_dirname(char *path);
dir_ptr get_node_ptr(char *path, dir_ptr current);

/******************************************************
 *          FUNCTIONS PROTOTYPES                      *
 ******************************************************/

void create_dir(dir_ptr root, char *path);
void create(dir_ptr root, char *path);
void delete(dir_ptr root, char *path);
void delete_r(dir_ptr root, char *path);
void do_write(dir_ptr root, char *path, char *contenuto);
void do_read(dir_ptr root, char *path);
void find(dir_ptr root, char *risorsa);

/******************************************************
 *          GLOBAL VARIABLES                          *
 ******************************************************/


int found = 0;

/******************************************************
 *          MAIN                                      *
 ******************************************************/

int main(void)
{
    char line[MAX];
    char *str;
    char *command;
    char *path;
    char *saveptr;
    char *risorsa;
    
    dir_ptr root = new_root();
    str = scan(line);
    
    while (strcmp(str, "exit") != 0)
    {
        command = strtok_r(str, " ", &saveptr);
        
        if(strcmp(command, "create_dir") == 0) {
            char *tp = strtok_r(NULL, " ", &saveptr);
            path = malloc((strlen(tp) + 1) * sizeof(char));
            strcpy(path, tp);
            create_dir(root, path);
        }
        else if(strcmp(command, "create") == 0) {
            char *tp = strtok_r(NULL, " ", &saveptr);
            path = malloc((strlen(tp) + 1) * sizeof(char));
            strcpy(path, tp);
            create(root, path);
        }
        else if(strcmp(command, "delete") == 0) {
            char *tp = strtok_r(NULL, " ", &saveptr);
            path = malloc((strlen(tp) + 1) * sizeof(char));
            strcpy(path, tp);
            delete(root, path);
        }
        else if(strcmp(command, "delete_r") == 0) {
            char *tp = strtok_r(NULL, " ", &saveptr);
            path = malloc((strlen(tp) + 1) * sizeof(char));
            strcpy(path, tp);
            delete_r(root, path);
        }
        else if(strcmp(command, "write") == 0) {
            char *tp = strtok_r(NULL, " ", &saveptr);
            path = malloc((strlen(tp) + 1) * sizeof(char));
            strcpy(path, tp);
            char *tz = strtok_r(NULL, "\"", &saveptr);
            char *contenuto = malloc((strlen(tz) + 1) * sizeof(char));
            strcpy(contenuto, tz);
            do_write(root, path, contenuto);
        }
        else if(strcmp(command, "read") == 0)  {
            char *tp = strtok_r(NULL, " ", &saveptr);
            path = malloc((strlen(tp) + 1) * sizeof(char));
            strcpy(path, tp);
            do_read(root, path);
        }
        else if(strcmp(command, "find") == 0){
            char *tp = strtok_r(NULL, " ", &saveptr);
            risorsa = malloc((strlen(tp) + 1) * sizeof(char));
            strcpy(risorsa, tp);
            find(root, risorsa);
            if (found == 0) printf("no\n");                                     //risorsa non trovata
            found = 0;
        
        }
        str = scan(line);
    }
    return 0;
}

/******************************************************
 *          FUNCTIONS DEFINITIONS                       *
 ******************************************************/

dir_ptr get_node_ptr(char *path, dir_ptr root) {
    
    char *saveptr;
    int exist = 0;
    dir_ptr current = NULL;
    current = root;
    
    if (path[0] == '/')
    {
        if ((strcmp(path, current->name)) == 0) {            //path is root
            return root;
        }
        
        char *next = strtok_r(path, "/", &saveptr);
        char *first_name = malloc((strlen(next) + 1) * sizeof(char));
        strcpy(first_name, next);
        
        if (current->figlio != NULL)                    // root has children
        {
            if ((strcmp(current->figlio->name, first_name)) == 0)
            {
                current = current->figlio;
                exist = 1;
            }
            else
            {
                dir_ptr sibling = current->figlio;
                while (sibling->fratello != NULL)
                {
                    if ((strcmp(sibling->fratello->name, first_name)) == 0) {
                        current = sibling->fratello;
                        exist = 1;
                        break;
                    }
                    sibling = sibling->fratello;
                }
            }
            
            if (exist == 0) {                                           //cartella iniziale non trovata
                printf("no\n");
                return NULL;
                
            }
        }
        
        while ((next = strtok_r(NULL, "/", &saveptr)) != NULL)
        {
            char *other_name = malloc((strlen(next) + 1) * sizeof(char));
            strcpy(other_name, next);
            exist = 0;
            if (current->figlio != NULL)
            {
                if ((strcmp(current->figlio->name, other_name)) == 0)
                {
                    current = current->figlio;
                    exist = 1;
                }
                else
                {
                    dir_ptr sibling = current->figlio;
                    while (sibling->fratello != NULL)
                    {
                        if ((strcmp(sibling->fratello->name, other_name)) == 0) {
                            current = sibling->fratello;
                            exist = 1;
                            break;
                        }
                        sibling = sibling->fratello;
                    }
                }
                
            }
            if (exist == 0) {                             //cartella intermedia non trovata
                printf("no\n");
                return NULL;
                
            }
        }
    }
    return current;
}


void create(dir_ptr root, char *path) {
    
    dir_ptr current = NULL;
    char *dup = strdup(path);
    char *tp = gnu_basename(path);
    char *bname = malloc((strlen(tp) + 1) * sizeof(char));
    strcpy(bname, tp);
    
    char *tz = gnu_dirname(path);
    char *dname = malloc((strlen(tz) + 1) * sizeof(char));
    strcpy(dname, tz);
    current = get_node_ptr(dname, root);
    
    if (current == NULL) return;
    
    if (current->figlio != NULL)
    {
        if (strcmp(current->figlio->name, bname) == 0)
        {
            printf("no\n");
            return;                                                     //cartella esistente figlio
        }
        else
        {
            dir_ptr prev = NULL;
            dir_ptr cur = current->figlio;
            dir_ptr new_dir = NULL;
            
            while (cur != NULL)
            {
                if (strcmp(cur->name, bname) < 0)
                {
                    prev = cur;
                    cur = cur->fratello;
                }
                
                else if (strcmp(cur->name, bname) > 0)
                {
                    new_dir = malloc(sizeof(dir));
                    new_dir->name = strdup(bname);
                    new_dir->fratello = cur;
                    new_dir->figlio = NULL;
                    new_dir->padre = current;
                    new_dir->flag = 1;
                    new_dir->path = strdup(dup);
                    
                    if (prev != NULL)
                    {
                        prev->fratello = new_dir;
                        printf("ok\n");                                 //creazione file a buon fine in ordine lessicografico
                        return;
                    }
                    else
                    {
                        current->figlio = new_dir;
                        printf("ok\n");
                        return;
                    }
                }
                
                else if (strcmp(cur->name, bname) == 0)                     // cartella esistente fratello
                {
                    printf("no\n");
                    return;
                }
                
            }
            
            new_dir = malloc(sizeof(dir));
            new_dir->name = strdup(bname);
            new_dir->fratello = cur;
            new_dir->figlio = NULL;
            new_dir->padre = current;
            new_dir->flag = 1;
            new_dir->path = strdup(dup);
            
            
            
            if (prev != NULL) {
                prev->fratello = new_dir;
                printf("ok\n");
                return;
            }
            else {
                current->figlio = new_dir;
                printf("ok\n");
                return;
            }
        }
    }
    else
    {
        dir_ptr new_dir = malloc(sizeof(dir));
        new_dir->name = strdup(bname);
        new_dir->fratello = NULL;
        new_dir->figlio = NULL;
        new_dir->padre = current;
        new_dir->flag = 1;
        new_dir->path = strdup(dup);
        
        
        current->figlio = new_dir;
        printf("ok\n");
        return;
    }
    
}


void create_dir(dir_ptr root, char *path) {
    
    dir_ptr current = NULL;
    char *dup = strdup(path);
    char *tp = gnu_basename(path);
    char *bname = malloc((strlen(tp) + 1) * sizeof(char));
    strcpy(bname, tp);
    
    
    char *tz = gnu_dirname(path);
    char *dname = malloc((strlen(tz) + 1) * sizeof(char));
    strcpy(dname, tz);
    
    current = get_node_ptr(dname, root);
    
    if (current == NULL) return;
    
    if (current->flag == 1) printf("no\n");                         // current = file e quindi non può creare figli
    
    if (current->figlio != NULL)
    {
        if (strcmp(current->figlio->name, bname) == 0)
        {
            printf("no\n");
            return;                                                     //cartella esistente figlio
        }
        else
        {
            dir_ptr prev = NULL;
            dir_ptr cur = current->figlio;
            dir_ptr new_dir = NULL;
            
            while (cur != NULL)
            {
                if (strcmp(cur->name, bname) < 0)
                {
                    prev = cur;
                    cur = cur->fratello;
                }
                
                else if (strcmp(cur->name, bname) > 0)
                {
                    new_dir = malloc(sizeof(dir));
                    new_dir->name = strdup(bname);
                    new_dir->fratello = cur;
                    new_dir->figlio = NULL;
                    new_dir->padre = current;
                    new_dir->flag = 0;
                    new_dir->path = strdup(dup);
                    
                    
                    if (prev != NULL)
                    {
                        prev->fratello = new_dir;
                        printf("ok\n");                                     // creazione a buon fine, vedi create
                        return;
                    }
                    else
                    {
                        current->figlio = new_dir;
                        printf("ok\n");
                        return;
                    }
                }
                
                else if (strcmp(cur->name, bname) == 0)                     // cartella esistente fratello
                {
                    printf("no\n");
                    return;
                }
                
            }
            
            new_dir = malloc(sizeof(dir));
            new_dir->name = strdup(bname);
            new_dir->fratello = cur;
            new_dir->figlio = NULL;
            new_dir->padre = current;
            new_dir->flag = 0;
            new_dir->path = strdup(dup);
            
            
            if (prev != NULL) {
                prev->fratello = new_dir;
                printf("ok\n");
                return;
            }
            else {
                current->figlio = new_dir;
                printf("ok\n");
                return;
            }
        }
        
    }
    
    else
    {
        dir_ptr new_dir = malloc(sizeof(dir));
        new_dir->name = strdup(bname);
        new_dir->fratello = NULL;
        new_dir->figlio = NULL;
        new_dir->padre = current;
        new_dir->flag = 0;
        new_dir->path = strdup(dup);
        
        
        current->figlio = new_dir;
        printf("ok\n");
        return;
    }
    
}

void delete(dir_ptr root, char *path) {
    
    dir_ptr current = NULL;
    char *tp = gnu_basename(path);
    char *bname = malloc((strlen(tp) + 1) * sizeof(char));
    strcpy(bname, tp);
    
    char *tz = gnu_dirname(path);
    char *dname = malloc((strlen(tz) + 1) * sizeof(char));
    strcpy(dname, tz);
    
    current = get_node_ptr(dname, root);
    
    if (current == NULL) return;
    
    if (current->figlio != NULL)
    {
        if (strcmp(current->figlio->name, bname) == 0)
        {
            if (current->figlio->figlio == NULL)                                        //controllo foglia
            {
                current->figlio = current->figlio->fratello;
                printf("ok\n");                                                         //spostamento puntatori
                return;
            }
            else
            {
                printf("no\n");
                return;
            }
        }
        else
        {
            dir_ptr sibling = current->figlio;
            while(sibling->fratello != NULL)
            {
                if (strcmp(sibling->fratello->name, bname) == 0)
                {
                    if (sibling->fratello->figlio ==  NULL)                             //controllo foglia
                    {
                        sibling->fratello = sibling->fratello->fratello;
                        printf("ok\n");                                                    //spostamento puntatori
                        return;
                    }
                    else
                    {
                        printf("no\n");
                        return;
                    }
                }
                sibling = sibling->fratello;
            }
        }
    }
    else
    {
        printf("no\n");
        return;
    }
    
    printf("no\n");                                                                     // non trovata
    return;
}

void delete_r(dir_ptr root, char *path) {
    
    dir_ptr current = NULL;
    char *tp = gnu_basename(path);
    char *bname = malloc((strlen(tp) + 1) * sizeof(char));
    strcpy(bname, tp);
    
    char *tz = gnu_dirname(path);
    char *dname = malloc((strlen(tz) + 1) * sizeof(char));
    strcpy(dname, tz);
    
    current = get_node_ptr(dname, root);
    
    if (current == NULL) return;
    
    if (current->figlio != NULL)
    {
        if (strcmp(current->figlio->name, bname) == 0)                                      //controllo e cancellazione
        {
            current->figlio = current->figlio->fratello;
            printf("ok\n");
            return;
        }
        else
        {
            dir_ptr sibling = current->figlio;
            while(sibling->fratello != NULL)
            {
                if (strcmp(sibling->fratello->name, bname) == 0)
                {
                    sibling->fratello = sibling->fratello->fratello;
                    printf("ok\n");
                    return;
                }
                sibling = sibling->fratello;
            }
        }
    }
    else
    {
        printf("no\n");
        return;
    }
    
    printf("no\n");                                                                         //non trovata
    return;
    
}

void do_write(dir_ptr root, char *path, char *contenuto) {
    
    dir_ptr current = NULL;
    int len = 0;
    current = get_node_ptr(path, root);
    
    if (current == NULL) return;
    
    if (current->flag == 1) {
        current->contenuto = contenuto;
        len = strlen(contenuto);
        printf("ok %d\n", len);
        
    }
    else printf("no\n");                                                            //non e' file
    
}

void do_read(dir_ptr root, char *path) {
    
    dir_ptr current = NULL;
    current = get_node_ptr(path, root);
    
    if (current == NULL) return;
    
    if (current->flag == 1)
    {
        if (current->contenuto == NULL)
        {
            printf("contenuto \n");                                                      // no contenuto
        }
        else
        {
            printf("contenuto %s\n", current->contenuto);
        }
    }
    else printf("no\n");                                                        //no file
}

void find(dir_ptr root, char *risorsa) {                                    //ricerca ricorsiva
    
    dir_ptr current = root;
    dir_ptr temp;
    
    if (current != NULL)
    {
        if (strcmp(current->name, risorsa) == 0)
        {
            found++;
            printf("ok %s\n", current->path);                       //trovata stampa path lessicografico
        }

        temp = current->figlio;
        
        while (temp != NULL)
        {
            find(temp, risorsa);
            temp = temp->fratello;
        }
        
    }
    
}

/******************************************************
 *          HELPERS DEFINITIONS                       *
 ******************************************************/

char *scan(char *string) {                                                      //legge stringa da input
    int ch;
    int i;
    
    string[0] = '\0';
    for(i = 0; i < MAX && ((ch = getchar()) != '\n'); i++)
    {
        string[i] = (char) ch;
        string[i+1] = '\0';
    }
    
    return string;
}


dir_ptr new_root(void) {                                                        //crea radice
    dir_ptr root;
    root = malloc(sizeof(dir));
    root->name = "/";
    root->padre = NULL;
    root->figlio = NULL;
    root->fratello = NULL;
    return root;
}

void bye_root(dir_ptr root) {
    free(root->name);
    free(root);
}

char *gnu_basename(char *path) {                                            //path= /a/b/c       basename = c
    char *base = strrchr(path, '/');
    return base ? base + 1 : (char*)path;
}

char *gnu_dirname (char *path)                                              //path= /a/b/c       dirname = /a/b
{
    static const char dot[] = ".";
    char *last_slash;
    last_slash = path != NULL ? strrchr (path, '/') : NULL;
    
    if (last_slash == path)
        ++last_slash;
    else if (last_slash != NULL && last_slash[1] == '\0')
        last_slash = memchr (path, last_slash - path, '/');
    if (last_slash != NULL)
        last_slash[0] = '\0';
    else
        path = (char *) dot;
    return path;
}


/******************************************************
 *          END OF PROGRAM                            *
 ******************************************************/

