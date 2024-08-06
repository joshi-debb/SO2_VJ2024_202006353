#include <stdio.h>
#include <stdlib.h>
#include "cJSON.h"
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <ctype.h>
#include <time.h>
#include <unistd.h>

int contador_retiros, contador_depositos, contador_transferencias;

struct data_struct
{
    int cuenta;
    char nombre[50];
    double saldo;
};

struct data_struct usuarios[150];
int usuarios_size = 0;
pthread_mutex_t lock;

struct thread_data
{
    cJSON *json_array;
    int inicio;
    int fin;
    int procesados;
    int cont_errores;
    char errores[150][100];
};

void *carga_usuarios(void *arg)
{
    struct thread_data *data = (struct thread_data *)arg;
    cJSON *json_array = data->json_array;
    int inicio = data->inicio;
    int fin = data->fin;

    for (int i = inicio; i < fin; i++)
    {
        cJSON *item = cJSON_GetArrayItem(json_array, i);
        if (cJSON_IsObject(item))
        {
            cJSON *no_cuenta = cJSON_GetObjectItem(item, "no_cuenta");
            cJSON *nombre = cJSON_GetObjectItem(item, "nombre");
            cJSON *saldo = cJSON_GetObjectItem(item, "saldo");

            if (cJSON_IsNumber(no_cuenta) && cJSON_IsString(nombre) && cJSON_IsNumber(saldo))
            {
                int cuenta_duplicada = 0;

                for (int j = 0; j < usuarios_size; j++)
                {
                    if (usuarios[j].cuenta == no_cuenta->valueint)
                    {
                        cuenta_duplicada = 1;
                        break;
                    }
                }

                if (cuenta_duplicada)
                {
                    sprintf(data->errores[data->cont_errores], "Linea #%d (registro %d): Número de cuenta \"%d\" duplicado", 3 + i * 5, i + 1, no_cuenta->valueint);
                    data->cont_errores++;
                }
                else if (saldo->valuedouble < 0)
                {
                    sprintf(data->errores[data->cont_errores], "Linea #%d (registro %d): Saldo \"%.2f\" no puede ser menor a 0", 5 + i * 5, i + 1, saldo->valuedouble);
                    data->cont_errores++;
                }
                else
                {
                    usuarios[usuarios_size].cuenta = no_cuenta->valueint;
                    sprintf(usuarios[usuarios_size].nombre, "%s", nombre->valuestring);
                    usuarios[usuarios_size].saldo = saldo->valuedouble;
                    usuarios_size++;
                    data->procesados++;
                }
            }
            else if (!cJSON_IsNumber(no_cuenta))
            {
                sprintf(data->errores[data->cont_errores], "Linea #%d (registro %d): Número de cuenta \"%s\" no es un número entero positivo", 3 + i * 5, i + 1, no_cuenta->valuestring);
                data->cont_errores++;
            }
            else if (!cJSON_IsNumber(saldo))
            {
                sprintf(data->errores[data->cont_errores], "Linea #%d (registro %d): Saldo \"%s\" no es un número real positivo", 5 + i * 5, i + 1, saldo->valuestring);
                data->cont_errores++;
            }
        }
    }

    return NULL;
}

void leer_usuarios(char *filename, struct thread_data args[3])
{
    // Open the JSON file for reading
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        perror("Error al abrir el archivo");
        return;
    }

    // Get the file size
    fseek(file, 0, SEEK_END);
    long filesize = ftell(file);
    fseek(file, 0, SEEK_SET);

    // Read the entire file into a buffer
    char *buffer = (char *)malloc(filesize + 1);
    fread(buffer, 1, filesize, file);
    buffer[filesize] = '\0';

    // Close the file
    fclose(file);

    // Parse the JSON data
    cJSON *json = cJSON_Parse(buffer);
    if (json == NULL)
    {
        perror("Error al parsear el JSON");
        return;
    }

    // Check if it's a valid JSON array
    if (!cJSON_IsArray(json))
    {
        perror("JSON no es un array");
        return;
    }

    int array_size = cJSON_GetArraySize(json);
    int segmento_size = array_size / 3;

    pthread_t threads[3];

    for (int i = 0; i < 3; i++)
    {
        args[i].json_array = json;
        args[i].inicio = i * segmento_size;
        args[i].fin = (i == 2) ? array_size : (i + 1) * segmento_size;
        args[i].procesados = 0;
        args[i].cont_errores = 0;
        pthread_create(&threads[i], NULL, carga_usuarios, &args[i]);
    }

    for (int i = 0; i < 3; i++)
    {
        pthread_join(threads[i], NULL);
    }

    // Clean up
    cJSON_Delete(json);
    free(buffer);
}

