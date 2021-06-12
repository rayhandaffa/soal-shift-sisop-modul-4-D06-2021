#define FUSE_USE_VERSION 28
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <sys/time.h>

static const char *pathdirektory = "/home/rayhandapis/Downloads";
static const char *pathLog = "/home/rayhandapis/SinSeiFS.log";

//no 1
void enkripsiAtoz(char *pathNya)
{
    if(!strcmp(pathNya, ".") || !strcmp(pathNya, ".."))
    {
        return;
    }
    char *temp = strtok(pathNya, ".");
    int selesai = strlen(temp);

    for(int i = 0; i < selesai; i++)
    {
        while(pathNya[i] != '\0')
        {
            if(!((pathNya[i]>=0&&pathNya[i]<65)||(pathNya[i]>90&&pathNya[i]<97)||(pathNya[i]>122&&pathNya[i]<=127)))
            {
                if(pathNya[i]>='A'&&pathNya[i]<='Z')
                {
                    pathNya[i] = 'Z'+'A'-pathNya[i];
                }

                if(pathNya[i]>='a'&&pathNya[i]<='z')
                {
                    pathNya[i] = 'z'+'a'-pathNya[i];
                }
            }

            if(!((pathNya[i]>=0&&pathNya[i]<65)||(pathNya[i]>90&&pathNya[i]<97)||(pathNya[i]>122&&pathNya[i]<=127)))
            {
                pathNya[i] = pathNya[i];
            }
            i++;
        }

    }
}

void renameRecursively(char *daripathNya)
{
    DIR *dirent;
    struct dirent *direntInit;
    char file[100];
    char extFile[100];
    char sebelumPath[1100];
    char sesudahPath[1100];

    dirent = opendir(daripathNya);

    if(dirent == NULL)
    {
        return;
    }

    while((direntInit = readdir(dirent))!= NULL)
    {
        if(strcmp(direntInit->d_name, "..") == 0|| strcmp(direntInit->d_name, ".") == 0)
        {
            continue;
        }

        if(direntInit->d_type == DT_DIR)
        {
            strcpy(file, direntInit->d_name);
            sprintf(sebelumPath, "%s/%s", daripathNya, file);
            renameRecursively(sebelumPath);
        }

        strcpy(file, direntInit->d_name);
        strcpy(extFile, file);
        sprintf(sebelumPath, "%s/%s", daripathNya, file);

        char *extention = strrchr(extFile, '.');

        enkripsiAtoz(file);

        //ketika ada extensinya
        if(extention != NULL)
        {
            sprintf(sesudahPath, "%s/%s%s", daripathNya, file, extention);
            rename(sebelumPath, sesudahPath);
        }

        //ketika tidak ada extensinya
        else if(extention != NULL)
        {
            sprintf(sesudahPath, "%s/%s", daripathNya, file);
            rename(sebelumPath, sesudahPath);
        }
    }
    closedir(dirent);
};

//1d
//log file mkdir atoz
void logfileAtozmkdir(char *pathNya)
{
    FILE *mkdirlogFile;
    mkdirlogFile = fopen("atoz.log", "a");
    fprintf(mkdirlogFile, "mkdir: %s\n", pathNya);
}

//log file rename atoz
void logfileAtozrename(char *pathNya, char *pathNya2)
{
    FILE *renamelogFile;
    renamelogFile = fopen("atoz.log", "a");
    fprintf(renamelogFile, "%s-->%s\n", pathNya, pathNya2);
}

//no 4
//buat Log 
// logInfo
void logInfo(char *desk)
{
    time_t now = time(NULL);
    struct tm *timeInfo = localtime(&now);
    FILE* fileLog = fopen(pathLog, "a");
    fprintf(fileLog, "INFO::%02d%02d20%02d-%02d:%02d:%02d::%s\n",timeInfo->tm_mday,timeInfo->tm_mon+1,timeInfo->tm_year+1900,timeInfo->tm_hour,timeInfo->tm_min,timeInfo->tm_sec, desk);
    fclose(fileLog);
}

//logWarning
void logWarning(char *desk)
{
    time_t now = time(NULL);
    struct tm *timeInfo = localtime(&now);
    FILE* fileLog = fopen(pathLog, "a");
    fprintf(fileLog, "WARNING::%02d%02d20%02d-%02d:%02d:%02d::%s\n",timeInfo->tm_mday,timeInfo->tm_mon+1,timeInfo->tm_year+1900,timeInfo->tm_hour,timeInfo->tm_min,timeInfo->tm_sec, desk);
    fclose(fileLog);
}

/*XMP FIELD*/
//get attribute files
static int xmp_getattr(const char *pathNya, struct stat *stbuf)
{
    int temp;
    char filePath[1000];

    sprintf(filePath, "%s%s", pathdirektory, pathNya);

    temp = lstat(filePath, stbuf);

    //jika gagal
    if(temp == -1)
    {
        return -errno;
    }
    return 0;
}

