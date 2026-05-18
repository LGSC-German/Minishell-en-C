#include <sys/sysmacros.h>
#include <sys/stat.h>
#include <sys/statvfs.h>
#include <sys/syscall.h>
#include <sys/ioctl.h>
#include <sys/utsname.h>
#include <sys/sysinfo.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
//#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <net/if.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <utmp.h>

#define MAX_LEN 500

/* Funciones base para el shell*/
char *crearvector(int n)
{
	char *vector = (char *)malloc(n * sizeof(char));
    return vector;
}
int caracter(const char *cadena, int i)
{
	int max = strlen(cadena);
	char caracter[2];
	while( i < max){
		strncpy(caracter,cadena+i,1);
		if (strncmp(caracter," ",1)==0)
			return i;
		i++;
	}
	return 0;
}
int espacios(const char *cadena, int i)
{
	int max = strlen(cadena);
	char caracter[2];
	if (strlen(cadena)==i) return 0;
	strncpy(caracter,cadena+i,1);
	if (strncmp(caracter," ",1)!=0) return -1;
	i++;
	while( i < max){
		strncpy(caracter,cadena+i,1);
		if (strncmp(caracter," ",1)!=0) return i;
		i++;
	}
	return 0;
}
DIR* odir(const char *path)
{
	DIR *directorio;
	if ( (directorio = opendir(path) )== NULL){
			perror("Error ruta");
		}
	return directorio;
}
void funstat(const char *path)
{
	struct stat sb;
	if(stat(path, &sb) == -1){
		perror("Error");
		return;
	}
	printf("ID of containing device:  [%x,%x]\n", major(sb.st_dev), minor(sb.st_dev));   
	printf("File type:                ");    
	switch (sb.st_mode & S_IFMT) {
       case S_IFBLK:  printf("block device\n");            break;
       case S_IFCHR:  printf("character device\n");        break;
       case S_IFDIR:  printf("directory\n");               break;
       case S_IFIFO:  printf("FIFO/pipe\n");               break;
       case S_IFLNK:  printf("symlink\n");                 break;
       case S_IFREG:  printf("regular file\n");            break;
       case S_IFSOCK: printf("socket\n");                  break;
       default:       printf("unknown?\n");                break;	
	}
}
void shell(char *path)
{
	char disenio[MAX_LEN];
	strcpy(disenio, "┌──(minishell)-[");
	strcat(disenio, path);
	strcat(disenio, "]\n└─$ ");
	printf("%s",disenio);
}

/*Tipado de funciones del shell*/
void PWD(char *opcion);
void CD(char *opcion, char *path);
void MKDIR(const char *path_name, mode_t mode);
void LS(char *opcion, const char *path);
void STAT(const char *opcion);
void CAT(char *opcion);
void RENAME(char *opcion);
void FIND(const char *path, const char *name);
/* wall, ip, mac, numerosdisp, free, uname */
void UNAME(char *opcion);
void IP(char *opcion);
void MAC(char *opcion);
void FREE(char *opcion);
void WALL(char *opcion);
void MESG(char *opcion);
//unlink, vfstat, date, who, mesg
void UNLINK(char *path);
void VFSTAT(char *opcion);
void DATE();
void WHO();