void *carga_ops(void *arg)
{
    pthread_mutex_lock(&lock);
    struct thread_data *data = (struct thread_data *)arg;
    cJSON *json_array = data->json_array;
    int inicio = data->inicio;
    int fin = data->fin;

    for (int i = inicio; i < fin; i++)
    {
        cJSON *item = cJSON_GetArrayItem(json_array, i);
        if (cJSON_IsObject(item))
        {
            cJSON *operacion = cJSON_GetObjectItem(item, "operacion");
            cJSON *cuenta1 = cJSON_GetObjectItem(item, "cuenta1");
            cJSON *cuenta2 = cJSON_GetObjectItem(item, "cuenta2");
            cJSON *monto = cJSON_GetObjectItem(item, "monto");

            if (cJSON_IsNumber(operacion) && cJSON_IsNumber(cuenta1) && cJSON_IsNumber(cuenta2) && cJSON_IsNumber(monto))
            {
                int existe_cuenta1, existe_cuenta2, pos_c1, pos_c2 = 0;
                existe_cuenta1 = verificar_cuenta(cuenta1->valueint, &pos_c1);

                if (cuenta2->valueint != 0)
                {
                    existe_cuenta2 = verificar_cuenta(cuenta2->valueint, &pos_c2);
                }
                else
                {
                    existe_cuenta2 = 1;
                }

                if (!existe_cuenta1)
                {
                    sprintf(data->errores[data->cont_errores], "Linea #%d (registro %d): Número de cuenta \"%d\" no existe", 4 + i * 6, i + 1, cuenta1->valueint);
                    data->cont_errores++;
                }
                else if (!existe_cuenta2)
                {
                    sprintf(data->errores[data->cont_errores], "Linea #%d (registro %d): Número de cuenta \"%d\" no existe", 5 + i * 6, i + 1, cuenta2->valueint);
                    data->cont_errores++;
                }
                else if (operacion->valueint < 1 || operacion->valueint > 3)
                {
                    sprintf(data->errores[data->cont_errores], "Linea #%d (registro %d): Operación \"%d\" no válida", 3 + i * 6, i + 1, operacion->valueint);
                    data->cont_errores++;
                }
                else if (monto->valuedouble < 0)
                {
                    sprintf(data->errores[data->cont_errores], "Linea #%d (registro %d): El monto \"%.2f\" es menor a 0", 6 + i * 6, i + 1, monto->valuedouble);
                    data->cont_errores++;
                }
                else
                {
                    if (operacion->valueint == 1)
                    {
                        // Deposito
                        usuarios[pos_c1].saldo += monto->valuedouble;
                        contador_depositos++;
                        data->procesados++;
                    }
                    else if (operacion->valueint == 2)
                    {
                        // Retiro
                        if (usuarios[pos_c1].saldo >= monto->valuedouble)
                        {
                            usuarios[pos_c1].saldo -= monto->valuedouble;
                            contador_retiros++;
                            data->procesados++;
                        }
                        else
                        {
                            sprintf(data->errores[data->cont_errores], "Linea #%d (registro %d): Saldo insuficiente para realizar el retiro", 6 + i * 6, i + 1);
                            data->cont_errores++;
                        }
                    }
                    else if (operacion->valueint == 3)
                    {
                        // Transferencia
                        if (usuarios[pos_c1].saldo >= monto->valuedouble)
                        {
                            usuarios[pos_c1].saldo -= monto->valuedouble;
                            usuarios[pos_c2].saldo += monto->valuedouble;
                            data->procesados++;
                            contador_transferencias++;
                        }
                        else
                        {
                            sprintf(data->errores[data->cont_errores], "Linea #%d (registro %d): Saldo insuficiente para realizar la transferencia", 6 + i * 6, i + 1);
                            data->cont_errores++;
                        }
                    }
                }
            }
            else if (!cJSON_IsNumber(cuenta1))
            {
                sprintf(data->errores[data->cont_errores], "Linea #%d (registro %d): Número de cuenta \"%s\" no es un número entero positivo", 4 + i * 6, i + 1, cuenta1->valuestring);
                data->cont_errores++;
            }
            else if (!cJSON_IsNumber(cuenta2))
            {
                sprintf(data->errores[data->cont_errores], "Linea #%d (registro %d): Número de cuenta \"%s\" no es un número entero positivo", 5 + i * 6, i + 1, cuenta2->valuestring);
                data->cont_errores++;
            }
            else if (!cJSON_IsNumber(monto))
            {
                sprintf(data->errores[data->cont_errores], "Linea #%d (registro %d): Monto \"%s\" no es un número real positivo", 6 + i * 6, i + 1, monto->valuestring);
                data->cont_errores++;
            }
        }
    }
    pthread_mutex_unlock(&lock);
    return NULL;
}

