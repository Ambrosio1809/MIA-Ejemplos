#include <iostream>

using namespace std;

int main()
{
    char linea[500]="";
    linea[0]=0;
    bool exit = false;

    printf("*----------------------------*\n");
    printf("*-------- PROYECTO 1 --------*\n");
    printf("*--Fernando Alberto Ambrosio-*\n");
    printf("*---------201404106----------*\n");
    printf("*----------------------------*\n");

    while(!exit){
        printf("Ingrese un comando ->");
        scanf("%[^\n]",&linea);
        if(strcasecmp(linea,"exit")==0){
            exit =true;
        }else{
            if(strcmp(linea,"")!=0){
                analizador(linea);
                printf("\n");
            }
        }
        linea[0]=0;
        getchar();
    }
    return 0;
}
