#include "master.h"
void analizador(char linea[]){
    // Aca podrá ir su analizador, Queda a discreción de cada uno
        if(hasCommand){
            if(strcasecmp(command, "mkdisk") == 0){
                mkdisk(params, valores, cantPieces);
            }else if(strcasecmp(command, "exec") == 0){
                exec(params, valores, cantPieces);
            }else if(strcasecmp(command, "rep") == 0){
                rep(params, valores, cantPieces);
            }else{
                printf("  *Comando '%s' no encontrado\n", command);
            }
        }

    }
}

//Metodo para realizar exec, ejecución del script
void exec(char params[][255],char valores[][255],int cantPieces){
    char parametro[255]=""; //arreglo de caracteres para el parametro
    char valor[255]=""; //arreglo de caracteres para el valor del parametro

    char path[255]=""; //direccion o ruta
    bool tienePath=false,error=false; //banderas

    for(int i=0;i<cantPieces;i++){ //ciclo para la cantidad de piezas o parejas que tengo en el comando
        strcpy(parametro,params[i]); //copio el parametro en la posición i a mi variable ejemplo: el parametro es -path
        strcpy(valor,valores[i]); // copio el valor en la posición i a mi variable ejemplo: esta es la direccion del path /home/fernando/Escritorio/archivo1.script.....

        if(strcasecmp(parametro,"-path")==0){ //si el parametro es path
            tienePath=true; //activo mi bandera del path
            strcpy(path,valor); //copio el valor a mi variable path

            if(!fileExist(path)){ //compruebo si existe la ruta
                error=true;
                printf(" La Ruta '%s' no existe\n",path);
            }
        }else{ //por si no reconoce el parametro
            printf(" El parametro '%s' no ha sido reconocido\n",parametro);
        }
        parametro[0]=0; //reinicio mis variables
        valor[0]=0; //reinicio mis variables
    }
    if(tienePath){ //si tiene direccion o path entonces realiza lo siguiente
        if(!error){ //si no hubo error
            FILE* entrada = fopen(path,"r"); //abro el archivo que viene en el path
            if(entrada==NULL){
                printf(" Error, no se pudo abrir el archivo de entrada\n"); //por si no se puede abrir el archivo
            }else{
                bool errorEliminar = remove("mounted.txt");
                if(!errorEliminar){
                    printf("\n");
                }else{
                }
                char linea[255]=""; // variable para almacenar cada linea del archivo a ejecutar
                linea[0]=0;
                char caracter;
                while(feof(entrada)==0){ //mientras que no sea el fin del archivo
                    while((caracter = fgetc(entrada))!='\n' && caracter != '\r' && feof(entrada)==0){
                        concatenar(linea,caracter); // concateno los caracteres en mi variable linea mientras no sea el final del archivo ni un salto de linea
                    }

                    if(strcmp(linea,"")!=0) printf("%s\n",linea); //imprimo la linea en consola, la aplicacion

                    if(strcmp(linea,"")!=0 && linea[0]!='\0' && !Comentario(linea)){
                        analizador(linea); //analizo la linea que se obtuvo
                    }
                    linea[0]=0;
                }
                fclose(entrada);
            }
        }else{
            printf(" Error, los valores son incorrectos");
        }
    }else{
        printf(" Hacen falta parametros obligatorios");
    }
}

