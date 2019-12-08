#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dlfcn.h>

#include "dlhal.h"

/*
 * Sample for the allwinner dl APIs
 *
 */
static char* get_file_name(char* path, int len)
{
	char* pt = path + len;
	while(*pt != '/' && pt != path)
	{
		pt--;
	}
	if(pt == path)
	return pt;
	return pt+1;
}

#define IPLOADER_SO_PATH "/system/lib/libip_loader.so"
int main(int argc, const char *argv[])
{
	void *handle;
	int (*sample)(char *);
	char *error;
	int ret ;
	char * filename = NULL;
	const char *filepath= argv[1];

	ret = aw_dlinit_ext(filepath, "-aes_sel" ,"H6", 32, NULL, NULL,NULL,IPLOADER_SO_PATH);
	if(ret <0){
		fprintf(stderr, "%s%d%s\n",__FILE__,__LINE__,dlerror());
		return -1;
	}

	handle = aw_dlopen(filename = get_file_name(filepath, strlen(filepath)), RTLD_LAZY);
	if (!handle) {
		fprintf(stderr, "%s\n", dlerror());
		return -1;
	}

	sample= (int(*)(char *)) aw_dlsym(handle, "sample");
	error = dlerror();
	if (error != NULL) {
		fprintf(stderr, "%s\n", error);
		return -1 ;
	}

	printf("%d\n", (*sample)("hello from dl caller\n"));
	aw_dlclose(filename, handle);

	exit(EXIT_SUCCESS) ;
}