//MKDIR ->make a directory
static int xmp_mkdir(const char *pathNya, mode_t modeT)
{
    int temp;
    char filePath[1000];

    sprintf(filePath, "%s%s", pathdirektory, pathNya);

    //buat file yang terdekode
    if(strstr(filePath, "AtoZ_"))
    {
        logfileAtozmkdir(filePath);
    }

    temp = mkdir(filePath, modeT);

    //jika gagal
    if(temp == -1)
    {
        return -errno;
    }

    //no 4 log mkdir
    char desk[1500];
    sprintf(desk, "MKDIR::%s", filePath);
    logInfo(desk);
    
    return 0;
}

//RENAME
static int xmp_rename(const char *dari, const char *menuju)
{
    int temp; 
    char daripathNya[1000];
    char menujupathNya[1000];

    sprintf(daripathNya, "%s%s", pathdirektory, dari);
    sprintf(menujupathNya, "%s%s", pathdirektory, menuju);

    //AtoZ -> normal decode
    if(strstr(daripathNya, "AtoZ_"))
    {
        if(!strstr(menujupathNya, "AtoZ_"))
        {
            renameRecursively(daripathNya);
        }
        
    }

    //normal decode -> AtoZ
    if(!strstr(daripathNya, "AtoZ_"))
    {
        if(strstr(menujupathNya, "AtoZ_"))
        {
            renameRecursively(daripathNya);
            // logfileAtozrename(daripathNya, menujupathNya);
        }
        logfileAtozrename(daripathNya, menujupathNya);
    }

    temp = rename(daripathNya, menujupathNya);

    //jika gagal 
    if(temp == -1)
    {
        return -errno;
    }

    //no 4 rename
    char desk[2500];
    sprintf(desk, "RENAME::%s::%s", daripathNya, menujupathNya);
    logInfo(desk);
    return 0;
}

//READDIR
static int xmp_readdir(const char *pathNya, void *buf, fuse_fill_dir_t filler, off_t setOff, struct fuse_file_info *ffi)
{
    char filePath[1000];

    if(strcmp(pathNya, "/") == 0)
    {
        pathNya = pathdirektory;
        sprintf(filePath, "%s", pathNya);
    }
    else 
    {
        sprintf(filePath, "%s%s", pathdirektory, pathNya);
    }

    int temp = 0; 
    DIR *dirent;
    struct dirent *direntInit;
    (void) setOff;
    (void) ffi;

    dirent = opendir(filePath);

    if(dirent == NULL)
    {
        return -errno;
    }

    while ((direntInit = readdir(dirent)) != NULL)
    {
        struct stat sta;

        memset(&sta, 0, sizeof(sta));

        sta.st_ino = direntInit->d_ino;
        sta.st_mode = direntInit->d_type << 12;
        temp = (filler(buf, direntInit->d_name, &sta, 0));//kalo ga berubah

        if(temp != 0)
        {
            break;
        }
    }
    closedir(dirent);

    return 0;
}

//READ
static int xmp_read(const char *pathNya, char *buf, size_t ukuran, off_t setOff, struct fuse_file_info *ffi)
{
    char filePath[1000];
    
    //getdir file nya
    if(strcmp(pathNya, "/") == 0)
    {
        pathNya = pathdirektory;
        sprintf(filePath, "%s", pathNya);
    }
    else 
    {
        sprintf(filePath, "%s%s", pathdirektory, pathNya);
    }

    int temp = 0, filedirektory =0;

    (void) ffi;
    filedirektory = open(filePath, O_RDONLY);

    //jika gagal
    if(filedirektory == -1)
    {
        return -errno;
    }

    temp = pread(filedirektory, buf, ukuran, setOff);

    //jika gagal
    if(temp == -1)
    {
        return -errno;
    }

    close(filedirektory);

    return temp;


}
//RMDIR
static int xmp_rmdir(const char *pathNya)
{
    int simp;
    char filePath[1000];

    sprintf(filePath, "%s%s", pathdirektory, pathNya);

    simp = rmdir(filePath);
    if(simp == -1)
    {
        return -errno;
    }

    //warning in unlink
    char desk[1512];
    sprintf(desk, "RMDIR::%s", filePath);
    logWarning(desk);

    return 0;
}
//UNLINK 
static int xmp_unlink(const char *pathNya)
{
    int simp;
    char filePath[1000];

    sprintf(filePath, "%s%s", pathdirektory, pathNya);

    simp = unlink(filePath);
    if(simp == -1)
    {
        return -errno;
    }

    //warning in unlink
    char desk[1512];
    sprintf(desk, "UNLINK::%s", filePath);
    logWarning(desk);

    return 0;
}
static struct fuse_operations xmp_oper = {
    .getattr = xmp_getattr,
    .mkdir = xmp_mkdir,
    .rename = xmp_rename,
    .readdir = xmp_readdir,
    .read = xmp_read,
    .unlink = xmp_unlink,
    .rmdir = xmp_rmdir
};

int main(int argc, char *argv[])
{
    umask(0);
    return fuse_main(argc, argv, &xmp_oper, NULL);
}
