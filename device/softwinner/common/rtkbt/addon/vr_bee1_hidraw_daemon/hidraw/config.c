#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <cutils/log.h>

#include "config.h"


tVoice_Hidraw_Settings *gConfig = NULL;
static char * gContent = NULL;


static char * voice_conf_getline(char * head, char ** left)
{
    char * next;
    if(*head == 0)
        return NULL;
    while((*head == '\r')||(*head == '\n'))
        head++;
    if(*head == 0)
        return NULL;
    next = head;
    while((*next != '\r')&&(*next != '\n')&&(*next != 0))
        next++;
    if(*next != 0)
        *next++ = 0;
    *left = next;
    return head;
}

static void voice_conf_parse_config(tVoice_Hidraw_Settings **items, char *content)
{
    char * group = NULL;
    char * item = NULL;
    char * val = NULL;
    char * line = NULL;
    int id = -1;
    tVoice_Hidraw_Settings* entry, * head, *end;

    char * p;
    int i = 0;

    head = NULL;
    end = NULL;

    ALOGD("voice_conf_parse_config\n");
    while((line = voice_conf_getline(content, &content)) != NULL)
    {
        ALOGD("voice_conf_parse_config line:%s\n", line);
        switch(line[0])
        {
            case '[':
            {
                p = strstr(line,"]");
                if(p)
                {
                    *p = 0;
                    group = &(line[1]);
                    ALOGI("voice_conf_parse_config Find Group[%s]\n", group);
                }else{
                    ALOGW("voice_conf_parse_config Expected ']' end of Group(%s)!\n", line);
                }
                break;
            }
            case '#':
            {
                break;
            }
            default:
            {
                p = strstr(line,"=");
                if(p)
                {
                    *p++ = 0;
                    item = line;
                    val = p;
                    if((item[0]==0)||(val[0]==0))
                    {
                        ALOGI("voice_conf_parse_config ignore(item:%s val:%s)\n", item[0]?item:"(null)", val[0]?val:"(null)");
                        break;
                    }
                    ALOGI("voice_conf_parse_config Find Item[%s:%s=%s]\n", group, item, val);
                    entry = malloc(sizeof(tVoice_Hidraw_Settings));
                    if(entry)
                    {
                        entry->next = NULL;
                        entry->group = group;
                        entry->item = item;
                        entry->val = val;
                        if(end)
                        {
                            end->next = entry;
                            end = entry;
                        }else
                            end = entry;
                        if(head == NULL)
                            head = entry;
                    }
                }
            }
        }
    }
    *items = head;
}


int voice_loadconfig(char * path)
{
    int i,res,fd;
    struct stat st;
    char * content = NULL;
    struct tVoice_Hidraw_Settings* config = NULL;

    ALOGI("voice_loadconfig(%s) load!\n", path);
    fd = open(path, O_RDONLY);
    if(fd < 0)
    {
        ALOGW("voice_loadconfig(%s) open fail!\n", path);
        return -1;
    }
    res = fstat(fd, &st);
    if(res)
    {
        ALOGW("voice_loadconfig(%s) stat fail! reason=%d\n", path, res);
        return -1;
    }
    if(st.st_size <= 0)
    {
        ALOGW("voice_loadconfig(%s) size fail! size=%d\n", path, (int)st.st_size);
        close(fd);
        return -1;
    }
    ALOGI("voice_loadconfig(%s) size=%d!\n", path, (int)st.st_size);
    content = malloc(st.st_size+1);
    if(content == NULL)
    {
        ALOGW("voice_loadconfig(%s) malloc fail!\n", path);
        close(fd);
        return -1;
    }
    res = read(fd, content, st.st_size);
    if(res <= 0)
    {
        ALOGW("voice_loadconfig(%s) read fail! res=%d\n", path, res);
        close(fd);
        return -1;
    }
    content[res] = 0;
    voice_conf_parse_config(&config, content);
    close(fd);

    gConfig = config;
    return 0;
}




static char * voice_api_GetConfig(char * group, char * item)
{
    int i;
    tVoice_Hidraw_Settings * entry;

    if(item == NULL)
        return NULL;
    entry = gConfig;
    while(entry)
    {
        if(group&&(entry->group)&&(strcmp(entry->group, group)==0)&&(strcmp(entry->item, item)==0))
            return entry->val;
        if((group==NULL)&&(entry->group==NULL)&&(strcmp(entry->item, item)==0))
            return entry->val;

        entry = entry->next;
    }
    return NULL;
}

int voice_get_int_config(char *group, char *item) {
    char *str;
    int value = -1;

    str = voice_api_GetConfig(group, item);
    if(str == NULL) {
        ALOGE("failed to get int value: %s-%s", group, item);
        goto result;
    }

    value = strtol(str, NULL, 16);

result:
    return value;
}

int voice_get_byte_array_config(char *group, char *item, char* arr, int len) {
    char *str, *front, *tail;
    int ret = -1;
    int i = 0;

    str = voice_api_GetConfig(group, item);
    if(str == NULL) {
        goto retval;
    }

    front = str;

    while(i < len) {
        tail = strstr(front, ",");
        if(tail == NULL) {
            tail = strstr(front, ";");
        }

        if(tail == NULL) {
            ALOGE("failed to get byte-array value: %s-%s-No%d", group, item, i + 1);
            goto retval;
        }

        *tail++ = 0;
        *(arr+i) = (strtol(front, NULL, 16)) & 0xff;
        front = tail;
        i++;
    }

    if(i == len)ret = i;

retval:
    return ret;
}

int voice_conf_exit()
{
    struct tVoice_Hidraw_Settings *item, *p;
    int i;

    item = gConfig;
    while(item)
    {
        p = item;
        item = item->next;
        free(p);
    }

    if(gContent)
        free(gContent);
    return 0;
}


