/*
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <libgen.h>
#include <time.h>
#include <dirent.h>

#include <logwrap/logwrap.h>

#include "mincrypt/rsa.h"
#include "mincrypt/sha.h"
#include "mincrypt/sha256.h"
#include <utils/Log.h>

#include "fs_mgr_priv.h"
#include "fs_mgr_priv_verity.h"

#include <fts.h>
#include "fs_mgr_fivm.h"

#define FILE_DESC 

#define VERITY_TABLE_RSA_KEY	"/verity_key"
#define VERITY_BLOCK			"/dev/block/by-name/verity_block"
#define VERITY_BLOCK_BACKUP			"/dev/block/by-name/Reserve1"
#define VERITY_DEBUG_FILE	"/file_list_target"

static int debug_en = 0;
static char *file_list = NULL ;

#define D(x...) \
	if(debug_en) \
		KLOG_INFO("",x);\
	else \
		do{}while(0)

static  void dump_buffer(void *buf, int len)
{
	int i;
	int line_counter = 0;
	int sep_flag = 0;
	int	addr = 0 ;

	unsigned char	line[16];
	line_counter = len>>4 ; 
	sep_flag = len & 0x0f; 

	if(debug_en){

		D("Dump buffer,  addr = 0x%p, len =0x%x\n",buf, len);

		for(i = 0 ;i < line_counter ; i+=1,addr+=16){
			memcpy(line, (void *)((ptrdiff_t)buf+(i<<4)), 16 );
		D("%08x : %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\n",
					addr ,
					*(unsigned char*)(line) , 
					*(unsigned char*)(line+1) , 
					*(unsigned char*)(line+2)  , 
					*(unsigned char*)(line+3)  , 
					*(unsigned  char*)(line+4) , 
					*(unsigned  char*)(line+5) , 
					*(unsigned char*)(line+6) , 
					*(unsigned char*)(line+7) , 
					*(unsigned char*)(line+8), 
					*(unsigned char*)(line+9), 
					*(unsigned char*)(line+10), 
					*(unsigned char*)(line+11), 
					*(unsigned char*)(line+12), 
					*(unsigned char*)(line+13), 
					*(unsigned char*)(line+14) ,
					*(unsigned char*)(line+15)) ;
	
		}

		memset(line, 0, 16);
		memcpy(line, (void *)((ptrdiff_t)buf + line_counter *16), sep_flag);

	if( len %16 ){
			D("%08x: %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\n",
					addr,
					*(unsigned char*)(line) , 
					*(unsigned char*)(line+1) , 
					*(unsigned char*)(line+2)  , 
					*(unsigned char*)(line+3)  , 
					*(unsigned  char*)(line+4) , 
					*(unsigned  char*)(line+5) , 
					*(unsigned char*)(line+6) , 
					*(unsigned char*)(line+7) , 
					*(unsigned char*)(line+8), 
					*(unsigned char*)(line+9), 
					*(unsigned char*)(line+10), 
					*(unsigned char*)(line+11), 
					*(unsigned char*)(line+12), 
					*(unsigned char*)(line+13), 
					*(unsigned char*)(line+14) ,
					*(unsigned char*)(line+15)) ;
		}
	}
}

static RSAPublicKey *load_key(char *path)
{
    FILE *f;
    RSAPublicKey *key;

    key = malloc(sizeof(RSAPublicKey));
    if (!key) {
        ERROR("Can't malloc key\n");
        return NULL;
    }

    f = fopen(path, "r");
    if (!f) {
        ERROR("Can't open '%s [%s] '\n", path, strerror(errno));
        free(key);
        return NULL;
    }

    if (!fread(key, sizeof(*key), 1, f)) {
        ERROR("Could not read key![%s]\n", strerror(errno));
        fclose(f);
        free(key);
        return NULL;
    }
	
    if (key->len != RSANUMWORDS) {
        ERROR("Invalid key length %d\n", key->len);
		D("Read Key dump: \n");
		dump_buffer(key,sizeof(*key));
        fclose(f);
        free(key);
        return NULL;
    }

    fclose(f);
    return key;
}

static int verify_table(char *signature, char *table, int table_length)
{
	int fd;
	RSAPublicKey *key;
	uint8_t hash_buf[SHA256_DIGEST_SIZE];
	int retval = -1;

	SHA256_hash((uint8_t*)table, table_length, hash_buf);

	key = load_key(VERITY_TABLE_RSA_KEY);
	if (!key) {
		ERROR("Couldn't load verity keys \n");
		goto out;
	}
	// verify the result
	if (!RSA_verify(key,
				(uint8_t*) signature,
				RSANUMBYTES,
				(uint8_t*) hash_buf,
				SHA256_DIGEST_SIZE)) {

		fd = open(VERITY_DEBUG_FILE, O_CREAT|O_RDWR|O_TRUNC, 0777);
		if (fd > 0)
		{
			write(fd, table, table_length);
		}
		else
		{
			ERROR("open VERITY_DEBUG_FILE failed\n");
		}
		close(fd);
		ERROR("Couldn't verify table. table_len:0x%x\n", table_length);
		D("Hash input the first 64 byte of table is :\n");
		dump_buffer(table, 64);
		D("SHA256_of_table is :\n");
		dump_buffer(hash_buf, 32);

		D("RSA signature input :\n");
		dump_buffer(signature,RSANUMBYTES);
		D("RSA key input :\n");
		dump_buffer(key,sizeof(*key));

		goto out;
	}

	retval = 0;

out:
	free(key);
	return retval;
}

static void dump_filelist(char *file_list, unsigned int count)
{

	if(debug_en){
		D("Dump file list\n");	
		unsigned int i  ;
		char *ps = file_list;
		for( i = 0 ; i< count ; i++){
			D("%s\n", ps);
			ps += FILE_NAME_LEN; 
		}
	}
}

static void dump_filedesc(struct FILE_Descriptor *file_desc,
unsigned int count)
{
	if(debug_en){
		D("Dump file desc\n");	
		unsigned int i  ;
		for( i = 0 ; i< count ; i++){
			D("		->name: %s\n",file_desc[i].name);
			D("		->length: %d\n",file_desc[i].length);
		}
	}
}
static int getpath_size(char *path)
{

	FILE *file ;
	int size ;

	if(!(file=fopen(path, "rb"))){
		ERROR("ERROR: open file %s fail\n", path);
		return -1 ;
	}

	if(fseek(file, 0L, SEEK_END) == -1)
		return -1 ;

	size= ftell(file);

	fclose(file);
	return size ;
}

static int __list_1st_depth(char * mount_point)
{
	DIR *midir;
	struct dirent* info_archivo;
	struct stat fileStat;
	char fullpath[256];
	char stats[32];


	midir=opendir(mount_point);
	if( midir == NULL)
	{
		ERROR("Error in opendir\n");
		return -1; 
	}

	while ((info_archivo = readdir(midir)) != 0)
	{
		INFO("%s ", info_archivo->d_name);
		strcpy (fullpath, mount_point);
		strcat (fullpath, "/");
		strcat (fullpath, info_archivo->d_name);
		memset(&stats[0],0, 32 );
		if (!stat(fullpath, &fileStat))
		{
			memcpy(&stats[0],((S_ISDIR(fileStat.st_mode))  ? "d" : "-"), 1);
			memcpy(&stats[1],((fileStat.st_mode & S_IRUSR) ? "r" : "-"), 1);
			memcpy(&stats[2],((fileStat.st_mode & S_IWUSR) ? "w" : "-"), 1);
			memcpy(&stats[3],((fileStat.st_mode & S_IXUSR) ? "x" : "-"), 1);
			memcpy(&stats[4],((fileStat.st_mode & S_IRGRP) ? "r" : "-"), 1);
			memcpy(&stats[5],((fileStat.st_mode & S_IWGRP) ? "w" : "-"), 1);
			memcpy(&stats[6],((fileStat.st_mode & S_IXGRP) ? "x" : "-"), 1);
			memcpy(&stats[7],((fileStat.st_mode & S_IROTH) ? "r" : "-"), 1);
			memcpy(&stats[8],((fileStat.st_mode & S_IWOTH) ? "w" : "-"), 1);
			memcpy(&stats[9],((fileStat.st_mode & S_IXOTH) ? "x" : "-"), 1);
			INFO("%s \n", stats);	
		} else
		{
			ERROR("Error in stat\n");
		}
		INFO("\n");
	}
	closedir(midir);
	return 0;
	
}

static int __list_dir(char * mount_point )
{
	FTS *ftsp;
	FTSENT *p, *chp;
	int fts_options = FTS_COMFOLLOW | FTS_LOGICAL | FTS_NOCHDIR;
	int rval = 0;
	int index =0, file_cnt ;
	char full_name[FILE_NAME_LEN];
	char *const argv[]={ mount_point, NULL};

	struct FILE_Descriptor *file_desc ;

	INFO("Travers %s file list\n", mount_point);	
	if ((ftsp = fts_open(argv, fts_options, NULL)) == NULL) {
		ERROR("fts_open\n");
		return -1;
	}

	chp = fts_children(ftsp, 0);
	if (chp == NULL) {
		ERROR("no files to traverse \n");
		return -1;              
	}

	while ((p = fts_read(ftsp)) != NULL) {
		switch (p->fts_info) {
			case FTS_D:
				break;
			case FTS_F:
				INFO("%s\n",p->fts_path);
				break;
			default:
				break;
		}
	}
out:
	fts_close(ftsp);
	return 0;
}

#define NEW_TRAVERSE 1
static unsigned int g_rel_file_cnt = 0;
static int traverse_ptree_new(char * mount_point, char * file_list, unsigned int fcnt)
{
	DIR *dirp = NULL;
	int ret = 0;
	struct dirent *denty = NULL;
	struct stat stat_info;
	struct FILE_Descriptor *file_desc = NULL;
	char full_name[FILE_NAME_LEN] = {0};
	memset(&stat_info, 0, sizeof(stat_info));
	if(!(file_list&&fcnt))
	{
		ERROR("dst list is null");
		return -1;
	}
	dirp = opendir(mount_point);
	if(!dirp)
		return -1;
	/*traversed file cnt is enouth, the real targe fs be added new files*/
	if (g_rel_file_cnt > fcnt)
		return -1;
	while((denty = readdir(dirp)) != NULL)
	{
		memset(&stat_info, 0, sizeof(stat_info));
		memset(full_name, 0, FILE_NAME_LEN);
		strcpy(full_name, mount_point);
		strcat(full_name,"/");
		strcat(full_name, denty->d_name);
		if((strcmp(denty->d_name,".") == 0) || (strcmp(denty->d_name,"..") == 0))
			continue;
		ret = lstat(full_name, &stat_info);
		if(ret < 0)
		{
			ERROR("lstat :%s failed\n", full_name);
			continue;
		}
		if (S_ISDIR(stat_info.st_mode))
		{
			//ERROR("%s\n", full_name);
			traverse_ptree_new(full_name, file_list, fcnt);
			continue;
		}
		if ((S_ISREG(stat_info.st_mode)) ||(S_ISLNK(stat_info.st_mode))||
			(S_ISFIFO(stat_info.st_mode))||(S_ISSOCK(stat_info.st_mode)))
		{
			file_desc = (struct FILE_Descriptor *)file_list ;
			//ERROR("%s\n", full_name);
			memcpy(file_desc[g_rel_file_cnt].name, full_name,
					strnlen(full_name, FILE_NAME_LEN));
			/*file_desc[g_rel_file_cnt].length = stat_info.st_size;*/
			file_desc[g_rel_file_cnt].length = getpath_size(full_name);
			g_rel_file_cnt++;
		}
	}
	chdir("..");
	closedir(dirp);
	return 0;
}

