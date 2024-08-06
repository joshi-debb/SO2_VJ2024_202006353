#include <mysql/mysql.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <time.h>


int main() {

    int fd[2];
    char buffer[10000];


    MYSQL *conn;
    MYSQL_ROW row;    
    char* server = "localhost";
    char* user = "grupo1";
    char* password = "123456";
    char* database = "ProyectoG1";
    
    conn = mysql_init(NULL);

    if (mysql_real_connect(conn, server, user, password, database, 0, NULL, 0) == NULL) {
        fprintf(stderr, "%s\n", mysql_error(conn));
        exit(1);
    }


    if (pipe(fd) == -1) {
        perror("Error en el pipe");
        return -1;
    }

    pid_t PidLector = fork();

    if (PidLector == -1) {
        perror("fork");
        exit(1);
    }

    //cerrrar pipe en modo 0 en modo lectura
    if (PidLector == 0) { 
        close(fd[0]);
        //todo el comando de execlp se conduce por aca
        //se abre en modo escritua
        dup2(fd[1], STDOUT_FILENO); 
        execlp("sudo", "sudo", "stap", "trace.stp", NULL);
    } else {  
        close(fd[1]);
        ssize_t nbytes;

        char *llamada , *nombre ,*sizetext;
        int pid,size;
        char buffer_time[20];
        char query[2000];
        char porcentaje_buffer[10] ="0.0";
        while ((nbytes = read(fd[0], buffer, sizeof(buffer))) > 0) {
            buffer[nbytes]= '\0';

            if (nbytes <= 4) {
  
                strcpy(porcentaje_buffer, strtok(buffer,","));
            }else{  
                char buffer_copy[10000];
                strcpy(buffer_copy, buffer);

                llamada = strtok(buffer_copy, ",");
                pid = atoi(strtok(NULL,","));
                nombre = strtok(NULL,",");
                size = atoi(strtok(NULL,","));  

                time_t tiempo = (time_t)atoi(strtok(NULL,","));
                strftime(buffer_time, sizeof(buffer_time), "%Y-%m-%d %H:%M:%S", localtime(&tiempo));
            }

            
            sprintf(query, "call InsertOrUpdateDashboard (%d,'%s',%d,'%s','%s','%s');",pid,nombre,size,porcentaje_buffer,llamada,buffer_time);
            printf("%s \n",query);

            if (mysql_query(conn, query) != 0) {
                fprintf(stderr, "Error al ejecutar la consulta: %s\n", mysql_error(conn));
            }
        }
       
        wait(NULL);
    
        mysql_close(conn);
    }
    return 0;
}