void leer_operaciones(char *filename, struct thread_data args[4])
{
    // Open the JSON file for reading
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        perror("Error al abrir el archivo");
        return;
    }

    // Get the file size
    fseek(file, 0, SEEK_END);
    long filesize = ftell(file);
    fseek(file, 0, SEEK_SET);

    // Read the entire file into a buffer
    char *buffer = (char *)malloc(filesize + 1);
    fread(buffer, 1, filesize, file);
    buffer[filesize] = '\0';

    // Close the file
    fclose(file);

    // Parse the JSON data
    cJSON *json = cJSON_Parse(buffer);
    if (json == NULL)
    {
        perror("Error al parsear el JSON");
        return;
    }

    // Check if it's a valid JSON array
    if (!cJSON_IsArray(json))
    {
        perror("JSON no es un array");
        return;
    }

    int array_size = cJSON_GetArraySize(json);
    int segmento_size = array_size / 4;

    contador_depositos, contador_retiros, contador_transferencias = 0; // Reset the counters
    pthread_mutex_init(&lock, NULL);
    pthread_t threads[4];
    for (int i = 0; i < 4; i++)
    {
        args[i].json_array = json;
        args[i].inicio = i * segmento_size;
        args[i].fin = (i == 3) ? array_size : (i + 1) * segmento_size;
        args[i].procesados = 0;
        args[i].cont_errores = 0;
        pthread_create(&threads[i], NULL, carga_ops, &args[i]);
    }

    for (int i = 0; i < 4; i++)
    {
        pthread_join(threads[i], NULL);
    }
    pthread_mutex_destroy(&lock);

    // Clean up
    cJSON_Delete(json);
    free(buffer);
}

char *estado_de_cuenta()
{
    // Create an empty JSON Array
    cJSON *json_array = cJSON_CreateArray();

    // Iterate through the structs
    for (int i = 0; i < usuarios_size; i++)
    {
        // Create an empty JSON Object
        cJSON *item = cJSON_CreateObject();

        // Add attributes to the object
        cJSON_AddNumberToObject(item, "no_cuenta", usuarios[i].cuenta);
        cJSON_AddStringToObject(item, "nombre", usuarios[i].nombre);
        cJSON_AddNumberToObject(item, "saldo", usuarios[i].saldo);

        // Add the object to the Array
        cJSON_AddItemToArray(json_array, item);
    }

    // Format the JSON Array
    char *formatted = cJSON_Print(json_array);
    cJSON_Delete(json_array); // Free the cJSON object
    return formatted;
}

void escribir_archivo(char *filename, char *data)
{
    // Open the file
    FILE *file = fopen(filename, "wb");
    if (file == NULL)
    {
        perror("Error al abrir el archivo");
        return;
    }

    // Write the data
    fwrite(data, 1, strlen(data), file);
    fclose(file);
}

void log_carga(struct thread_data args[3])
{
    time_t now = time(NULL);
    struct tm *t = localtime(&now);

    char filename[100];
    strftime(filename, sizeof(filename), "carga_%Y_%m_%d-%H_%M_%S.log", t);

    FILE *file = fopen(filename, "w");
    if (file == NULL)
    {
        perror("Error al escribir el archivo de log de carga");
        return;
    }

    fprintf(file, "-------------------- Carga de usuarios --------------------\n\n");
    fprintf(file, "Fecha: %04d-%02d-%02d %02d:%02d:%02d\n\n",
            t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
            t->tm_hour, t->tm_min, t->tm_sec);

    int total_users = 0;
    fprintf(file, "Usuarios Cargados:\n");
    for (int i = 0; i < 3; i++)
    {
        fprintf(file, "Hilo #%d: %d\n", i + 1, args[i].procesados);
        total_users += args[i].procesados;
    }
    fprintf(file, "Total: %d\n\n", total_users);

    fprintf(file, "Errores:\n");
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < args[i].cont_errores; j++)
        {
            fprintf(file, "  - %s\n", args[i].errores[j]);
        }
    }

    fclose(file);
}

