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

#ifndef __CORE_FS_VERITY_FILE_H
#define __CORE_FS_VERITY_FILE_H



#define FILE_SIG_MAGIC          0x78381729U 

#define SHA256_DIGEST_LENGTH    32
#define SHA512_DIGEST_LENGTH    64
#define SHA_DIG_MAX             SHA512_DIGEST_LENGTH  

#define RSA2048_SIG_LENGTH      256
#define RSA3072_SIG_LENGTH      384
#define RSA_SIG_MAX             RSA3072_SIG_LENGTH 

#define FILE_NAME_LEN           512 
#define DIR_MAX_FILE_NUM        10000

struct FILE_LIST_HEAD{
	unsigned int magic;
	unsigned int version ;
	unsigned char sha_alg[8] ;
	unsigned char sig[RSA_SIG_MAX];
	unsigned char root_dir[64];
	unsigned int  file_cnt ;
	unsigned int  file_name_len ;
	unsigned int  reserved[4];
};

struct FILE_SIG_HEAD{
	unsigned int magic ;
	unsigned char sig[RSA_SIG_MAX];
	char sha_alg[8];
	char rsa_alg[8];
	unsigned char root_dir[64];
	unsigned int total_len;
	unsigned int actual_cnt ; /*actual file count*/
};
struct FILE_SIG{
	unsigned int crc ; /*name crc*/
	unsigned int flag ;
	unsigned char sha[SHA_DIG_MAX];
	unsigned int  name_len;
	unsigned char name[FILE_NAME_LEN];
};
struct FILE_Descriptor{
	char            name[FILE_NAME_LEN];
	unsigned int    length; 
	int             attr;
};

typedef struct{
	void			*sig_head;
	unsigned int	sig_head_size;
	void			*sig_table;
	unsigned int	sig_table_size;
}fivm_param_t ;

#define FIVM_DEV	"/dev/fivm"
#define CMD_FIVM_INIT     _IO('M', 0)
#define CMD_FIVM_ENABLE	  _IO('M', 1)
#define CMD_FIVM_SET	  _IOR('M', 2, fivm_param_t)

extern int fs_mgr_verity_file(char *mount_point) ;
#endif
