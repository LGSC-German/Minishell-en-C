//E1-P3
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#define MAX_LEN 100

int espacios(char* cadena, int i);
void shell(char* diren);
void PWD(char* diren);
int CD(char* diren);
void MKDIR(const char *path, mode_t mode);
void LS(const char *diren);

int main (int argc, char *argv[ ])
{
	char opcion[MAX_LEN], path[MAX_LEN];
	strcpy(path,getcwd(argv[0], MAX_LEN));

	while (1)
	{
		strcpy(path,getcwd(path, MAX_LEN));
		shell(path);
		fgets(opcion, MAX_LEN, stdin);
		opcion[strlen(opcion)-1] = 0;
		
		if (strncmp(opcion,"pwd",3) == 0){
		    PWD(opcion);
		}
		if(strncmp(opcion,"cd",2) == 0)
		{
			int i = CD(opcion);
			if (i==0)
				strcpy(path,opcion+3);

			else
				perror("Ruta invalida");
		}
		if (strncmp(opcion,"mkdir",5) == 0)
		{
			char name[MAX_LEN];
			if (strncmp(opcion+6, "/",1) == 0)
				strcpy(name,opcion + 6);
			else {
				strcpy(name, path);
				strcat(name,"/");
				strcat(name, opcion + 6);
			}
			MKDIR(name,0775);
			
		}
		if (strcmp(opcion,"ls") == 0)
		{
			LS(path);
		}
		
		if(strcmp(opcion,"exit") == 0 || strcmp(opcion,"EXIT") == 0)
		{
			exit (1);
		}
		strcpy(opcion,"");
	}
}

int espacios(char* cadena, int i)
{
	int max = strlen(cadena);
	char caracter[2];
	while( i < max){
		strncpy(caracter,cadena+i,1);
		if (strncmp(caracter," ",1)!=0)
			return i;
		i++;
	}
	return 0;
}

void shell(char* diren)
{
	char disenio[50];
	strcpy(disenio, "┌──(minishell)-[");
	strcat(disenio, diren);
	strcat(disenio, "]\n└─$");
	printf("%s",disenio);
}

void PWD(char* diren)
{
	int num = espacios(diren,3);
    if (num == 0)
		printf("%s\n",getcwd(diren+3, MAX_LEN));
	else
		printf("Argumento invalido: %s\n",diren+num);
}

int CD(char* diren)
{
	char path[MAX_LEN];
	int num = espacios(diren,2);
	strcpy(path,diren+num);
	
	return chdir(path);
}

void MKDIR(const char *path, mode_t mode)
{
	
	if(mkdir(path,mode) == 0){
		printf("Exito\n");
		return;
	}	
	perror("Error");
	return;
}

void LS(const char *diren)
{
	DIR *directorio;
	struct dirent *entradadir; 

	if ( (directorio = opendir(diren) )== NULL)
	{
		fprintf (stderr, "No puedo abrir el directorio %s. Error %s\n", diren, strerror(errno));
		exit(1);
	}
	while ( (entradadir = readdir (directorio) ) != NULL){
		if (entradadir->d_type==DT_DIR) printf("\e[34m %s\n \e[0m",entradadir ->d_name);
		//if (entradadir->d_type!=DT_UNKNOWN ) printf("\e[31m%s \n \e[0m",entradadir ->d_name);
		else if (entradadir->d_type!=DT_REG ) printf("\e[32m %s\n \e[0m",entradadir ->d_name);
		else if (entradadir->d_type!=DT_FIFO ) printf("\e[33m%s\n \e[0m",entradadir ->d_name);
		else if (entradadir->d_type!=DT_LNK ) printf("\e[3m %s\n \e[0m",entradadir ->d_name);
	}
	closedir (directorio);	
}
