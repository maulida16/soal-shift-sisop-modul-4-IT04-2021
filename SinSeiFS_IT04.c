#define FUSE_USE_VERSION 28
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <sys/time.h>
#include <stdlib.h>

static const char *dirpath = "/home/gerry/Downloads/";
static const char *logpath = "/home/gerry/Documents/SinSeiFS.log";
char log1[1200], *desc;

void atbash(char *name) {
    if (strcmp(name, ".") == 0 || strcmp(name, "..") == 0) return;

    char *dot = strrchr(name, '.');
    char *atoz = strstr(name, "AtoZ_");
    int i;
    for (i = atoz - name; i < strlen(name); ++i) {
        if (name[i] == '/') {
            break;
        }
    }

    if (atoz == NULL) {
        i = 0;
    }

    int last = dot ? dot - name: strlen(name);
    for (; i < last; ++i) {
        if ('A' <= name[i] && name[i] <= 'Z') {
            name[i] = 155 - name[i];
        } else if ('a' <= name[i] && name[i] <= 'z') {
            name[i] = 219 - name[i];
        }
    }
}

void rot13(char *name) {
    if (strcmp(name, ".") == 0 || strcmp(name, "..") == 0) return;

    int name_len = strlen(name);
    for (int i = 0; i < name_len; ++i) {
        if ('A' <= name[i] && name[i] <= 'M') {
            name[i] = 13 + name[i];
        } else if ('N' <= name[i] && name[i] <= 'Z') {
            name[i] = -13 + name[i];
        } else if ('a' <= name[i] && name[i] <= 'm') {
            name[i] = 13 + name[i];
        } else if ('n' <= name[i] && name[i] <= 'z') {
            name[i] = -13 + name[i];
        }
    }

    char *dot = strrchr(name, '.');
    for (int i = (int)(dot - name); i < name_len; ++i) {
        if ('A' <= name[i] && name[i] <= 'M') {
            name[i] = 13 + name[i];
        } else if ('N' <= name[i] && name[i] <= 'Z') {
            name[i] = -13 + name[i];
        } else if ('a' <= name[i] && name[i] <= 'm') {
            name[i] = 13 + name[i];
        } else if ('n' <= name[i] && name[i] <= 'z') {
            name[i] = -13 + name[i];
        }
    }
}

void check_encryption(char *path, const char *fpath) {
    printf("check %s %s\n", path, fpath);
    if (strstr(fpath, "/AtoZ_") != NULL) {
        atbash(path);
    } else if (strstr(fpath, "/RX_") != NULL) {
        atbash(path);
        rot13(path);
    }
    printf("enc %s\n", path);
}

// nomer 4
void makelog(char *level,char *desc)
{
    FILE *fp = fopen(logpath,"a");
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    
    char waktu[1000];
    strftime(waktu, 100, "%y%m%d-%H:%M:%S", tm);
    
    sprintf(log1, "%s::%s::%s\n", level, waktu, desc);
    fputs(log1, fp);
    memset(log1, 0, sizeof(log1));
    
    fclose(fp);
}

static int xmp_getattr(const char *path, struct stat *st) {
    char fpath[2000], name[1000], temp[1000];
    sprintf(temp, "%s", path);

    int name_len = strlen(path);
    for (int i = 0; i < name_len; i++) {
        name[i] = path[i + 1];
    }
    printf("getattr %s\n", name);
    
    // vigenere_dec(name);
    // atbash(name);
    // rot13(name);
    check_encryption(temp, path);
    sprintf(fpath, "%s/%s", dirpath, temp);
    
    int res = lstat(fpath, st);
    if (res != 0){
        return -ENOENT;
    }

    return 0;
}

static int xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi) {
    int res;
    DIR *dp;
    struct dirent *de;

    (void) offset;
    (void) fi;
    char fpath[2000];
    char name[1000];

    if (strcmp(path, "/") == 0) {
        sprintf(fpath, "%s", dirpath);
    } 
    else {
        sprintf(name, "%s", path);
        // vigenere_dec(name);
        // atbash(name);
        // rot13(name);
        check_encryption(name, path);
        sprintf(fpath, "%s/%s", dirpath, name);
    }

    printf("readdir: %s\n", fpath);

    dp = opendir(fpath);
    if (dp == NULL)
        return -errno;

    while ((de = readdir(dp)) != NULL) {
        struct stat st;
        memset(&st, 0, sizeof(st));
        st.st_ino = de->d_ino;
        st.st_mode = de->d_type << 12;

        char fullpathname[2257];
        sprintf(fullpathname, "%s/%s", fpath, de->d_name);
        
        char temp[1000];
        strcpy(temp, de->d_name);
        // vigenere_enc(temp);
        // atbash(name);
        // rot13(name);
        check_encryption(temp, fpath);

        res = (filler(buf, temp, &st, 0));
        if (res != 0) break;
    }

    closedir(dp);

    return 0;
}