void mkdisk(char params[][255], char values[][255], int cantPieces){
    char param[255] = ""; //cadena para el parametro
    char value[254] = ""; // cadena para el valor del parametro

    int size;
    char unit = 'M';//en Megabytes por defecto
    char fit = 'F';//Primer ajuste por defecto
    char path[255];
    path[0] = 0;

    bool hasSize = false;
    bool hasPath = false;
    bool error = false;

    for(int i = 0; i < cantPieces; i++){ //recorremos el comando
        strcpy(param, params[i]); //copiamos el valor en la posición i del vector en la variable del parametro
        strcpy(value, values[i]); //copiamos el valor en la posición i del vector en la variable del valor
        printf("el comando en %i es %s \n",i,param);
        printf("el valor en %i es %s \n",i,value);
        printf("la cantidad de piezas son %i \n",cantPieces);

        if(strcasecmp(param, "-size") == 0){ //comparamos el parametro guardado para ver si viene size
            hasSize = true; //activamos nuestra bandera para el size

            if(EsEntero(value)){ //validamos si el valor del size es entero
                size = ObtenerEnterodeCadena(value); //metodo con el que obtengo el numero que viene en el size y lo convierto

                if(size <= 0){ //si el size es menor que 0, negativo, debe dar error
                    error = true; //bandera para el error
                    printf("  Valor de tamaño '%d' no valido\n", size); //si no es valido indica el error
                }
            }else{ // da error porque el size no es un numero entero
                error = true; //bandera para el error
                printf("  Valor de tamaño '%s' no valido\n", value); //indica el error
            }
        }else if(strcasecmp(param, "-u") == 0){ //comparamos el parametro en nuestra variable con el que se espera
            unit = toupper(value[0]); //obtenemos el valor del parametro y para mas facilidad lo volvemos en mayuscula

            if(unit != 'K' && unit != 'M'){ //comparamos el valor con el que se espera
                printf("  Valor de unidad '%c' no valido\n", unit); //si no es ninguno de los valores da error
                unit = 'K';
            }

        }else if(strcasecmp(param, "-path") == 0){// comparamos el parametro en nuestra variable con el que se espera
            hasPath = true; //activo mi bandera
            strcpy(path, value);//copio el valor a mi variable
            char *pathWithoutFilename = ObtenerPathSinNombreArchivo(path); //metodo para obtener la direccion en donde va a estar el archivo

            if(!Existedir(pathWithoutFilename)){ //comprobamos si existe la dirección
                printf("  Ruta '%s' no existe, se creará\n", pathWithoutFilename);
                crearCarpetas(path); //METODO QUE COMPRUEBA si no existe la ruta o dirección se crea
            }else{
                if(!okExtension(path, "dk")){ //METODO PARA COMPROBAR la extensión del archivo sea la correcta
                    printf("  Extension:!dk\n");
                }
            }
        }else if(strcasecmp(param, "-f") == 0){//comparamos el parametro con el que se espera
            fit = toupper(value[0]);//convertimos a mayuscula para más facilidad

            if(fit != 'B'
            && fit != 'F'
            && fit != 'W'){ //si no es ninguno de los valores esperados, da error
                printf("  Valor de fit '%c' no valido\n", fit);
                fit = 'F';
            }
        } else{ //SI VIENE ALGÚN OTRO PARAMETRO QUE NO SE ESPERA EN ESTE COMANDO, ERROR
            printf("  Parametro '%s' desconocido\n", param);
            error = true;
        }

        param[0] = 0;//reinicio de variables
        value[0] = 0;//reinicio de variables
    }

    if(hasSize && hasPath){ //si tiene size y dirección parametros obligatorios, nuestras banderas en "true" entonces si puede realizar lo siguiente
        if(!error){ //si no hubo error en algun otro punto del comando
            int bytes;//en Mb

            if(unit == 'K'){ //si la unidad son kilobytes
                bytes = size*1024;//en kb
            }else{ //sino es en megabytes
                bytes = size*1024*1024;//en Mb
            }

            FILE* file = fopen(path, "w+b"); //abrimos el archivo en modo escritura y binario

            if(file == NULL){ //no se pudo crear el archivo
                printf("  Error al crear disco\n");
            }else{
                int start = (int) sizeof(MBR); // el inicio va a ser del tamaño del mbr

                time_t current_time; // se obtiene la fecha actual
                current_time = time(NULL);//se obtiene la cantidad de segundos

                //srand(time(NULL));// se genera un numero aleatorio
                int disk_signature = rand(); //se genera un numero aleatorio para el disk signature del mbr

                MBR* mbr = (MBR*) malloc(sizeof(MBR)); // se crea o inicializa el struct asignando el espacio en memoria del tamaño del struct MBR
                mbr->mbr_tamano = bytes; //asignamos el valor correspondiente a cada atributo
                mbr->mbr_fecha_creacion = current_time;
                mbr->mbr_disk_signature = disk_signature;

                rewind(file);//nos colocamos al inicio del archivo
                fwrite(mbr, sizeof(MBR), 1, file); // se escribe el mbr, el tamaño que se va a escribir, la cantidad de registros a escribir y el archivo

                for(int i = start; i < bytes; i++){
                    fputc('0', file); //llenamos de 0 el archivo
                }

                printf(" Disco creado en '%s'\n", path);
                fclose(file); // se cierra el archivo
                //mkdisk2(params,values,cantPieces); //para raid
            }
        }else{
            printf(" Error, valores incorrectos\n");
        }
    }else{
        printf(" Faltan parametros obligatorios\n");
    }

}
