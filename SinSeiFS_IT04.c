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

static const char *dirpath = "/home/gerry";
static const char *logpath = "/home/gerry/SinSeiFS.log";
char log1[1200], *desc;

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

static  int  xmp_getattr(const char *path, struct stat *stbuf)
{
    int res;
    char fpath[1000];

    sprintf(fpath,"%s%s",dirpath,path);

    res = lstat(fpath, stbuf);

    if (res == -1) return -errno;

    return 0;
}

static int xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi)
{
    char fpath[1000];

    if(strcmp(path,"/") == 0)
    {
        path=dirpath;
        sprintf(fpath,"%s",path);
    } else sprintf(fpath, "%s%s",dirpath,path);

    int res = 0;

    DIR *dp;
    struct dirent *de;
    (void) offset;
    (void) fi;

    dp = opendir(fpath);

    if (dp == NULL) return -errno;

    while ((de = readdir(dp)) != NULL) {
        struct stat st;

        memset(&st, 0, sizeof(st));

        st.st_ino = de->d_ino;
        st.st_mode = de->d_type << 12;
        res = (filler(buf, de->d_name, &st, 0));

        if(res!=0) break;
    }

    closedir(dp);

    return 0;
}

static int xmp_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi)
{
    char fpath[1000];
    if(strcmp(path,"/") == 0)
    {
        path=dirpath;

        sprintf(fpath,"%s",path);
    }
    else sprintf(fpath, "%s%s",dirpath,path);

    int res = 0;
    int fd = 0 ;

    (void) fi;

    fd = open(fpath, O_RDONLY);

    if (fd == -1) return -errno;

    res = pread(fd, buf, size, offset);

    if (res == -1) res = -errno;

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