int main (int argc, char *argv[ ])
{
	char opcion[MAX_LEN], path[MAX_LEN], *auxiliar;
	int i,j;
	strcpy(path,getcwd(argv[0], MAX_LEN));
	
	while (1)
	{
		strcpy(path,getcwd(path, MAX_LEN));
		shell(path);
		fgets(opcion, MAX_LEN, stdin);
		opcion[strlen(opcion)-1] = 0;
		i = espacios(opcion,0);
		if (i != -1) // valida espacios antes de cualquier caracter
			strcpy(opcion,opcion+i);
		if (strncmp(opcion,"pwd",3) == 0) PWD(opcion);
		if (strncmp(opcion,"cd",2) == 0) CD(opcion, path);
		if (strncmp(opcion,"mkdir",5) == 0) MKDIR(opcion,0775);
		if (strncmp(opcion,"ls",2) == 0) LS(opcion,path);
		if (strncmp(opcion,"stat",3) == 0) STAT(opcion);
		if (strncmp(opcion,"cat",3) == 0) CAT(opcion);
		if (strncmp(opcion,"rename",6) == 0) RENAME(opcion);
		if (strncmp(opcion,"uname",5) == 0) UNAME(opcion);
		if(strncmp(opcion,"find",4) == 0){
			auxiliar = crearvector(MAX_LEN);
			i = espacios(opcion, 4);
			j = caracter(opcion, i);
			strncpy(auxiliar,opcion+i,j-i);
			path[j-i] = 0;
			printf("%s\n",auxiliar);
			printf("%s\n",opcion+j);
			j = espacios(opcion, j);
			FIND(auxiliar,opcion+j);
			free(auxiliar);
		}
		if (strncmp(opcion,"ip",2) == 0) IP(opcion);
		if (strncmp(opcion,"mac",3) == 0) MAC(opcion);
		if (strncmp(opcion,"free",4) == 0) FREE(opcion);
		if (strncmp(opcion,"wall",4) == 0) WALL(opcion);//pruebalo julio no me jala se lo pedi a copilot
		if (strncmp(opcion,"mesg",4) == 0) MESG(opcion);//pruebalo julio no me jala se lo pedi a copilot
		if (strncmp(opcion,"unlink",6) == 0) UNLINK(opcion);
		if (strncmp(opcion,"vfstat",6) == 0) VFSTAT(opcion);
		if (strncmp(opcion,"date",4) == 0) DATE();
		if (strncmp(opcion,"who",4) == 0) WHO();
		if(strcmp(opcion,"exit") == 0 || strcmp(opcion,"EXIT") == 0)
			exit (1);
		strcpy(opcion,"");
		printf("\n");
		
	}
}

void PWD(char *opcion)
{
	int num = espacios(opcion,3);
    if (num >= 0)
		printf("%s\n",getcwd(opcion+3, MAX_LEN));
	else
		printf("\e[0;33mComando invalido:\e[0m quizas es pwd\n");
}

void CD(char *opcion, char *path)
{
	int num = espacios(opcion,2);
	if (num == -1 ||  num == 0){
		printf ("\e[0;33mComando invalido:\e[0m cd <direccion>\n");
	} else{
		strcpy(path,opcion+num);
		num = chdir(path);
		if (num == -1) printf("\033[0;33mRuta invalida:\033[0m cd <ruta>\n");
	}
	return;
}

void MKDIR(const char *path_name, mode_t mode)
{
	int num = espacios(path_name,5);
	if (num == -1) printf("\e[0;33mComando invalido:\e[0m mkdir <ruta_nombre>\n");
	else if (num == -1) printf("\e[0;33mRuta invalida:\e[0m mkdir <ruta>\n");
	else if(mkdir(path_name+num,mode) == 0) printf("Creado con exito\n");
	return;
}

