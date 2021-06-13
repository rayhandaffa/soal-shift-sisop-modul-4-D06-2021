# Sistem Operasi D-06 (2021)
Praktikum Sistem Operasi Modul 4 - FUSE
Kelompok : 
1. Ramadhan Arif - 05111940000162
2. Zulfayanti Sofia Solichin - 05111940000189
3. Rayhan Daffa Alhafish - 05111940000227

## Penjelasan dan Penyelesaian soal no.1
Soal nomer 1 ini kita diminta untuk melakukan encoding nama directory dan files dengan menggunakan Atbash Cipher (mirror-ing). Dimana path mount source (root) filesystem adalah berada di directory `/home/rayhandapis/Downloads`. Untuk menyelesaikan itu, perlu adanya sebuah fungsi untuk melakukan encoding. Pada program ini kami menggunakan funtion `enkripsiAtoZ()`
```
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
```
Pada function `enkripsiAtoZ()` ini pada awalnya diperlukan adanya pemisahan nama sebuah file dari extensionnya. Pemisahan tersebut menggunakan `strtok` dengan dilimiter `.`. Selanjutnya, nama file tersebut diambil, dan dari situ kita dapat melakukan sebuah looping pada setiap character untuk di encrypt.
- **Penjelasan dan Penyelesaian Soal 1a**<br>
Pada sub soal 1a ini terdapat sebuah kondisi yaitu jika sebuah direktori yang telah dibuat dengan awalan "AtoZ_" direktori tersebut akan menjadi sebuah direktori ter-encode. Untuk memenuhi kondisi tersebutmaka diperlukan sebuah field `xmp_mkdir`.
  ```
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
      
    return 0;
  }
  ```
   Dari potongan codingan di atas dapat dibuat sebuah asumsi, sebuah directory dengan awalan "AtoZ_", maka directory tersebut akan menjadi sebuah directory yang telah ter-encode. Sehingga, hasil dari folder di dalam nya akan ter-decode ketika melakukan renaming (Section 1b). Function `xmp_mkdir()` pada awalnya kita melakukan sebuah pengecekan apabila sebuah kondisi directory yang telah di buat dengan awal "AtoZ_" dengan menggunakan `strstr`, maka kita akan memanggil sebuah fungsi `logfileAtoZmkdir()` untuk menyimpan log (section 1d).
   
- **Penjelasan dan Penyelesaian Soal 1b**<br>
Pada soal sub 1b ini kita diminta untuk menyesuaikan sebuah kondisi dimana apabila sebuah directory di-rename dengan awalan "AtoZ_", maka directory tersebut akan menjadi direktori yang ter-encode. Untuk menyesuaikan kondisi tersebut kita perlu membuat dan me-remake sebuah field `xmp_rename`
  ```
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
  ```
  Pada function `xmp_rename` ini hal yang paling pertama dapat dilakukan adalah melakukan pengecekan sebuah direktori, apakah direktori tersebut mengandung "AtoZ_" dengan menggunakan `strstr`. Jika sebuah direktori tersebut mengandung "AtoZ_", maka kita perlu memanggil sebuah function `renameRecursively` (dijelaskan pada section selanjutnya).
- **Penjelasan dan Penyelesaian Soal 1c**<br>
Soal 1c, terdapat kondisi apabila direktori yang terenkripsi di rename menjadi tidak ter-encode, maka isi direktori tersebut akan ter-decode. Untuk memenuhi kondisi tersebut terdapat sebuah function "AtoZ_" maka direktori tersebut akan ter-decode. Proses decoding dapat dilakukan dengan memanggil sebuah function `renameRecursively`
- **Penjelasan dan Penyelesaian Soal 1d**<br>
Dari setiap proses yang dilakukan oleh Program maka akan dibuat sebuah direktori ter-encode dengan kondisi mkdir (make a directory) dan rename akan tercatat ke sebuah log. Log tersebut akan di simpan dimana program itu dijalan. 
  ``` 
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
  ```