static int traverse_ptree(char * mount_point, char * file_list,
int file_count  )
{
	FTS *ftsp;
	FTSENT *p, *chp;
	int fts_options = FTS_COMFOLLOW | FTS_LOGICAL | FTS_NOCHDIR;
	int rval = 0;
	int index =0, file_cnt ;
	char full_name[FILE_NAME_LEN];
	char *const argv[]={ mount_point, NULL};

	struct FILE_Descriptor *file_desc = NULL;

	INFO("Travers %s file list\n", mount_point);	
	if ((ftsp = fts_open(argv, fts_options, NULL)) == NULL) {
		ERROR("fts_open\n");
		return -1;
	}

	chp = fts_children(ftsp, 0);
	if (chp == NULL) {
		ERROR("no files to traverse \n");
		return -1;              
	}

	file_cnt = file_count;
	while ((p = fts_read(ftsp)) != NULL) {
		switch (p->fts_info) {
			case FTS_D:
				break;
			case FTS_SL:
			case FTS_NSOK:
			case FTS_NS:
			case FTS_F:
			case FTS_SLNONE:
				sprintf(full_name, "%s", p->fts_path);
#ifndef FILE_DESC
				memcpy(file_list+FILE_NAME_LEN*index,
						full_name,
						strnlen(full_name, FILE_NAME_LEN));
#else
				file_desc = (struct FILE_Descriptor *)file_list ;
				memcpy(file_desc[index].name,
							full_name,
							strnlen(full_name, FILE_NAME_LEN));
				file_desc[index].length = getpath_size(p->fts_path);
#endif
				if (p->fts_info == FTS_SLNONE)
				{
					ERROR("FTS_SLNONE found in target /system \n");
					file_desc[index].length = 0;
				}
				//ERROR("%s\n", full_name);
				index ++;
				if( --file_cnt == 0  ) 
				{
					goto out ;
				}
				break;
			default:
				break;
		}
	}
out:
	fts_close(ftsp);
	return 0;
}


