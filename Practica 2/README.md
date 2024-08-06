# Manual Técnico 

## Introducción
En esta práctica, se ha desarrollado una aplicación en C que simula el funcionamiento de un sistema bancario, permitiendo la gestión de datos de usuarios y la realización de operaciones monetarias como depósitos, retiros y transferencias. La aplicación hace uso de técnicas de multithreading para garantizar la eficiencia y la integridad de los datos. Permite comprender el funcionamiento de los hilos en Linux, así como aplicar conceptos de concurrencia y paralelismo, además de utilizar técnicas de sincronización para asegurar la integridad de los datos.

## Librerías Utilizadas

![image](https://github.com/crstn07/SO2_GRUPO1/assets/87725718/de280aaa-5e19-4778-941b-fefab56d6576)

Estas librerías abarcan una amplia gama de funcionalidades desde la manipulación de datos básicos y estructuras de control hasta la gestión de procesos concurrentes y operaciones del sistema.

## Variables 

![image](https://github.com/crstn07/SO2_GRUPO1/assets/87725718/c173f91c-ff6a-4793-a53b-6cc5fadc5065)

Estas variables se utilizan para llevar la cuenta del número de retiros, depósitos y transferencias que se han realizado. Son contadores globales que se incrementarán o se utilizarán en varias partes del programa. Tambien se utilizaron los structs para definir los datos asociados a una cuenta bancaria, asi como el struct que se utiliza para pasar los datos a hilos.

## Carga de Datos
Se solicita al usuario que ingrese la ruta de un archivo que contiene datos de usuarios, lee esta ruta, la limpia de caracteres no deseados, y luego llama a funciones para procesar estos datos en paralelo utilizando hilos. Al finalizar, imprime un mensaje confirmando que la carga de usuarios se realizó con éxito. Las funciones leer_usuarios y log_carga se encargan del procesamiento real y del registro de la operación, respectivamente.

- Funcion leer_usuarios

    ![image](https://github.com/crstn07/SO2_GRUPO1/assets/87725718/59f95391-71ba-4c58-974e-d9abf0196320)

    La función leer_usuarios lee un archivo JSON que contiene información de usuarios, lo parsea y distribuye el procesamiento de los datos entre tres hilos. Cada hilo procesa una porción del arreglo JSON. Al final, la función espera a que todos los hilos terminen, y luego libera la memoria utilizada para el JSON y el buffer del archivo. Esta implementación permite manejar grandes cantidades de datos de manera eficiente utilizando la paralelización.

- Funcion log_carga

    ![image](https://github.com/crstn07/SO2_GRUPO1/assets/87725718/0e3c0def-21cd-45d4-8479-1e12288e4ff1)

    La función log_carga crea un archivo de log con información sobre la carga de usuarios procesada por varios hilos. Genera un nombre de archivo basado en la fecha y hora actuales. Luego, abre el archivo y escribe un encabezado con la fecha y hora. Para cada hilo, registra cuántos usuarios fueron procesados y detalla cualquier error encontrado durante el procesamiento. Finalmente, cierra el archivo de log. Esta función facilita el seguimiento y la auditoría de las operaciones de carga de usuarios.


## Menu Principal

![image](https://github.com/crstn07/SO2_GRUPO1/assets/87725718/f8e542bf-07fe-4ff1-a395-8ad5fa2c9b34)

El usuario puede seleccionar entre varias operaciones, tales como depósitos, retiros, transferencias, consultas de cuentas, carga masiva de operaciones y generar el estado de cuentas. Cada opción ejecuta la función correspondiente:

- Depósito: Solicita número de cuenta y monto, luego llama a la función deposito.
- Retiro: Solicita número de cuenta y monto, luego llama a la función retiro.
- Transferencia: Solicita números de cuenta de débito y crédito, y monto, luego llama a la función transferencia.
- Consultar Cuenta: Solicita número de cuenta y llama a la función consultar_cuenta.
- Cargar Operaciones: Solicita la ruta del archivo, lee el archivo para cargar operaciones, y llama a leer_operaciones y log_operaciones.
- Estado de Cuentas: Genera un archivo JSON con el estado de cuentas y llama a escribir_archivo.
- Salir: Finaliza el sistema.
- Entrada Inválida: Informa al usuario sobre una opción inválida y continúa el bucle.
- La función limpiar_buffer se utiliza para limpiar el buffer de entrada en caso de errores de lectura. El programa continúa ejecutándose hasta que el usuario selecciona la opción para salir.

## Funcion de Deposito

![image](https://github.com/crstn07/SO2_GRUPO1/assets/87725718/4f35e102-7850-47e2-9118-4f0f9e8c21f3)

La función deposito realiza un depósito en una cuenta bancaria. Verifica la existencia de la cuenta y la validez del monto. Si la cuenta no existe o el monto es inválido, devuelve un código de error correspondiente. Si el depósito se realiza con éxito, actualiza el saldo de la cuenta y devuelve un código de éxito.

## Funcion de Retiro

![image](https://github.com/crstn07/SO2_GRUPO1/assets/87725718/b64d132c-4953-444b-aba6-7da0de94fb65)

La función retiro permite retirar fondos de una cuenta bancaria. Verifica si la cuenta existe y si el monto es válido. Si la cuenta no existe, el monto es inválido o no hay suficiente saldo, devuelve un código de error correspondiente. Si el retiro se realiza con éxito, actualiza el saldo de la cuenta y devuelve un código de éxito.

## Funcion de Transferencia

![image](https://github.com/crstn07/SO2_GRUPO1/assets/87725718/1ce99d6b-4945-4d47-beff-0641622a9cc7)

La función transferencia permite transferir fondos entre dos cuentas bancarias. Verifica si las cuentas existen y si el monto es válido. Si alguna de las cuentas no existe, el monto es inválido o no hay suficiente saldo en la cuenta de débito, devuelve un código de error correspondiente. Si la transferencia se realiza con éxito, actualiza los saldos de las cuentas y devuelve un código de éxito.

## Funcion de Consultar Cuenta

![image](https://github.com/crstn07/SO2_GRUPO1/assets/87725718/866c5667-b0c2-4e3f-b0ca-98590cf15130)

La función consultar_cuenta permite consultar la información de una cuenta bancaria. Verifica si la cuenta existe y, si es así, muestra los datos de la cuenta (número de cuenta, nombre y saldo). Si la cuenta no existe, devuelve un código de error correspondiente.

## Funcion de Cargar Operaciones

Primero se solicita al usuario la ruta del archivo para cargar operaciones. Luego, lee la entrada del usuario, limpia el buffer de entrada y elimina el carácter de nueva línea al final de la cadena, si lo hay. Después, inicializa una estructura thread_data, lee las operaciones del archivo especificado y registra las operaciones en un archivo de log. Finalmente, imprime un mensaje indicando que las operaciones han sido cargadas correctamente.

- Funcion leer_operaciones

    ![image](https://github.com/crstn07/SO2_GRUPO1/assets/87725718/d9ebc78e-d573-4b48-86ee-8cf143d93317)

    La función leer_operaciones abre un archivo JSON para lectura y lee su contenido en un buffer. Luego, parsea el contenido JSON y verifica si es un arreglo válido. Divide el arreglo en 4 segmentos y lanza 4 hilos para procesar cada segmento de manera paralela. Cada hilo cuenta las operaciones de depósito, retiro y transferencia. Al final, limpia los recursos utilizados y devuelve el número total de operaciones procesadas.


- Funcion log_operaciones

    ![image](https://github.com/crstn07/SO2_GRUPO1/assets/87725718/16e03e2b-2287-4314-83de-930c126307a6)

    La función log_operaciones crea un archivo de registro de operaciones. Genera un nombre de archivo basado en la fecha y hora actuales. Luego, abre el archivo y escribe un encabezado con la fecha y hora. Registra el resumen de las operaciones realizadas, incluyendo el número de retiros, depósitos y transferencias, así como el total de operaciones. También registra el número de operaciones realizadas por cada hilo y cualquier error encontrado durante el procesamiento. Finalmente, cierra el archivo de registro.


## Funcion de Estado de Cuentas

Se imprime un mensaje indicando que se ha creado el archivo de estado de cuentas. Luego, llama a la función estado_de_cuenta para generar el estado de cuentas en formato JSON. Posteriormente, llama a la función escribir_archivo para escribir el estado de cuentas en un archivo llamado "estado_de_cuentas.json". Finalmente, libera la memoria utilizada por el JSON y finaliza.

- Funcion estado_de_cuenta

    ![image](https://github.com/crstn07/SO2_GRUPO1/assets/87725718/6c26e444-ba51-4bbf-b796-705f29455972)

    La función estado_de_cuenta genera un estado de cuentas en formato JSON. Itera a través de la lista de usuarios y crea un objeto JSON para cada uno, con los atributos de número de cuenta, nombre y saldo. Luego, agrega estos objetos al arreglo JSON. Formatea el arreglo JSON en una cadena de caracteres y devuelve esta cadena como resultado. Finalmente, libera la memoria utilizada por el objeto cJSON.

- Funcion escribir_archivo

    ![image](https://github.com/crstn07/SO2_GRUPO1/assets/87725718/cba4ab1b-b9f2-4b7a-a65a-fbe7a10167a1)

    La función escribir_archivo crea un archivo con el nombre especificado y escribe los datos proporcionados en él. Abre el archivo en modo de escritura binaria. Si la apertura del archivo falla, se imprime un mensaje de error y la función retorna. Luego, escribe los datos en el archivo y lo cierra.