static int xmp_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
    char fpath[2000];
    char name[1000];

    if (strcmp(path, "/") == 0) {
        sprintf(fpath, "%s", dirpath);
    } else {
        sprintf(name, "%s", path);
        // vigenere_dec(name);
        // atbash(name);
        // rot13(name);

        check_encryption(name, path);
        sprintf(fpath, "%s/%s", dirpath, name);
    }

    printf("read %s\n", fpath);
    
    int res = 0;
    int fd = 0 ;

    (void) fi;
    fd = open(fpath, O_RDONLY);
    if (fd == -1)
        return -errno;

    res = pread(fd, buf, size, offset);
    if (res == -1)
        res = -errno;

    close(fd);
    return res;
}

static int xmp_mkdir(const char *path, mode_t mode)
{
    int res;
    char fpath[1000];
    sprintf(fpath,"%s%s",dirpath,path);
    
    res = mkdir(fpath, mode);
    if(res == -1)
        return -errno;
    desc=(char*)malloc((sizeof(char)*strlen(fpath))+strlen("MKDIR::")+100);
    sprintf(desc, "MKDIR::%s", fpath);
    makelog("INFO", desc);
    free(desc);
    
    return 0;
}

static int xmp_rmdir(const char *path)
{
 int res;
 char fpath[1000];
 sprintf(fpath,"%s%s",dirpath,path);

 res = rmdir(fpath);
 if(res == -1)
  return -errno;
 desc=(char*)malloc((sizeof(char)*strlen(fpath))+strlen("RMDIR::")+100);
 sprintf(desc, "RMDIR::%s", fpath);
 makelog("WARNING", desc);
 free(desc);
 
 return 0;
}

static int xmp_rename(const char *from, const char *to)
{
 int res;
     char frname[1000], toname[1000], frpath[1000], topath[1000];
     strcpy(frname, from);
    strcpy(toname, to);
     sprintf(frpath, "%s%s", dirpath, frname);
     sprintf(topath, "%s%s", dirpath, toname);

 res = rename(frpath, topath);
 if (res == -1)
  return -errno;
 desc=(char*)malloc((sizeof(char)*(strlen(frname)+strlen(toname)))+strlen("RENAME::")+100);
 sprintf(desc, "RENAME::%s::%s", frname, toname);
 makelog("INFO", desc);
 free(desc);

 return 0;
}

static int xmp_create(const char *path,mode_t mode, struct fuse_file_info *fi)
{
    int res;
    char fpath[1000];
    sprintf(fpath,"%s%s",dirpath,path);
    res = open(fpath, fi->flags, mode);
    
    if(res == -1)
        return -errno;
    fi->fh = res;
    desc=(char*)malloc((sizeof(char)*strlen(fpath))+strlen("CREATE::")+100);
    sprintf(desc, "CREATE::%s", fpath);
    makelog("INFO", desc);
    free(desc);
    
    return 0;
}

static int xmp_unlink(const char *path)
{
 int res;
 char fpath[1000];
 sprintf(fpath,"%s%s",dirpath,path);
 
 res = unlink(fpath);
 if (res == -1)
  return -errno;
 desc=(char*)malloc((sizeof(char)*strlen(fpath))+strlen("REMOVE::")+100);
 sprintf(desc, "REMOVE::%s", fpath);
 makelog("WARNING", desc);
 free(desc);

 return 0;
}

static struct fuse_operations xmp_oper = {
    .getattr = xmp_getattr,
    .readdir = xmp_readdir,
    .read = xmp_read,
    .mkdir = xmp_mkdir,
    .rmdir = xmp_rmdir,
    .rename = xmp_rename,
    .create = xmp_create,
    .unlink = xmp_unlink,
};

int main(int argc, char *argv[])
{
 umask(0);
 return fuse_main(argc, argv, &xmp_oper, NULL);
}
