

bool existePartExtendida(MBR* mbr){
 //compruebo si alguna de las particiones es de tipo extendida
    if(mbr->mbr_partition_1->part_type == 'E'
    || mbr->mbr_partition_2->part_type == 'E'
    || mbr->mbr_partition_3->part_type == 'E'
    || mbr->mbr_partition_4->part_type == 'E'){
        return true;
    }

    return false;
}

bool NombreRepetido(FILE* file, MBR* mbr, char nombre[]){
//se comprueba si ya existe una particion con ese nombre
    if(mbr->mbr_partition_1->part_status == '1'){
        if(strcasecmp(mbr->mbr_partition_1->part_name, nombre) == 0) return true;
    }
    if(mbr->mbr_partition_2->part_status == '1'){
        if(strcasecmp(mbr->mbr_partition_2->part_name, nombre) == 0) return true;
    }
    if(mbr->mbr_partition_3->part_status == '1'){
        if(strcasecmp(mbr->mbr_partition_3->part_name, nombre) == 0) return true;
    }
    if(mbr->mbr_partition_4->part_status == '1'){
        if(strcasecmp(mbr->mbr_partition_4->part_name, nombre) == 0) return true;
    }

    if(existePartExtendida(mbr)){ //comṕrobamos si existe particion extendida
        partition* part = getPartExtendida(mbr); //obtenemos particion
        fseek(file, part->part_start, SEEK_SET); //posicion al inicio
        EBR* ebr = (EBR*) malloc(sizeof(EBR));
        fread(ebr, sizeof(EBR), 1, file); //leemos y guardamos

        while(ebr->part_next != -1){
            if(ebr->part_status == '1'){
                if(strcasecmp(ebr->part_name, nombre) == 0) return true; //si ya existe particion con el nombre
            }

            fseek(file , ebr->part_next, SEEK_SET);
            fread(ebr, sizeof(EBR), 1, file);
        }

        if(ebr->part_status == '1'){
            if(strcasecmp(ebr->part_name, nombre) == 0) return true;
        }
    }

    return false;
}

void CrearpartLogica(FILE* file, MBR* mbr, int bytes, char name[], char fit){
    partition* partExtendida = getPartExtendida(mbr); //obtenemos la particion extendida si existe
    int inicioExt = partExtendida->part_start; //declaramos el inicio
    int limitExt = inicioExt + partExtendida->part_size - 1; // obtenemos el final

    fseek(file, partExtendida->part_start, SEEK_SET); //posionamos al inicio del archivo

    EBR* ebr = (EBR*) malloc(sizeof(EBR));
    fread(ebr, sizeof(EBR), 1, file); //leemos y guardamos

    int inicioLibre = inicioExt;
    int limiteLibre = limitExt;
    bool canCreate = false;
    bool first = true;

    while(ebr->part_next != -1){
        if(ebr->part_status == '1'){
            inicioLibre = ebr->part_start + ebr->part_size; //si el estado es 1 entonces el espacio libre es del inicio al size
        }

        limiteLibre = ebr->part_next - 1; //limite es a la particion siguiente menos 1

        fseek(file, ebr->part_next, SEEK_SET); //posicion al inicio
        fread(ebr, sizeof(EBR), 1, file); //leemos y guardamos en ebr

        if((limiteLibre - inicioLibre + 1 >= bytes) && (bytes > (int)sizeof(EBR))){//Mayor que size y que el EBR
            canCreate = true;
            break;
        }

        first = false;
    }

    if(!canCreate){ //si no se puede crear
        if(ebr->part_status == '0') inicioLibre = inicioExt;//EBR inicial, null
        else inicioLibre = ebr->part_start + ebr->part_size;//EBR final, no null

        limiteLibre = limitExt;

        if((limiteLibre - inicioLibre+1 >= bytes) && (bytes > (int) sizeof(EBR))){//Mayor o igual que size y que el EBR
            canCreate = true;
        }
    }

    if(canCreate){ //si se puede crear
        EBR* newEbr = (EBR*) malloc(sizeof(EBR)); //nuevo ebr
        newEbr->part_status = '1';
        newEbr->part_fit = fit;
        newEbr->part_start = inicioLibre + (int)sizeof(EBR);
        newEbr->part_size = bytes - (int)sizeof(EBR);
        newEbr->part_next = -1;
        strcpy(newEbr->part_name, name);

        if(!first){ //si no es el primero
            newEbr->part_next = ebr->part_next;
            ebr->part_next = newEbr->part_start - (int)sizeof(EBR); // el siguiente va a ser el inicio del nuevo

            fseek(file, ebr->part_start - (int)sizeof(EBR), SEEK_SET); //posicionamos al inicio
            fwrite(ebr, sizeof(EBR), 1, file); //escribimos en el archivo
        }else{
            fseek(file, partExtendida->part_start, SEEK_SET); //posiionamos
            fread(ebr, sizeof(EBR), 1, file);

            if(ebr->part_status == '0'){
                newEbr->part_next = ebr->part_next;
            }else{
                newEbr->part_next = ebr->part_next;
                ebr->part_next = inicioLibre;
                fseek(file, partExtendida->part_start, SEEK_SET);
                fwrite(ebr, sizeof(EBR), 1, file);
            }
        }

        fseek(file, inicioLibre, SEEK_SET);
        fwrite(newEbr, sizeof(EBR), 1, file);

        printf("  Particion '%s' de tipo 'L' creada en %d\n", name, inicioLibre);
    }else{
        printf("  La particion no se ha podido crear\n");
    }
}


