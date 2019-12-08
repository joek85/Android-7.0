/*
 * Copyright(c) 2013-2016 Allwinnertech Co., Ltd. 
 *
 *		http://www.allwinnertech.com
 *		Author: Ryan <ryanchen@allwinnertech.com>
 *
 * Allwinner IP protction library
 *
 */

#ifndef __AW_DLFCN_H__
#define __AW_DLFCN_H__

#ifdef __cplusplus  
extern "C" {  
#endif 

int aw_dlinit(const char *filename,const char *aes_opt, const char *aes_key, int aes_len ,\
				const char*rsa_file, const char *cid,const char *version);
void  *aw_dlopen(const char *, int);
void  *aw_dlsym(void *, const char *);
int    aw_dlclose(const char *,void *);
char  *aw_dlerror(void);

#ifdef __cplusplus  
}  
#endif  

#endif

