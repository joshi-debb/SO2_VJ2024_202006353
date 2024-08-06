#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>

int read_calls = 0;
int write_calls = 0;

void sigint_handler(int signum) {
    
    char buffer[1024];
    FILE *file_logs = fopen("syscalls.log", "r");
    if (file_logs == NULL) {
        perror("Error al abrir el archivo de logs");
        exit(EXIT_FAILURE);
    }

    while (fgets(buffer, sizeof(buffer), file_logs) != NULL) {
        if (strstr(buffer, "read") != NULL) {
            read_calls++;
        } else if (strstr(buffer, "write") != NULL) {
            write_calls++;
        }
    }

    // Cerrar el archivo después de leerlo
    fclose(file_logs);

    // Imprimir resultados
    printf("\nNúmero total de llamadas al sistema: %d\n", read_calls + write_calls);
    printf("Número de llamadas al sistema por tipo:\n");
    printf(" -Read: %d\n", read_calls);
    printf(" -Write: %d\n", write_calls);

    exit(EXIT_SUCCESS);
}
int main() {

    signal(SIGINT, sigint_handler);    
    pid_t pid1 , pid2 ;
    pid_t pid = fork();
    
    if (pid == -1) {
        perror("Error al hacer fork");
        exit(EXIT_FAILURE);
    } else if (pid > 0) {
        pid_t pid2 = fork(); // Creamos el segundo proceso hijo
        if (pid2 == -1) {
            perror("Error al hacer fork");
            exit(EXIT_FAILURE);
        } else if (pid2 > 0) {
            // Estamos en el proceso padre
            char command[100];
            snprintf(command, sizeof(command), "sudo stap trace.stp %d %d > syscalls.log &", pid, pid2);
            system(command);
            int status;
            waitpid(pid, &status, 0); 
            waitpid(pid2, &status, 0);

        } else {
            // hijo 2
            char *args[] = {"0", NULL};
            execv("Hijo.bin", args);
            perror("Error en execv");
            exit(EXIT_FAILURE);
        }
    } else {
        // hijo 1
        char *args[] = {"0", NULL};
        execv("Hijo.bin", args);
        perror("Error en execv");
        exit(EXIT_FAILURE);
    }

    return 0;
}
