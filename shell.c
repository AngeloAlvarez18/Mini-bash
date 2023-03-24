#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>

// Retorna la cantidad de argumentos ingresados por el usuario
// Toma una cadena y devuelve un array de cadenas, que son separadas
// segun si hay un espacio.
int parse(char* str_to_parse, char** args, int* op){
    int i = 0; // cantidad de argumentos
    char* str;
    for(str = strtok(str_to_parse, " "); str != NULL; i++){
        // Si usa el operador >, indicamos a la bandera que 
        // se usó, pero no lo copiamos como argumento.
        if (strcmp(str, ">") == 0){
            *op = 1;
            i--;                
        }
        else{
            args[i] = (char *) malloc(strlen(str) + 1);
            strcpy(args[i], str);
        }
        str = strtok(NULL, " ");
    }
    args[i] = NULL;
    return i;
}

void usa_comando(char** args, int cant){
    // Abrimos el archivo con las banderas
    int fd = open(args[cant-1], O_CREAT | O_WRONLY | O_TRUNC, 0644);
    args[cant-1] = NULL;
    dup2(fd,1);
    execvp(args[0], args);
    exit(1);
}

int main(){
    printf("---- Mini Shell V3.0 ----\n");
    pid_t pid;
    char buff[30];
    int flag = 1; // Flag que indica si salimos de la shell
    
    while(flag){
        printf("> ");
        int cant_args = 0, status;
        char** args = malloc(sizeof(char*) * 20);
        int op = 0; // Bandera que indica si se usó el operador >
        fgets(buff, 30, stdin);
        buff[strlen(buff)-1] = '\0'; // Borramos el salto de linea del buffer
        cant_args = parse(buff, args, &op);
        if (strcmp(buff, "exit") == 0)
            flag = 0;
        else{
            pid = fork();
            if(pid == 0){   
                if (op){            
                    usa_comando(args, cant_args);
                }
                else{
                    execvp(buff, args);
                    printf("No existe este comando, pruebe uno valido\n");
                }
                exit(0);        
            }
            else{
                wait(&status);
                printf("\n");
            }        
        }
        for(int i = 0; i < cant_args; i++)
            free(args[i]);
        free(args);
    }

    
    return 0;
}