static int fivm_ioctl(int ctl, void *param)
{
	int retval =-1;
    int fd;


    if ((fd = open(FIVM_DEV , O_RDWR)) < 0) {
        ERROR("Error opening fivm device  (%s)\n", strerror(errno));
		return 0 ;
	}
	
    if (ioctl(fd, ctl , param)) {
        ERROR("Error ioctl fivm device fail (%s)\n", strerror(errno));
		close(fd);
        return retval;
	}

	close(fd);
	return 0 ;
}

static int fs_mgr_init_verity_file(void) 
{
	return fivm_ioctl(CMD_FIVM_INIT, NULL);
}

static int read_file_sig_table(
		void **sig_head, unsigned int *sig_head_size,
		void **sig_table, unsigned int *sig_table_size
		)
{
	int cnt,rlen , 
		rc = -1 ,
		offset ;
	struct FILE_SIG_HEAD *file_sig_head = NULL ;
	char * file_sig_table = NULL ;
	FILE *f = NULL ;

	rlen = sizeof(struct FILE_SIG_HEAD); 
	offset = sizeof(struct FILE_LIST_HEAD);

	file_sig_head =(struct FILE_SIG_HEAD *) malloc(sizeof(*file_sig_head));
	if(!file_sig_head){
		ERROR(" out of memory\n");
		return -1 ;
	}

	if(!(f = fopen(VERITY_BLOCK, "r")) ){
        INFO("Error opening fivm block  (%s: %s)\n", VERITY_BLOCK,strerror(errno));
		goto try_backup ;	
	}

	fseek(f, offset, SEEK_SET);

    if (fread(file_sig_head, rlen, 1, f) != 1) {
        ERROR("Could not read file list head [%s]\n",strerror(errno));
		goto try_backup ;
    }

	if( file_sig_head->magic != FILE_SIG_MAGIC ){
		ERROR("Read file %s fail\n", VERITY_BLOCK);
		goto try_backup ;
	}
	rc = 0;
	goto read_pass ;

try_backup:
	if(!(f = fopen(VERITY_BLOCK_BACKUP, "r")) ){
        ERROR("Error opening fivm block  (%s: %s)\n",
        VERITY_BLOCK_BACKUP,strerror(errno));
		goto out ;	
	}
	fseek(f, offset, SEEK_SET);

    if (fread(file_sig_head, rlen, 1, f) != 1) {
        ERROR("Could not read file list head [%s]\n",strerror(errno));
		goto out;
    }

	if( file_sig_head->magic != FILE_SIG_MAGIC ){
		ERROR("Read file %s fail\n", VERITY_BLOCK_BACKUP);
		goto out;
	}

read_pass:
    cnt = file_sig_head->actual_cnt ;
	if( cnt > DIR_MAX_FILE_NUM ){
		ERROR("out of files count 0x%x\n",cnt);
		goto out ;
	}

	rlen = sizeof(struct FILE_SIG) * cnt; 
	file_sig_table = malloc(rlen );
	if(!file_sig_table){
		ERROR(" out of memory\n");
		goto out ;
	}

    if (fread(file_sig_table, rlen, 1, f) != 1) {
        ERROR("Could not read file sig table [%s]\n ", strerror(errno));
		goto out ;
    }
	
	*sig_head = file_sig_head ;
	*sig_head_size = sizeof(*file_sig_head) ;

	*sig_table = file_sig_table;
	*sig_table_size = rlen ; 
	rc = 0 ;

out :
	if(f)
		fclose(f);
	if(rc && file_sig_head)
		free(file_sig_head);
	if(rc && file_sig_table)
		free(file_sig_table);

	return rc ;
}