void CrearPartPrimoExt(FILE* file, MBR* mbr, int bytes, char name[], char fit, char tipoParticion){
    int inicioLibre = (int) sizeof(MBR);
    int bytesLibres = 0;
    bool canCreate = false;

    fseek(file, (int) sizeof(MBR), SEEK_SET); //posicionamos

    while(feof(file) == 0){ //mientras no sea el fin del archivo
        bytesLibres = 0;
        fgetc(file); //obtenemos el caracter en la posicion en la que se encuentra

        if(puedeCrearPart(mbr, inicioLibre, 1)){ //comprobamos si es posible crear
            while(puedeCrearPart(mbr, inicioLibre + bytesLibres, 1) && feof(file) == 0){ //si puede crear y no es el fin del archivo
                bytesLibres++;
                fgetc(file);
            }

            if(bytesLibres >= bytes){ //si el espacio es mayor al size
                if(tipoParticion == 'P'){
                    canCreate = puedeCrearPart(mbr, inicioLibre, bytes); // comprueba si es posible crear
                }else{
                    if(bytesLibres > (int) sizeof(EBR)){
                        canCreate = puedeCrearPart(mbr, inicioLibre, bytes);
                    }
                }

                printf("inicio:%d Libre:%d bytes:%d\n", inicioLibre, bytesLibres, bytes);
                if(canCreate) break;
            }

            inicioLibre += bytesLibres;
        }else{
            inicioLibre++;
        }
    }

    if(canCreate){
    //creamos nueva particion
        partition* part = (partition*) malloc(sizeof(partition));
        part->part_status = '1';
        part->part_type = tipoParticion;
        part->part_fit = fit;
        part->part_start = inicioLibre;
        part->part_size = bytes;
        strcpy(part->part_name, name);
        //en donde la colocamos
        if(mbr->mbr_partition_1->part_status == '0') mbr->mbr_partition_1 = part;
        else if(mbr->mbr_partition_2->part_status == '0') mbr->mbr_partition_2 = part;
        else if(mbr->mbr_partition_3->part_status == '0') mbr->mbr_partition_3 = part;
        else if(mbr->mbr_partition_4->part_status == '0') mbr->mbr_partition_4 = part;


        rewind(file); //al inicio
        fwrite(mbr, sizeof(MBR), 1, file); //escribimos

        if(tipoParticion == 'E'){
            fseek(file, part->part_start, SEEK_SET);
            EBR* ebr = (EBR*) malloc(sizeof(EBR));
            ebr->part_status = '0';
            ebr->part_next = -1;

            fwrite(ebr, sizeof(EBR), 1, file);
        }

        printf(" Particion '%s' de tipo '%c' creada en '%d'\n", name, tipoParticion, inicioLibre);
    }else{
        printf(" No se puede crear la particion\n");
    }

}

bool puedeCrearPart(MBR* mbr, int inicioLibre, int bytes){
//verificamos en las particiones si se puede crear una particion
    if(mbr->mbr_partition_1->part_status == '1'){
        if(!((mbr->mbr_partition_1->part_start > inicioLibre + bytes-1 && mbr->mbr_partition_1->part_start + mbr->mbr_partition_1->part_size-1 > inicioLibre + bytes-1)
        || (mbr->mbr_partition_1->part_start + mbr->mbr_partition_1->part_size-1 < inicioLibre && mbr->mbr_partition_1->part_start < inicioLibre))){
            return false;
        }
    }

    if(mbr->mbr_partition_2->part_status == '1'){
        if(!((mbr->mbr_partition_2->part_start > inicioLibre + bytes-1 && mbr->mbr_partition_2->part_start + mbr->mbr_partition_2->part_size-1 > inicioLibre + bytes-1)
        || (mbr->mbr_partition_2->part_start + mbr->mbr_partition_2->part_size-1 < inicioLibre && mbr->mbr_partition_2->part_start < inicioLibre))){
            return false;
        }
    }

    if(mbr->mbr_partition_3->part_status == '1'){
        if(!((mbr->mbr_partition_3->part_start > inicioLibre + bytes-1 && mbr->mbr_partition_3->part_start + mbr->mbr_partition_3->part_size-1 > inicioLibre + bytes-1)
        || (mbr->mbr_partition_3->part_start + mbr->mbr_partition_3->part_size-1 < inicioLibre && mbr->mbr_partition_3->part_start < inicioLibre))){
            return false;
        }
    }

    if(mbr->mbr_partition_4->part_status == '1'){
        if(!((mbr->mbr_partition_4->part_start > inicioLibre + bytes-1 && mbr->mbr_partition_4->part_start + mbr->mbr_partition_4->part_size-1 > inicioLibre + bytes-1)
        || (mbr->mbr_partition_4->part_start + mbr->mbr_partition_4->part_size-1 < inicioLibre && mbr->mbr_partition_4->part_start < inicioLibre))){
            return false;
        }
    }
    return true;
}