void LS(char *opcion, const char *path)
{
	DIR *directorio;
	struct dirent *entradadir; 
	int tipo = 0, j = 0;
	int i = espacios(opcion,2);
	if(i == -1) {
		printf("\e[0;33mComando invalido:\e[0m quizas es ls\n");
		return;
	}
	if(i != 0){
		if(strncmp(opcion+i,"-",1)!=0){
			printf("\e[0;33mComando incorrecto:\e[0m ls -<bandera>\n");
			return;
		}
		i++;
		if(strncmp(opcion+i,"l",1)==0) tipo=1;
		if(strncmp(opcion+i,"a",1)==0) tipo=2;
		if(strncmp(opcion+i,"i",1)==0) tipo=3;
		if(strncmp(opcion+i,"la",2)==0) tipo=4;
		if(strncmp(opcion+i,"li",2)==0) tipo=5;
		if(strncmp(opcion+i,"lai",2)==0) tipo=6;
		
		if(tipo>0 && tipo<4) i++;
		if(tipo==4 || tipo==5) i=i+2;
		if(tipo==6) i=i+3;
		j = espacios(opcion,i);
	}
	if(j==-1){
			printf("\e[0;33mBandera incorreta:\e[0m %s", opcion+i);
			return;
	}
	
	directorio = odir(path); // -l la a i li lai
	if (directorio == NULL)
		return;
	while ( (entradadir = readdir (directorio) ) != NULL){
		char fullpath[512];
		snprintf(fullpath, sizeof(fullpath), "%s/%s", path, entradadir->d_name);// es lo mismo que aplicar 1 strcpy y 2 strcat

		if (tipo == 2 || tipo == 4 || tipo == 6){
			if (tipo == 6) printf("%10ld  ",entradadir->d_ino);
			if (tipo == 4 || tipo == 6) {
				struct stat sb;
				if (stat(fullpath, &sb) == -1) {
					perror("stat");
				} else {
					printf("[%x,%x] ", major(sb.st_dev), minor(sb.st_dev));
					switch (sb.st_mode & S_IFMT) {
						case S_IFBLK:  printf("block ");     break;
						case S_IFCHR:  printf("char ");      break;
						case S_IFDIR:  printf("dir ");       break;
						case S_IFIFO:  printf("fifo ");      break;
						case S_IFLNK:  printf("link ");      break;
						case S_IFREG:  printf("file ");      break;
						case S_IFSOCK: printf("socket ");    break;
						default:       printf("unknown ");   break;
					}
				}
			}

			if (entradadir->d_type==DT_DIR) printf(" %s\n",entradadir ->d_name);
			else if (entradadir->d_type==DT_REG) printf("\e[34m %s\e[0m\n",entradadir ->d_name);
			else if (entradadir->d_type==DT_FIFO) printf("\e[33m %s\e[0m\n",entradadir ->d_name);
			else if (entradadir->d_type==DT_LNK) printf("\e[36m %s\e[0m\n",entradadir ->d_name);
			else printf("\e[31m%s\e[0m\n",entradadir ->d_name);

		} else if (strcmp(entradadir->d_name, ".") != 0 && strcmp(entradadir->d_name, "..") != 0) {
			if (tipo == 3 || tipo == 5) printf("%10ld  ",entradadir->d_ino);
			if (tipo == 1 || tipo == 5) {
				struct stat sb;
				if (stat(fullpath, &sb) == -1) {
					perror("stat");
				} else {
					printf("[%x,%x] ", major(sb.st_dev), minor(sb.st_dev));
					switch (sb.st_mode & S_IFMT) {
						case S_IFBLK:  printf("block ");     break;
						case S_IFCHR:  printf("char ");      break;
						case S_IFDIR:  printf("dir ");       break;
						case S_IFIFO:  printf("fifo ");      break;
						case S_IFLNK:  printf("link ");      break;
						case S_IFREG:  printf("file ");      break;
						case S_IFSOCK: printf("socket ");    break;
						default:       printf("unknown ");   break;
					}
				}
			}

			if (entradadir->d_type==DT_DIR) printf(" %s\n",entradadir ->d_name);
			else if (entradadir->d_type==DT_REG) printf("\e[34m %s\e[0m\n",entradadir ->d_name);
			else if (entradadir->d_type==DT_FIFO) printf("\e[33m %s\e[0m\n",entradadir ->d_name);
			else if (entradadir->d_type==DT_LNK) printf("\e[36m %s\e[0m\n",entradadir ->d_name);
			else printf("\e[31m%s\e[0m\n",entradadir ->d_name);

		}
	}
	closedir(directorio);	
}

void STAT(const char *opcion)
{
	int i = espacios(opcion,4);
	
	if (i == -1) {
		 printf("\e[0;33mComando invalido:\e[0m quizas es stat\n");
		 return;
	}
	if (i == 0){
		printf ("\e[0;33mDireccion invalida:\e[0m stat <direccion>\n");
		return;
	}
	
	DIR *directorio;
	directorio = odir(opcion+i);
	if (directorio == NULL)	return;
	funstat(opcion+i);
	closedir(directorio);
}

