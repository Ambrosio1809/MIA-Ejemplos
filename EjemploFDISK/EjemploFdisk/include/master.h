#ifndef MASTER_H
#define MASTER_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <time.h>
#include <ctype.h>
#include <sys/stat.h>
#include <sstream>
#include <thread>

typedef struct partition{
    char part_status;
    char part_type;
    char part_fit;
    int part_start;
    int part_size;
    char part_name[16];
}partition;

typedef struct MBR{
    int mbr_tamano;
    time_t mbr_fecha_creacion;
    int mbr_disk_signature;
    partition *mbr_partition_1;
    partition *mbr_partition_2;
    partition *mbr_partition_3;
    partition *mbr_partition_4;
}MBR;



void analizador(char line[]); //analizador del sistema
void mkdisk(char params[][255], char values[][255], int cantPieces); //crear disco
void fdisk(char params[][255], char values[][255], int cantPieces);//crear particiones, agregar o quitar espacio
void exec(char params[][255],char values[][255], int cantPieces);//Ejecturar un script completo
#endif // MASTER_H
