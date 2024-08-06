#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>

void escribir(int file_descriptor)
{
    char line[9];
    for (int i = 0; i < 8; ++i)
    {
        if (rand() % 2 == 0)
        {
            // Genera un número aleatorio
            line[i] = '0' + rand() % 10;
        }
        else
        {
            // Genera una letra aleatoria
            line[i] = 'a' + rand() % 26;
        }
    }
    line[8] = '\0';
    write(file_descriptor, line, strlen(line));
}

void leer(int file_descriptor)
{
    char buffer[9];
    read(file_descriptor, buffer, 8);
    buffer[8] = '\0';
}

int main()
{    
    srand(time(NULL) + getpid());
    int file_descriptor = open("practica1.txt", O_RDWR | O_CREAT | O_TRUNC, 0644);
    if (file_descriptor == -1)
    {
        perror("Error al abrir el archivo");
        return 1;
    }
    
    while (1)
    {
        int rand_call = rand() % 2;
        // printf("randcall: %d\n", rand_call);
        sleep(rand() % 3 + 1);
        if (rand_call)
        {             
            // printf("Escribiendo\n"); 
            escribir(file_descriptor);
        }
        else
        {
            // printf("Leyendo\n");
            leer(file_descriptor);
        }
    }
    // Cierra el archivo
    close(file_descriptor);
    return 0;
}