- **Penjelasan dan Penyelesaian Soal 1e**<br>
Soal 1e ini semua metode yang encode pada suatu direktori akan berlaku terhadap direktori yang ada di dalam nya menggunakan sebuah rektursif. Oleh karena itu, diperlunya sebuah function bernama `renameRecursively`
  ```
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
          puts(file);
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
  ```
  Pada function `renameRecursively`, untuk melakukan sebuah recursive pada direktori-direktori di dalamnya sehingga dapat dilakukan sebuah while loop selama diretory nya tersebuh tidak sama dengan NULL artinya direktori tersebut ada. Proses tersebut akan berlangsung secara recursive terus menerus.
## Output
## Penjelasan dan Penyelesaian soal no.2
- **Penjelasan dan Penyelesaian Soal 2a**<br>
- **Penjelasan dan Penyelesaian Soal 2b**<br>
- **Penjelasan dan Penyelesaian Soal 2c**<br>
- **Penjelasan dan Penyelesaian Soal 2d**<br>
## Penjelasan dan Penyelesaian soal no.3
- **Penjelasan dan Penyelesaian Soal 3a**<br>
- **Penjelasan dan Penyelesaian Soal 3b**<br>
- **Penjelasan dan Penyelesaian Soal 3c**<br>
- **Penjelasan dan Penyelesaian Soal 3d**<br>
## Penjelasan dan Penyelesaian soal no.4
Pada soal nomor 4, kita diminta melakukan pencatatan setiap kegiatan pada filesystem pada file `SinSeiFS.log` pada sebuah pat bernama `pathLog = /home/rayhandapis/SinSeiFS.log`. Log tersebut akan terdapat dua buah level yaitu, `WARNING` dan `INFO`. `WARNING` digunakan untuk mencatat syscall rmdir (remove directory) dan unlink (remove file). Selanjutnya, `INFO` untuk mencatat semua syscall kecuali yang di panggil oleh `WARNING`. Untuk memenuhi dan menjawab semua itu, maka dibuah 2 buat function `logInfo()` dan `logWarning()`.
  ```
  //no 4
  //buat Log 
  // logInfo
  void logInfo(char *desk)
  {
      time_t now = time(NULL);
      struct tm *timeInfo = localtime(&now);
      FILE* fileLog = fopen(pathLog, "a");
      fprintf(fileLog,    "INFO::%02d%02d20%02d-%02d:%02d:%02d::%s\n",timeInfo->tm_mday,timeInfo->tm_mon+1,timeInfo->tm_year+1900,timeInfo->tm_hour,timeInfo->tm_min,timeInfo->tm_sec, desk);
      fclose(fileLog);
  }

  //logWarning
  void logWarning(char *desk)
  {
      time_t now = time(NULL);
      struct tm *timeInfo = localtime(&now);
      FILE* fileLog = fopen(pathLog, "a");
      fprintf(fileLog,  "WARNING::%02d%02d20%02d-%02d:%02d:%02d::%s\n",timeInfo->tm_mday,timeInfo->tm_mon+1,timeInfo->tm_year+1900,timeInfo->tm_hour,timeInfo->tm_min,timeInfo->tm_sec, desk);
      fclose(fileLog);
  }
  ```
  Kedua function tersebut memiliki cara kerja yang sama dimana menggunakan variabel `time_t now` dan sebuah struct `struct tm *timeInfo`. Setelah itu, dilakukan pemanggilan semua log berdasarkan format yang telah di tentukan yang dimana `logWarning` akan dipanggil pada sebuah `xmp_unlink` dan `xmp_rmdir`. dan untuk `logInfo` dipanggil pasa sisanya. Terakhir semuanya akan di print dengan menggunakan `fprintf` sesuai format yang ditentukan.
  Untuk `xmp_rmdir` dan `xmp_unlink` dapat dilihat di bawah ini: 
  ```
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
  ```