void CAT(char *opcion)
{
	char texto[5000];
	int i = espacios(opcion,3);
	if (i == -1) {
		 printf("\e[0;33mComando invalido:\e[0m quizas es cat\n");
		 return;
	}
	int b = open(opcion+i, O_RDONLY);
	if (b == -1){
		printf ("\e[0;33mDireccion invalida:\e[0m cat <direccion>\n");
		return;
	}
	int n = read(b, texto, sizeof(texto));
	write(STDOUT_FILENO, texto, n);
	close(b);
}

void RENAME(char *opcion)
{
	char name[MAX_LEN];
	int i = espacios(opcion, 6);
	if (i == -1) {
		 printf("\e[0;33mComando invalido:\e[0m quizas es rename\n");
		 return;
	} 
	if (i == 0) {
		 printf("\e[0;33mDireccion invalida:\e[0m quizas es rename <nombre_actual> <nombre_nuevo>\n");
		 return;
	}
	int j = caracter(opcion, i);
	strncpy(name,opcion+i,j-i);
	j = espacios(opcion, j);
	printf("%s\n%s\n",name,opcion+j);
	i = rename(name,opcion+j);
	if (i != 0){
		printf("\e[0;33mError:\e[0m No renombrado");
		return;
	}
	printf("Renombrado: \e[0;32m%s --> %s\e[0m\n",name,opcion+j);
	return;
}

void FIND(const char *path, const char *name)
{
	int j = strlen(name);
	char newpath[MAX_LEN],base[MAX_LEN];
	DIR *directorio;
	struct dirent *entradadir;
	directorio = odir(path);
	if (directorio == NULL)	return;
	strcpy(base,path);
	strcat(base,"/");
	while ( (entradadir = readdir (directorio) ) != NULL){
		strcpy(newpath,base);
		if (entradadir->d_type==DT_REG){
			if (strncmp(name,entradadir ->d_name,j)==0){
				printf("\e[34mArchivo encontrado en este directorio:\e[0m %s\n",newpath);
				strcat(newpath,entradadir ->d_name);
				funstat(newpath);
			} else{ 
				strcat(newpath,entradadir ->d_name);
				printf("%s\n",newpath);
			}
		}
		if (entradadir->d_type==DT_DIR){
			strcat(newpath,entradadir ->d_name);
			if (strcmp(entradadir->d_name, ".") != 0 && strcmp(entradadir->d_name, "..") != 0) {
				FIND(newpath, name);
			}
		}
	}
	closedir(directorio);
}

void UNAME(char *opcion)
{
	struct utsname syst;  // estructura real, no puntero
    int i = espacios(opcion, 5);
    if (i == -1){
        printf("\e[0;33mComando invalido:\e[0m quizas es uname \n");
        return;
    }

    if (uname(&syst) == -1){  // pasamos la dirección
        printf("\e[0;33mError en uname:\e[0m no obtencion \n");
        return;
    }

    printf("%s %s %s %s %s\n", syst.sysname, syst.nodename, syst.release, syst.version, syst.machine);
}

void IP(char *opcion)
{
	if (espacios(opcion, 2) == -1){
		printf("\e[0;33mComando invalido:\e[0m quizas es ip\n");
		return;
	}
	int sock;
    struct ifreq ifr;
    struct sockaddr_in *ipaddr;
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == -1) {
        perror("Error creando socket");
        return;
    }
    strncpy(ifr.ifr_name, "docker0", IFNAMSIZ - 1);
    if (ioctl(sock, SIOCGIFADDR, &ifr) == -1) {
        perror("ioctl SIOCGIFADDR (¿Está activa la interfaz?)");
    } else {
        ipaddr = (struct sockaddr_in *)&ifr.ifr_addr;
        printf("IP de docker0: %s\n", inet_ntoa(ipaddr->sin_addr));
    }
    strncpy(ifr.ifr_name, "lo", IFNAMSIZ - 1);
    if (ioctl(sock, SIOCGIFADDR, &ifr) == -1) {
        perror("ioctl SIOCGIFADDR (¿Está activa la interfaz?)");
    } else {
        ipaddr = (struct sockaddr_in *)&ifr.ifr_addr;
        printf("IP de lo: %s\n", inet_ntoa(ipaddr->sin_addr));
    }    
    strncpy(ifr.ifr_name, "wlan0", IFNAMSIZ - 1);
    if (ioctl(sock, SIOCGIFADDR, &ifr) == -1) {
        perror("ioctl SIOCGIFADDR (¿Está activa la interfaz?)");
    } else {
        ipaddr = (struct sockaddr_in *)&ifr.ifr_addr;
        printf("IP de wlan0: %s\n", inet_ntoa(ipaddr->sin_addr));
    }
}