static int fs_mgr_enable_verity_file(void) 
{
	fivm_param_t param;
	int rc ;
	
	void * sig_head = NULL, *sig_table = NULL;
	unsigned int  sig_head_size = 0, sig_table_size =0;

	memset(&param,0,sizeof(fivm_param_t));

	rc = read_file_sig_table(
			&sig_head, &sig_head_size,
			&sig_table, &sig_table_size
			);
	if(rc < 0){
		ERROR("read file sig table fail\n");
		return -1; 
	}

#if 0
	D("Sig_head:\n");
	dump_buffer(sig_head, sig_head_size);
	D("Sig_table:\n");
	dump_buffer(sig_table, 16*sizeof(struct FILE_SIG));
#endif

	if (verify_table( (char *) ((struct FILE_SIG_HEAD *)sig_head)->sig,
				sig_table,
				sig_table_size) < 0) {
		ERROR(" verify file sig table fail\n");
		free(sig_head);
		free(sig_table);
		return -1;
	}

	param.sig_head=sig_head;
	param.sig_head_size=sig_head_size;
	param.sig_table=sig_table ;
	param.sig_table_size=sig_table_size;

#if 0
	D("Fivm set param:\n");
	dump_buffer(param, sizeof(param));
#endif 
	if( fivm_ioctl(CMD_FIVM_SET, &param) <0){
		ERROR("fivm cmd set fail\n");
		free(sig_head);
		free(sig_table);
		return -1 ;
	}

	free(sig_head);
	free(sig_table);

	return fivm_ioctl(CMD_FIVM_ENABLE, NULL);
}