void log_operaciones(struct thread_data args[3])
{
    time_t now = time(NULL);
    struct tm *t = localtime(&now);

    char filename[100];
    strftime(filename, sizeof(filename), "operaciones_%Y_%m_%d-%H_%M_%S.log", t);

    FILE *file = fopen(filename, "w");
    if (file == NULL)
    {
        perror("Error al escribir el archivo de log de operaciones");
        return;
    }

    fprintf(file, "-------------------- Resumen de Operaciones --------------------\n\n");
    fprintf(file, "Fecha: %04d-%02d-%02d %02d:%02d:%02d\n\n",
            t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
            t->tm_hour, t->tm_min, t->tm_sec);

    int total_ops = 0;
    fprintf(file, "Operaciones Realizadas:\n");
    fprintf(file, "Retiros: %d\n", contador_retiros);
    fprintf(file, "Depositos: %d\n", contador_depositos);
    fprintf(file, "Transferencias: %d\n", contador_transferencias);
    fprintf(file, "Total: %d\n\n", contador_retiros + contador_depositos + contador_transferencias);
    fprintf(file, "Operaciones por Hilo:\n");
    for (int i = 0; i < 4; i++)
    {
        fprintf(file, "Hilo #%d: %d\n", i + 1, args[i].procesados);
        total_ops += args[i].procesados;
    }
    fprintf(file, "Total: %d\n\n", total_ops);

    fprintf(file, "Errores:\n");
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < args[i].cont_errores; j++)
        {
            fprintf(file, "  - %s\n", args[i].errores[j]);
        }
    }

    fclose(file);
}

void limpiar_buffer()
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// funcion para depositar a una cuenta
int deposito(const int cuenta, const double monto, const int validacion)
{
    int posicion;
    if (!verificar_cuenta(cuenta, &posicion))
    {
        if (validacion)
        {
            return 1;
        }
        printf("\n > La cuenta no existe\n");
        return 1;
    }
    if (!verificar_monto(monto))
    {
        if (validacion)
        {
            return 2;
        }
        printf("\n > El monto es invalido\n");
        return 2;
    }
    usuarios[posicion].saldo = usuarios[posicion].saldo + monto;
    printf("\n > Deposito correcto");
    return 0;
}

// funcion para retirar saldo de una cuenta
int retiro(const int cuenta, const double monto, const int validacion)
{
    int posicion;
    if (!verificar_cuenta(cuenta, &posicion))
    {
        if (validacion == 1)
        {
            return 1;
        }
        else
        {
            printf("\n > La cuenta no existe\n");
            return 1;
        }
    }
    if (!verificar_monto(monto))
    {
        if (validacion == 1)
        {
            return 2;
        }
        else
        {
            printf("\n > El monto es invalido\n");
            return 2;
        }
    }
    if (monto > usuarios[posicion].saldo)
    {
        if (validacion == 1)
        {
            return 3;
        }
        else
        {
            printf("\n > No cuenta con suficiente saldo\n");
            return 3;
        }
    }

    usuarios[posicion].saldo -= monto;
    printf("\n > Debito correcto");
    return 0;
}

// funcion para realizar transferencias entre 2 cuentas
int transferencia(const int cuenta, const int cuenta2, const double monto, const int validacion)
{
    int posicion;
    int posicion2;
    if (!verificar_cuenta(cuenta, &posicion))
    {
        if (validacion)
        {
            return 1;
        }
        printf("\n > La cuenta a debitar no existe");
        return 1;
    }
    if (!verificar_cuenta(cuenta2, &posicion2))
    {
        if (validacion)
        {
            return 2;
        }
        printf("\n > La cuenta a acreditar no existe");
        return 1;
    }
    if (!verificar_monto(monto))
    {
        if (validacion)
        {
            return 3;
        }
        printf("\n > El monto es invalido\n");
        return 1;
    }
    if (monto > usuarios[posicion].saldo)
    {
        if (validacion)
        {
            return 4;
        }
        printf("\n > No. cuenta con insuficiente saldo\n");
        return 1;
    }

    usuarios[posicion2].saldo = usuarios[posicion2].saldo + monto;
    usuarios[posicion].saldo = usuarios[posicion].saldo - monto;
    printf("\n > Transferencia Correctamente");
    return 0;
}

// funcion para consulta la informaicon actual de la cuenta
int consultar_cuenta(const int cuenta)
{
    int posicion;
    if (!verificar_cuenta(cuenta, &posicion))
    {
        printf("\n > La cuenta no existe\n");
        return 1;
    }
    printf("\n > DATOS:  ");
    printf("\n  - Numero de cuenta: %d", usuarios[posicion].cuenta);
    printf("\n  - Nombre: %s", usuarios[posicion].nombre);
    printf("\n  - Monto Total: %.2f", usuarios[posicion].saldo);
    printf("\n");

    return 0;
}