void analizador(char linea[]){
    //analisis
            if(strcasecmp(command, "mkdisk") == 0){
                mkdisk(params, valores, cantPieces);
            }else if(strcasecmp(command, "fdisk") == 0){
                fdisk(params, valores, cantPieces);
            }else{
                printf("  *Comando '%s' no encontrado\n", command);
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
                size = ObtenerEnterodeCadena(value); //obtengo el numero que viene en el size y lo convierto

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
            char *pathWithoutFilename = ObtenerPathSinNombreArchivo(path); //otenemos la direccion en donde va a estar el archivo

            if(!Existedir(pathWithoutFilename)){ //comprobamos si existe la dirección
                printf("  Ruta '%s' no existe, se creará\n", pathWithoutFilename);
                crearCarpetas(path); //si no existe la ruta o dirección se crea
            }else{
                if(!okExtension(path, "dk")){ //compruebo que la extensión del archivo sea la correcta
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

    if(hasSize && hasPath){ //si tiene size y dirección parametros obligatorios, nuestras banderas entonces si puede realizar lo siguiente
        if(!error){ //si no hubo error
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
                //mbr->mbr_disk_fit = fit;
                mbr->mbr_partition_1 = getInitPartition();
                mbr->mbr_partition_2 = getInitPartition();
                mbr->mbr_partition_3 = getInitPartition();
                mbr->mbr_partition_4 = getInitPartition();

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

void fdisk(char params[][255], char values[][255], int cantPieces){
    char param[255] = "";
    char value[255] = "";

    int size;
    char unit = 'K';
    char path[255];
    char typePartition = 'P';
    char fit = 'F';
    char typeDelete[4];
    char name[16];
    int add;

    bool hasSize = false;
    bool hasPath = false;
    bool hasName = false;
    bool hasAdd = false;
    bool hasDelete = false;
    bool error = false;
    bool errorDelete = false;
    bool errorAdd = false;

    for(int i = 0; i < cantPieces; i++){ //recorremos
        strcpy(param, params[i]);
        strcpy(value, values[i]);

        if(strcasecmp(param, "-size") == 0){
            hasSize = true;

            if(EsEntero(value)){
                size = ObtenerEnterodeCadena(value);//metodo para obtener el numero del size unicamente

                if(size <= 0){
                    error = true;
                    printf(" Valor '%d' no valido\n", size);
                }
            }else{
                error = true;
                printf(" Valor '%s' no valido\n", value);
            }
        }else if(strcasecmp(param, "-u") == 0){
            unit = toupper(value[0]);

            if(unit != 'B'
            && unit != 'K'
            && unit != 'M'){
                printf(" Valor de la unidad '%c' no valido\n", unit);
                unit = 'K';
            }
        }else if(strcasecmp(param, "-path") == 0){
            hasPath = true;
            strcpy(path, value);

            if(!fileExist(path)){
                error = true;
                printf(" Ruta '%s' no existe\n", path);
            }
        }else if(strcasecmp(param, "-type") == 0){
            typePartition = toupper(value[0]);

            if(typePartition != 'P'
            && typePartition != 'E'
            && typePartition != 'L'){
                printf(" Valor '%c' no valido\n", typePartition);
                typePartition = 'P';
            }
        }else if(strcasecmp(param, "-f") == 0){
            fit = toupper(value[0]);

            if(fit != 'B'
            && fit != 'F'
            && fit != 'W'){
                printf(" Valor del fit '%c' no valido\n", fit);
                fit = 'W';
            }
        }else if(strcasecmp(param, "-delete") == 0){
            hasDelete = true;
            strcpy(typeDelete, value);

            if(strcasecmp(typeDelete, "fast") != 0
            && strcasecmp(typeDelete, "full") != 0){
                printf("  Valor '%s' no valido\n", typeDelete);
                errorDelete = true;
            }
        }else if(strcasecmp(param, "-name") == 0){
            hasName = true;
            strcpy(name, value);
        }else if(strcasecmp(param, "-add") == 0){
            hasAdd = true;

            if(EsEntero(value)){
                add = ObtenerEnterodeCadena(value);//obtener el numero unicamente, para el add
            }else{
                errorAdd = true;
                printf("  Valor '%s' no valido\n", value);
            }
        }else{
            printf("  Parametro '%s' desconocido\n", param);
        }

        param[0] = 0;
        value[0] = 0;
    }

    if(!hasDelete && !hasAdd){
        if(hasSize && hasPath && hasName){
            if(!error){
                int bytes;
                if(unit == 'B'){
                    bytes = size;//en bytes
                }else if(unit == 'K'){
                    bytes = size*1024;//en kb
                }else{
                    bytes = size*1024*1024;//en Mb
                }

                FILE* file = fopen(path, "r+b");

                if(file == NULL){
                    printf("  Error, no se pudo abrir el disco\n");
                }else{
                    MBR* mbr = (MBR*) malloc(sizeof(MBR));
                    rewind(file);
                    fread(mbr, sizeof(MBR), 1, file);
                    bool hasAllPartitions = false;

                    if(mbr->mbr_partition_1->part_status == '1'
                    && mbr->mbr_partition_2->part_status == '1'
                    && mbr->mbr_partition_3->part_status == '1'
                    && mbr->mbr_partition_4->part_status == '1'){
                        hasAllPartitions = true;
                    }

                    if(hasAllPartitions && typePartition != 'L'){
                        printf(" Error, ya no se pueden crear mas particiones\n");
                    }else if(NombreRepetido(file, mbr, name)){ //metodo para determinar si ya existe una partición con ese nombre
                        printf(" Error, nombre '%s' usado en otra particion\n", name);
                    }else{
                        bool hasExtendedPartition = existePartExtendida(mbr); //verifico si existe una particion extendida

                        if(typePartition == 'E' && hasExtendedPartition){
                            printf("  Error, ya existe una particion extendida\n");
                        }else if(typePartition == 'L' && !hasExtendedPartition){
                            printf("  Error, no existe una particion extendida\n");
                        }else{
                            if(typePartition == 'L'){
                                CrearpartLogica(file, mbr, bytes, name, fit); // metodo para crear la particion logica
                            }else{//tipo de partición == 'Primaria'
                                CrearPartPrimoExt(file, mbr, bytes, name, fit, typePartition); //metodo para crear partición primaria o extendida
                            }
                        }
                    }

                    fclose(file);
                }
            }else{
                printf("  Error, valores incorrecto\n");
            }
        }else{
            printf("  Faltan parametros obligatorios\n");
        }
    }else if(hasAdd && !hasDelete){
        if(hasPath && hasName){
            if(!errorAdd){
                int bytes;
                if(unit == 'B'){
                    bytes = add;//en bytes
                }else if(unit == 'K'){
                    bytes = add*1024;//en kb
                }else{
                    bytes = add*1024*1024;//en Mb
                }

                FILE* file = fopen(path, "r+b");

                if(file == NULL){
                    printf(" Error, no se pudo abrir el disco\n");
                }else{
                    MBR* mbr = (MBR*) malloc(sizeof(MBR));
                    rewind(file);
                    fread(mbr, sizeof(MBR), 1, file);

                    if(NombreRepetido(file, mbr, name)){
                        addTamParticion(file, mbr, name, bytes);//metodo para añadir el tamaño
                    }else{
                        printf(" Error, la particion '%s' no existe\n", name);
                    }

                    fclose(file);
                }
            }else{
                printf(" Error, valores incorrectos\n");
            }
        }else{
            printf(" Faltan parametros obligatorios\n");
        }
    }else if(hasDelete && !hasAdd){
        if(hasPath && hasName){
            if(!errorDelete){
                char option;
                bool correct = false;

                do{
                    printf(" Desea eliminar la particion '%s'? S/N: ", name);
                    scanf(" %c", &option);

                    if(toupper(option) == 'S'
                    || toupper(option) == 'N') correct = true;

                }while(!correct);

                if(toupper(option) == 'S'){
                    FILE* file = fopen(path, "r+b");

                    if(file == NULL){
                        printf(" Error, no se pudo abrir el disco\n");
                    }else{
                        MBR* mbr = (MBR*) malloc(sizeof(MBR));
                        rewind(file);
                        fread(mbr, sizeof(MBR), 1, file);

                        if(NombreRepetido(file, mbr, name)){
                            deletePart(file, mbr, name, typeDelete, path); //metodo para eliminar particion
                        }else{
                            printf(" Error, la particion '%s' no existe", name);
                        }

                        fclose(file);
                    }
                }
            }else{
                printf(" Error, valores incorrectos\n");
            }
        }else{
            printf(" Faltan parametros obligatorios\n");
        }
    }else{
        printf(" Error, tiene parametros 'Add' y 'Delete'\n");
    }
}