void MAC(char *opcion)
{
	if (espacios(opcion, 3) == -1){
		printf("\e[0;33mComando invalido:\e[0m quizas es mac\n");
		return;
	}
	int sock;
    struct ifreq ifr;
    unsigned char *mac;
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == -1) {
        perror("Error creando socket");
        return;
    }
    strncpy(ifr.ifr_name, "docker0", IFNAMSIZ - 1);
    if (ioctl(sock, SIOCGIFHWADDR, &ifr) == -1) {
        perror("ioctl SIOCGIFHWADDR");
    } else {
       mac = (unsigned char *)ifr.ifr_hwaddr.sa_data;
        printf("MAC de docker0: %02x:%02x:%02x:%02x:%02x:%02x\n", 
               mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    }
    strncpy(ifr.ifr_name, "lo", IFNAMSIZ - 1);
    if (ioctl(sock, SIOCGIFHWADDR, &ifr) == -1) {
        perror("ioctl SIOCGIFHWADDR");
    } else {
       mac = (unsigned char *)ifr.ifr_hwaddr.sa_data;
        printf("MAC de lo: %02x:%02x:%02x:%02x:%02x:%02x\n", 
               mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    }
    strncpy(ifr.ifr_name, "wlan0", IFNAMSIZ - 1);
    if (ioctl(sock, SIOCGIFHWADDR, &ifr) == -1) {
        perror("ioctl SIOCGIFHWADDR");
    } else {
       mac = (unsigned char *)ifr.ifr_hwaddr.sa_data;
        printf("MAC de wlan0: %02x:%02x:%02x:%02x:%02x:%02x\n", 
               mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    }
}

void FREE(char *opcion)
{
	if (espacios(opcion, 4) == -1){
		printf("\e[0;33mComando invalido:\e[0m quizas es free\n");
		return;
	}
	
    struct sysinfo info;

    if (sysinfo(&info) == 0) {
        printf("Memoria total: %lu \n", info.totalram);
        printf("Memoria libre: %lu \n", info.freeram);
        printf("Memoria usada: %lu \n", (info.totalram - info.freeram));
        printf("Swap total: %lu \n", info.totalswap);
        printf("Swap libre: %lu \n", info.freeswap);
    } else {
        perror("Error al obtener información de memoria");
    }

    return;
}


void UNLINK(char *opcion)
{
	int i = espacios(opcion,6);
	if (i <= 0){
		printf("\e[0;33mUso:\e[0m unlink <archivo>\n");
		return;
	}

	if (unlink(opcion+i) == -1){
		perror("Error");
		return;
	}

	printf("Archivo eliminado: %s\n", opcion+i);
}

void VFSTAT(char *opcion)
{
	int i = espacios(opcion,6);
	struct statvfs vfs;

	if (i == -1){
		printf("\e[0;33mUso:\e[0m vfstat <archivo>\n");
		return;
	}

	if (statvfs(opcion+i, &vfs) != 0){
		perror("llamado de statvfs");
		return;
	}
	
	printf("\tArchivo:%s", opcion+1);
	printf("\tTamaño de bloques: %ld\n",  (long) vfs.f_bsize);
	printf("\tTamaño de fragmento: %ld\n", (long) vfs.f_frsize);
	printf("\tTamaño en unidades: %lu\n", (unsigned long) vfs.f_blocks);
	printf("\tBloques libres %lu\n",  (unsigned long) vfs.f_bfree);
	printf("\tBloques Disponibles: %lu\n", (unsigned long) vfs.f_bavail);
	printf("\tNúmero de Inodos: %lu\n",  (unsigned long) vfs.f_files);
	printf("\tNúmero de Inodos Libres: %lu\n",  (unsigned long) vfs.f_ffree);
	printf("\tNúmero de Inodos Disponibles: %lu\n", (unsigned long) vfs.f_favail);
	printf("\tID del S.A.: %#lx\n",  (unsigned long) vfs.f_fsid);
	printf("\tBandera: ");
	if (vfs.f_flag == 0)
		printf("(Ninguna)\n");
	else {
		if ((vfs.f_flag & ST_RDONLY) != 0)
			printf("ST_RDONLY ");
		if ((vfs.f_flag & ST_NOSUID) != 0)
			printf("ST_NOSUID");
		printf("\n");
	}
	printf("\tLongitud max para archivo: %ld\n", (long)vfs.f_namemax);
}

void DATE()
{
	time_t t;
	struct tm *date;

	time(&t);
	date = localtime(&t);

	char fecha[100];
	strftime(fecha, sizeof(fecha), "%Y-%m-%d %a %H:%M:%S", date);

	printf("%s\n", fecha);
}

void WHO()
{
	struct utmp *usuarios;
	setutent(); // inicio
	while ((usuarios = getutent()) != NULL){
		if (usuarios->ut_type == USER_PROCESS){
			printf("%s\t%s\n", usuarios->ut_user, usuarios->ut_line);
		}
	}
	endutent();
}

void WALL(char *opcion)
{
	int i = espacios(opcion, 4);
	if (i == -1){
		printf("\e[0;33mComando invalido:\e[0m quizas es wall\n");
		return;
	}
	if (i == 0){
		printf("\e[0;33mwall invalido:\e[0m falta mensaje\n");
		return;
	}
	struct utmp *ut;

    setutent();
    while ((ut = getutent()) != NULL) {
        if (ut->ut_type == USER_PROCESS) {
            char tty_path[64];
            snprintf(tty_path, sizeof(tty_path), "/dev/%s", ut->ut_line);
            int fd = open(tty_path, O_WRONLY | O_NONBLOCK);
            if (fd != -1) {
                dprintf(fd, "\nBroadcast message:\n%s\n", opcion+i);
                close(fd);
            }
        }
    }
    endutent();
}

void MESG(char *opcion)
{
	int i = espacios(opcion, 4);
	if (i == -1){
		printf("\e[0;33mComando invalido:\e[0m quizas es mesg\n");
		return;
	}
	if (i == 0){
		printf("\e[0;33mmesg invalido:\e[0m falta usuario y mensaje\n");
		return;
	}
	char name[29];
	int j = caracter(opcion, i);
	strncpy(name,opcion+i,j-i);
	i = espacios(opcion,j);
	if (i == 0){
		printf("\e[0;33mmesg invalido:\e[0m falta mensaje\n");
		return;
	}
	int bandera = 1;
	struct utmp *ut;

    setutent();
    while ((ut = getutent()) != NULL) {
        if (ut->ut_type == USER_PROCESS) {
            if (strcmp(name, ut->ut_user) == 0) {
				bandera = 0;
                char tty_path[64];
                snprintf(tty_path, sizeof(tty_path), "/dev/%s", ut->ut_line);
                int fd = open(tty_path, O_WRONLY | O_NONBLOCK);
                if (fd != -1) {
                    dprintf(fd, "\nBroadcast message:\n%s\n", opcion+i);
                    close(fd);
                }
            }
        }
    }
    endutent();
    if (bandera){
		printf("\e[0;33musuario invalido:\e[0m usuario no encontrado\n");
		return;
	}
}