int cmp_file_name( const void * a, const void *b)
{
	int ret ;
	ret =memcmp( a, b, FILE_NAME_LEN ) ;
	if(ret <0)
		return -1 ;
	else
		return 1 ;

	return ret ;
}

int cmp_file_desc( const void * a, const void *b)
{
	int ret ;
	char *a_name = ((struct FILE_Descriptor *)a)->name,
		 *b_name = ((struct FILE_Descriptor *)b)->name ;

	ret =memcmp( a_name, b_name, FILE_NAME_LEN ) ;
	if(ret <0 )
		return -1 ;
	else 
		return 1;
}

static int sort_file_desc(void *in, unsigned int cnt)
{
	qsort(in,cnt,sizeof(struct FILE_Descriptor),cmp_file_desc);
	return 0 ;
}
static int sort_file_list(void *in, unsigned int cnt)
{
	 qsort(in, cnt, FILE_NAME_LEN, cmp_file_name);
	 return 0;
}


static int load_file_list_head(void ** tag_buf)
{
	struct FILE_LIST_HEAD * file_list_head = NULL;
	int len ;
	FILE *f;

	len = sizeof(*file_list_head);
	file_list_head = (struct FILE_LIST_HEAD * )malloc(len) ;
	if( !file_list_head ){
		ERROR("load_file_list_head: cant malloc list head \n");
	return 0 ;
		return -1;
	}

	if(!(f = fopen(VERITY_BLOCK, "r")) ){
        INFO("Error opening verity block  (%s)\n", VERITY_BLOCK );
		goto try_backup ;
	}

    if (fread(file_list_head, len, 1, f) != 1) {
        ERROR("Could not read file list head\n");
		fclose(f);
		goto try_backup;
    }

	if(file_list_head->magic != FILE_SIG_MAGIC ){
		ERROR("Wrong format of file list head magic\n");
		dump_buffer((void *)file_list_head, sizeof(*file_list_head));
		fclose(f);
		goto try_backup;
	}

	if(file_list_head->file_cnt > DIR_MAX_FILE_NUM || 
			file_list_head->file_name_len >FILE_NAME_LEN){
		ERROR("Wrong file list count or name\n");
		fclose(f);
		goto try_backup;
	}
	goto done ;

try_backup:
	ERROR("Use backup VERITY_BLOCK_BACKUP\n");
	if(!(f = fopen(VERITY_BLOCK_BACKUP, "r")) ){
        ERROR("Error opening fivm block  (%s: %s)\n",
        VERITY_BLOCK_BACKUP,strerror(errno));
		free(file_list_head);
		return -1 ;
	}

    if (fread(file_list_head, len, 1, f) != 1) {
        ERROR("Could not read file list head\n");
		free(file_list_head);
		fclose(f);
		return -1 ;
    }

	if(file_list_head->magic != FILE_SIG_MAGIC ){
		ERROR("Wrong format of file list head magic\n");
		free(file_list_head);
		fclose(f);
		return -1 ;
	}

	if(file_list_head->file_cnt > DIR_MAX_FILE_NUM || 
			file_list_head->file_name_len >FILE_NAME_LEN){
		ERROR("Wrong file list count or name\n");
		free(file_list_head);
		fclose(f);
		return -1  ;
	}

done:
	*tag_buf =(void *)file_list_head ;
	fclose(f);
	return 0 ; 

}