// verificaciones
int verificar_cuenta(const int cuenta, int *posicion)
{
    for (int i = 0; i < usuarios_size; i++)
    {
        if (cuenta == usuarios[i].cuenta)
        {
            *posicion = i;
            return 1;
        }
    }
    return 0;
}

int verificar_monto(const double monto)
{
    int numero = monto;
    int punto_decimal = 0; // Variable para contar los puntos decimales encontrados
    while (numero != 0)
    {
        int digito = numero % 10;
        if (!isdigit(digito + '0') && digito != '.')
        {
            return 0; // El monto no es válido
        }

        numero /= 10;
    }
    return 1; // El monto es válido
}

int main(int argc, char *argv[])
{
    int opcion;
    int cuenta1;
    int cuenta2;
    double monto;
    char filepath[256];
    char filepath2[256];

    printf("Ingrese la ruta del archivo para carga masiva de usuarios: ");
    if (fgets(filepath, sizeof(filepath), stdin) == NULL)
    {
        perror("Error al leer la entrada");
        return EXIT_FAILURE;
    }

    // Remover el salto de línea al final de la cadena, si lo hay
    size_t len = strlen(filepath);
    if (len > 0 && filepath[len - 1] == '\n')
    {
        filepath[len - 1] = '\0';
    }

    // CARFA MASIVA DE USUARIOS
    struct thread_data args[3];
    leer_usuarios(&filepath, args);
    log_carga(args);
    printf("\n > Usuarios cargados correctamente.\n");

    do
    {
        printf("\n+------------------------+");
        printf("\n|    Menú de opciones    |");
        printf("\n+------------------------+");
        printf("\n| 1. Depósito            |");
        printf("\n| 2. Retiro              |");
        printf("\n| 3. Transacción         |");
        printf("\n| 4. Consultar Cuenta    |");
        printf("\n| 5. Cargar Operaciones  |");
        printf("\n| 6. Estado de Cuentas   |");
        printf("\n| 7. Salir.              |");
        printf("\n+------------------------+");
        printf("\n > Introduzca opcion (1-7): ");

        if (scanf("%d", &opcion) != 1)
        {
            printf("\nError: Ingrese un número entero válido.\n");
            limpiar_buffer(); // Limpia el buffer de entrada
            continue;
        }
        switch (opcion)
        {
        case 1:
            printf("\n---------- Deposito ----------");
            printf("\n > Numero de cuenta: ");
            scanf("%d", &cuenta1);
            printf(" > Monto a Transferir: ");
            scanf("%lf", &monto);
            deposito(cuenta1, monto, 0);
            break;

        case 2:
            printf("\n---------- Retiro ----------");
            printf("\n > Numero de cuenta: ");
            scanf("%d", &cuenta1);
            printf(" > Monto a debitar: ");
            scanf("%lf", &monto);
            retiro(cuenta1, monto, 0);
            break;

        case 3:
            printf("\n---------- Transferencia ----------");
            printf("\n > Numero de cuenta a debitar: ");
            scanf("%d", &cuenta1);
            printf("\n > Numero de cuenta a creditar: ");
            scanf("%d", &cuenta2);
            printf(" > Monto a debitar: ");
            scanf("%lf", &monto);
            transferencia(cuenta1, cuenta2, monto, 0);
            break;

        case 4:
            printf("\n---------- Consultar Cuenta ---------- ");
            printf("\n > Numero de cuenta: ");
            scanf("%d", &cuenta1);
            consultar_cuenta(cuenta1);
            break;
        case 5:
            
            limpiar_buffer();
            printf("Ingrese la ruta del archivo para carga masiva de operaciones: ");
            if (fgets(filepath2, sizeof(filepath2), stdin) == NULL)
            {
                perror("Error al leer la entrada");
                return EXIT_FAILURE;
            }

            // Remover el salto de línea al final de la cadena, si lo hay
            size_t len = strlen(filepath2);
            if (len > 0 && filepath2[len - 1] == '\n')
            {
                filepath2[len - 1] = '\0';
            }
            struct thread_data args2[4];
            leer_operaciones(&filepath2, args2);
            log_operaciones(args2);
            printf("\n > Operaciones cargadas correctamente.\n");
            break;
        case 6:
            printf("\n > Se creó el archivo de Estado de Cuentas \"estado_de_cuentas.json\"\n");
            // ESTADO DE CUENTAS
            char *new_json = estado_de_cuenta();
            escribir_archivo("estado_de_cuentas.json", new_json);
            free(new_json);
            break;
        case 7:
            printf("\n > Sistema Finalizado\n");
            exit(0);
            break;
        default:
            printf("\n > Opcion Invalida");
            break;
        }

    } while (opcion != 8);
    return 0;
}