int fs_mgr_verity_file(char *mount_point)
{

	char * file_list =NULL;
	struct FILE_Descriptor *file_desc = NULL;
	struct FILE_LIST_HEAD * file_list_head = NULL;
	int	retval = -1 ;
	unsigned int cnt, len;

	INFO("AW FIVM: Version 1.0\n");
	INFO("Verify %s file list\n", mount_point);

#if 0
	__list_1st_depth("/");
#endif
	/*Load file list head*/
	if( load_file_list_head( (void *)&file_list_head) <0 ){
		ERROR("load file list head fail\n");
		goto out ;
	}

	if( memcmp( file_list_head->root_dir,
				mount_point,
				strnlen(mount_point,FILE_NAME_LEN )) ){
		ERROR("Wrong mount point, [verify]%s VS [fs_tab]%s \n",
				file_list_head->root_dir,
				mount_point);
		goto out ;
	}

	/*Go around the mount point file-system  */
	cnt = file_list_head->file_cnt ;
#ifndef FILE_DESC
	len = cnt * file_list_head->file_name_len ;
	file_list =(char *)malloc(len);
	if( !file_list ){
		ERROR("fs_mgr_verity_file: cant malloc file list\n");
		goto out ;
	}
	if( traverse_ptree(mount_point, file_list, cnt  ) <0 ){
		ERROR("Traverse system ptree fail \n");
		goto out ;
	}

	sort_file_list( file_list, cnt);

	/*Verity the actual file list signature */
	if (verify_table( (char *)file_list_head->sig,
				file_list,
				len) < 0) {

		ERROR("fs_mgr_verity_file: rsa verify fail\n");
		dump_filelist(file_list, cnt);
		goto out;
	}
#else
	len = cnt * sizeof(struct FILE_Descriptor);
	file_desc = malloc(len + 10);
	if( !file_desc ){
		ERROR("fs_mgr_verity_file: cant malloc file desc\n");
		goto out ;
	}
	memset(file_desc, 0, len + 10);
	INFO("traverse_ptree_debug: %s file_count is %d\n", mount_point, cnt);

	#ifdef NEW_TRAVERSE
	INFO("New Traverse /system begin \n");
	g_rel_file_cnt = 0;
	traverse_ptree_new(mount_point, ((char *)file_desc), cnt);
	INFO("New Traverse /system end \n");
	INFO("traverse_ptree_debug: %s g_rel_file_cnt is %d\n", mount_point, g_rel_file_cnt);
	if (g_rel_file_cnt != cnt)
	{
		ERROR("ERR:file cnt not match!!!\n");
		goto out;
	}
	#else
	if( traverse_ptree(mount_point, (char *)file_desc, cnt  ) <0 ){
		ERROR("Traverse system ptree fail \n");	
		goto out ;
	}

	#endif

	sort_file_desc( file_desc, cnt);

	if (verify_table( (char *)file_list_head->sig,
				(char *)file_desc,
				len) < 0) {

		ERROR("fs_mgr_verity_file: rsa verify fail\n");
		D("file desc signature:\n");
		dump_buffer(file_list_head->sig, 256);
		dump_filedesc(file_desc, cnt);
		goto out;
	}
#endif

	if( fs_mgr_enable_verity_file() <0){
		ERROR("fs_mgr_verity_file: enable fivm driver fail \n");
		goto out ;
	}

	retval = 0;
out:
	INFO("File-system [%s] list verify %s\n",mount_point,
			(retval ==0) ? "Pass" : "Fail");

	if(file_list_head)
		free(file_list_head);
	if(file_list )
		free(file_list);
	if(file_desc)
		free(file_desc);
	if (debug_en)
	return 0;
	return retval ;
}


