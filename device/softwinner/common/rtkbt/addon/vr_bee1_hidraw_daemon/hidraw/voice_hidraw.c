#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/inotify.h>
#include <sys/poll.h>
#include <linux/input.h>
#include <linux/hidraw.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>

#include <pthread.h>


#include "voice_hidraw.h"
#include "bee_settings.h"
#include "get_voice_app.h"
#include "sbc.h"
#include "voice_uipc.h"
#include "config.h"

#define RAW_DATA_PATH "/data/vr/raw_data"
static int file_counter = 0;
int raw_fd;
int gSocket_Running = 0;
int gTarget_fd = 0;



typedef struct {
    int vid;
    int pid;
    int vers;
    unsigned char voice_key_reportID;
    unsigned char voice_data_reportID;
    unsigned char voice_cmd_reportID;
    int voice_keypress_data_len;
    unsigned char *voice_keypress_data;
    int voice_keyrelease_data_len;
    unsigned char *voice_keyrelease_data;
    int decode_type;
    int data_mask;
    int autocmd;
    
}VOICE_SETTING;

static struct pollfd *ufds;
static char **device_names;
static int nfds;

static int *unused_bt_hid_fds;
static int nunusedfds;
static char **unused_device_names;


VOICE_SETTING global_voice_setting;
pthread_mutex_t mutex_voice_settings;

int frame_counter = 0;

int set_rcu_info(int vid, int pid, int vers) {
    pthread_mutex_lock(&mutex_voice_settings);
    if(vid != -1)
        global_voice_setting.vid = vid;

    if(pid != -1)
        global_voice_setting.pid = pid;

    if(vers != -1)
        global_voice_setting.vers = vers;

    pthread_mutex_unlock(&mutex_voice_settings);

    return 0;
}

int set_voice_data_reportID(unsigned char reportID) {
    pthread_mutex_lock(&mutex_voice_settings);
    global_voice_setting.voice_data_reportID = reportID;
    pthread_mutex_unlock(&mutex_voice_settings);
    return 0;
}

int set_voice_cmd_reportID(unsigned char reportID) {
    pthread_mutex_lock(&mutex_voice_settings);
    global_voice_setting.voice_cmd_reportID = reportID;
    pthread_mutex_unlock(&mutex_voice_settings);
    return 0;
}

int set_voice_key_reportID(unsigned char reportID) {
    pthread_mutex_lock(&mutex_voice_settings);
    global_voice_setting.voice_key_reportID = reportID;
    pthread_mutex_unlock(&mutex_voice_settings);
    return 0;
}

int set_voice_keypress_data(int len, unsigned char *data) {
    if(len > 0) {
        pthread_mutex_lock(&mutex_voice_settings);
        if(global_voice_setting.voice_keypress_data != NULL) {
            if(global_voice_setting.voice_keypress_data_len < len) {

                unsigned char *buf = (unsigned char *)realloc(global_voice_setting.voice_keypress_data, len);
                if(buf == NULL) {
                    //log
                    pthread_mutex_unlock(&mutex_voice_settings);
                    error("Failed to realloc for voice_keypress_data");
                    return -1;
                }else {
                    global_voice_setting.voice_keypress_data = buf;
                }
            }
        }else {
            global_voice_setting.voice_keypress_data = (unsigned char *)malloc(len);
            if(global_voice_setting.voice_keypress_data == NULL) {
                //log
                pthread_mutex_unlock(&mutex_voice_settings);
                error("Failed to malloc for voice_keypress_data");
                return -2;
            }
        }

        memcpy(global_voice_setting.voice_keypress_data, data, len);
        global_voice_setting.voice_keypress_data_len = len;
        pthread_mutex_unlock(&mutex_voice_settings);
        return 0;
    }else {
        warn("len is negative for voice_keypress_data");
        return -3;
    }
}

int set_voice_keyrelease_data(int len, unsigned char *data) {
    if(len > 0) {
        pthread_mutex_lock(&mutex_voice_settings);
        if(global_voice_setting.voice_keyrelease_data != NULL) {
            if(global_voice_setting.voice_keyrelease_data_len < len) {
                unsigned char *buf = (unsigned char *)realloc(global_voice_setting.voice_keyrelease_data, len);
                if(buf == NULL) {
                    //log
                    pthread_mutex_unlock(&mutex_voice_settings);
                    error("Failed to realloc for voice_keypress_data");
                    return -1;
                }else {
                    global_voice_setting.voice_keyrelease_data = buf;
                }
            }
        }else {
            global_voice_setting.voice_keyrelease_data = (unsigned char *)malloc(len);
            if(global_voice_setting.voice_keyrelease_data == NULL) {
                pthread_mutex_unlock(&mutex_voice_settings);
                error("Failed to malloc for voice_keyrelease_data");
                return -2;
            }
        }

        memcpy(global_voice_setting.voice_keyrelease_data, data, len);
        global_voice_setting.voice_keyrelease_data_len = len;
        pthread_mutex_unlock(&mutex_voice_settings);
        return 0;
    }else {
        //log
        warn("len is negative for voice_keyrelease_data");
        return -3;
    }
}

int set_encode_type(int type) {
    if(type <= DECODE_TYPE_MAX) {
        pthread_mutex_lock(&mutex_voice_settings);
        global_voice_setting.decode_type = type;
        pthread_mutex_unlock(&mutex_voice_settings);
        return 0;
    }else {
        warn("Unknown Decode Type !!!");
        return -1;
    }
}

int data_type_mask(int mask) {
    //log
    debug("Set Data Type Mask 0x%04x", mask);

    pthread_mutex_lock(&mutex_voice_settings);
    
    if(mask & DATA_TYPE_MASK_RAW) {
        global_voice_setting.data_mask = DATA_TYPE_MASK_RAW;
    }else {
        if(mask & DATA_TYPE_MASK_VOICE) {
            global_voice_setting.data_mask |= DATA_TYPE_MASK_VOICE;
        }
    }

    pthread_mutex_unlock(&mutex_voice_settings);
    return 0;
}

int set_auto_cmd(int autocmd) {
    pthread_mutex_lock(&mutex_voice_settings);
    if(autocmd == 1) {
        global_voice_setting.autocmd = AUTOCMD_AUTO;
    }else if(autocmd == 2){
        global_voice_setting.autocmd = AUTOCMD_NONE;
    }
    pthread_mutex_unlock(&mutex_voice_settings);

    return 0;
}



int voice_msbc_init() {

    int ivalue, i;
   
    voice_loadconfig(CONFIG_PATH);

    pthread_mutex_lock(&mutex_voice_settings);
    
    memset(&global_voice_setting, 0, sizeof(global_voice_setting));

    ivalue = voice_get_int_config(CONFIG_GROUP_RCU, CONFIG_ITEM_RCU_VID);
    if(ivalue >= 0) {
        global_voice_setting.vid = ivalue;
        ALOGD("use config VID: %04x", global_voice_setting.vid);
    } else {
        global_voice_setting.vid = BEE_RCU_VID;
        ALOGD("use default VID: %04x", global_voice_setting.vid);
    }

    ivalue = voice_get_int_config(CONFIG_GROUP_RCU, CONFIG_ITEM_RCU_PID);
    if(ivalue >= 0) {
        global_voice_setting.pid = ivalue;
        ALOGD("use config PID: %04x", global_voice_setting.pid);
    } else {
        global_voice_setting.pid = BEE_RCU_PID;
        ALOGD("use default PID: %04x", global_voice_setting.pid);
    }

    ivalue = voice_get_int_config(CONFIG_GROUP_RCU, CONFIG_ITEM_RCU_VER);
    if(ivalue >= 0) {
        global_voice_setting.vers = ivalue;
        ALOGD("use config version: %04x", global_voice_setting.vers);
    } else {
        global_voice_setting.vers = BEE_RCU_VERSION;
        ALOGD("use default VERSION: %04x", global_voice_setting.vers);
    }

    ivalue = voice_get_int_config(CONFIG_GROUP_REPORTID, CONFIG_ITEM_REPORTID_KEY);
    if(ivalue > 0) {
        global_voice_setting.voice_key_reportID = ivalue & 0xff;
        ALOGD("use config key report id: %02x", global_voice_setting.voice_key_reportID);
    } else {
        global_voice_setting.voice_key_reportID = BEE_VOICEKEY_REPORTID;
        ALOGD("use default key report id: %02x", global_voice_setting.voice_key_reportID);
    }
    

    ivalue = voice_get_int_config(CONFIG_GROUP_REPORTID, CONFIG_ITEM_REPORTID_DATA);
    if(ivalue > 0) {
        global_voice_setting.voice_data_reportID = ivalue & 0xff;
        ALOGD("use config data report id: %02x", global_voice_setting.voice_data_reportID);
    } else {
        global_voice_setting.voice_data_reportID = BEE_MSBC_VOICEDATA_REPORTID;
        ALOGD("use default data report id: %02x", global_voice_setting.voice_data_reportID);
    }

    ivalue = voice_get_int_config(CONFIG_GROUP_REPORTID, CONFIG_ITEM_REPORTID_CMD);
    if(ivalue > 0) {
        global_voice_setting.voice_cmd_reportID = ivalue & 0xff;
        ALOGD("use config cmd report id: %02x", global_voice_setting.voice_cmd_reportID);
    } else {
        global_voice_setting.voice_cmd_reportID = BEE_MSBC_VOICECMD_REPORTID;
        ALOGD("use default cmd report id: %02x", global_voice_setting.voice_cmd_reportID);
    }

    ivalue = voice_get_int_config(CONFIG_GROUP_REPORTVALUE, CONFIG_ITEM_REPORTVALUE_KEYDOWNLEN);
    if(ivalue > 0) {
        global_voice_setting.voice_keypress_data_len = ivalue;
        ALOGD("use config keydown len: %d", global_voice_setting.voice_keypress_data_len);
    } else {
        global_voice_setting.voice_keypress_data_len = BEE_VOICEKEY_LEN;
        ALOGD("use default keydown len: %d", global_voice_setting.voice_keypress_data_len);
    }

    global_voice_setting.voice_keypress_data = (unsigned char *)malloc(global_voice_setting.voice_keypress_data_len);
    if(global_voice_setting.voice_keypress_data == NULL) {
        //log
        error("failed to get buf for keydown");
        pthread_mutex_unlock(&mutex_voice_settings);
        return -1;
    }else {
        if(ivalue > 0) {
            if(voice_get_byte_array_config(CONFIG_GROUP_REPORTVALUE, CONFIG_ITEM_REPORTVALUE_KEYDOWN,
                global_voice_setting.voice_keypress_data, global_voice_setting.voice_keypress_data_len)
                != global_voice_setting.voice_keypress_data_len) {
                
                global_voice_setting.voice_keypress_data_len = BEE_VOICEKEY_LEN;
                if(ivalue < BEE_VOICEKEY_LEN) {
                    global_voice_setting.voice_keypress_data = (unsigned char *)realloc(global_voice_setting.voice_keypress_data, BEE_VOICEKEY_LEN);
                    if(global_voice_setting.voice_keypress_data == NULL) {
                        error("failed to realloc buf for keydown");
                        pthread_mutex_unlock(&mutex_voice_settings);
                        return -1;
                    }
                }
                memcpy(global_voice_setting.voice_keypress_data, BEE_VOICE_KEYDOWN_DATA, global_voice_setting.voice_keypress_data_len);
                ALOGD("use default keydown data");
            }
        }else {
            memcpy(global_voice_setting.voice_keypress_data, BEE_VOICE_KEYDOWN_DATA, global_voice_setting.voice_keypress_data_len);
            ALOGD("use default keydown data");
        }
        ALOGD("key down data:");
        for(i = 0; i < global_voice_setting.voice_keypress_data_len; i++) {
            ALOGD("data[%d] = %02x", i, *(global_voice_setting.voice_keypress_data + i));
        }
    }

    ivalue = voice_get_int_config(CONFIG_GROUP_REPORTVALUE, CONFIG_ITEM_REPORTVALUE_KEYUPLEN);
    if(ivalue > 0) {
        global_voice_setting.voice_keyrelease_data_len = ivalue;
        ALOGD("use config keyup len: %d", global_voice_setting.voice_keyrelease_data_len);
    } else {
        global_voice_setting.voice_keyrelease_data_len = BEE_VOICEKEY_LEN;
        ALOGD("use default keyup len: %d", global_voice_setting.voice_keyrelease_data_len);
    }

    global_voice_setting.voice_keyrelease_data = (unsigned char *)malloc(global_voice_setting.voice_keyrelease_data_len);
    if(global_voice_setting.voice_keyrelease_data == NULL) {
        //log
        error("failed to get buf for keyup");
        pthread_mutex_unlock(&mutex_voice_settings);
        return -1;
    }else {
        if(ivalue > 0) {
            if(voice_get_byte_array_config(CONFIG_GROUP_REPORTVALUE, CONFIG_ITEM_REPORTVALUE_KEYUP,
                global_voice_setting.voice_keyrelease_data, global_voice_setting.voice_keyrelease_data_len)
                != global_voice_setting.voice_keyrelease_data_len) {
                
                global_voice_setting.voice_keyrelease_data_len = BEE_VOICEKEY_LEN;
                if(ivalue < BEE_VOICEKEY_LEN) {
                    global_voice_setting.voice_keyrelease_data = (unsigned char *)realloc(global_voice_setting.voice_keyrelease_data, BEE_VOICEKEY_LEN);
                    if(global_voice_setting.voice_keyrelease_data == NULL) {
                        error("failed to realloc buf for keyup");
                        pthread_mutex_unlock(&mutex_voice_settings);
                        return -1;
                    }
                }
                memcpy(global_voice_setting.voice_keyrelease_data, BEE_VOICE_KEYUP_DATA, global_voice_setting.voice_keyrelease_data_len);
                ALOGD("use default keyup data");
            }
        }else {
            memcpy(global_voice_setting.voice_keyrelease_data, BEE_VOICE_KEYUP_DATA, global_voice_setting.voice_keyrelease_data_len);
            ALOGD("use default keyup data");
        }
        ALOGD("key up data:");
        for(i = 0; i < global_voice_setting.voice_keyrelease_data_len; i++) {
            ALOGD("data[%d] = %02x", i, *(global_voice_setting.voice_keyrelease_data + i));
        }
    }

    ivalue = voice_get_int_config(CONFIG_GROUP_SETTINGS, CONFIG_ITEM_SETTINGS_DECODETYPE);
    if(ivalue > 0) {
        global_voice_setting.decode_type = ivalue;
        ALOGD("use config decode type: %d", global_voice_setting.decode_type);
    } else {
        global_voice_setting.decode_type = DECODE_TYPE_MSBC;
        ALOGD("use default decode type: %d", global_voice_setting.decode_type);
    }

    ivalue = voice_get_int_config(CONFIG_GROUP_SETTINGS, CONFIG_ITEM_SETTINGS_DATAMASK);
    if(ivalue > 0) {
        global_voice_setting.data_mask = ivalue;
        ALOGD("use config data mask: %d", global_voice_setting.data_mask);
    } else {
        global_voice_setting.data_mask = DATA_TYPE_MASK_VOICE;
        ALOGD("use default decode type: %d", global_voice_setting.data_mask);
    }

    ivalue = voice_get_int_config(CONFIG_GROUP_SETTINGS, CONFIG_ITEM_SETTINGS_AUTOCMD);
    if(ivalue > 0) {
        global_voice_setting.autocmd = ivalue;
        ALOGD("use config decode type: %d", global_voice_setting.autocmd);
    } else {
        global_voice_setting.autocmd = AUTOCMD_AUTO;
        ALOGD("use default decode type: %d", global_voice_setting.autocmd);
    }

    pthread_mutex_unlock(&mutex_voice_settings);
    voice_conf_exit();
    return 0;
}

void voice_open_store_file()
{
    char record_name[50] = {0}; 
    sprintf(record_name, "%s%d", RAW_DATA_PATH, file_counter);

    raw_fd =  open (record_name, O_WRONLY | O_CREAT |O_TRUNC, 0644);

    if(raw_fd < 0)
    {
        ALOGE("can't open record file:%s ", record_name);
    }


	return;
}

int voice_write_decode_buf(uint8* pu_decode, ssize_t len)
{
    if (raw_fd > 0)
    {
        write (raw_fd, pu_decode, len);
        return 0;
    }
    return -1;
}


/**********************************************
*
*   hidraw node related
*
***********************************************/
const char *bus_str(int bus){
    switch (bus) {
        case BUS_USB:
            return "USB";
            
        case BUS_HIL:
            return "HIL";
            
        case BUS_BLUETOOTH:
            return "Bluetooth";

        case BUS_VIRTUAL:
            return "Virtual";
                
        default:
            return "Other";
    }
}
    
static int get_hidraw_info(int fd)
{
    int i, res, desc_size = 0;
    char buf[256];
    struct hidraw_report_descriptor rpt_desc;
    struct hidraw_devinfo info;
    
    memset(&rpt_desc, 0x0, sizeof(rpt_desc));
    memset(&info, 0x0, sizeof(info));
    memset(buf, 0x0, sizeof(buf));
    debug("-----------------------------------------------\n");
    
    /* Get Raw Name */
    res = ioctl(fd, HIDIOCGRAWNAME(256), buf);
    if (res < 0)
        perror("HIDIOCGRAWNAME");
    else
        debug("Raw Name: %s\n", buf);
    /* Get Physical Location */
    res = ioctl(fd, HIDIOCGRAWPHYS(256), buf);
    if (res < 0)
        perror("HIDIOCGRAWPHYS");
    else
        debug("Raw Phys: %s\n", buf);
    /* Get Raw Info */
    res = ioctl(fd, HIDIOCGRAWINFO, &info);
    if (res < 0) {
        perror("HIDIOCGRAWINFO");
    } else {
        debug("Raw Info:");
        debug(" bustype: %d (%s)", info.bustype, bus_str(info.bustype));
        debug(" vendor: 0x%04x", info.vendor);
        debug(" product: 0x%04x\n", info.product);
    }
    /* Get Report Descriptor Size */
    res = ioctl(fd, HIDIOCGRDESCSIZE, &desc_size);
    if (res < 0)
       perror("HIDIOCGRDESCSIZE");
    else
        debug("Report Descriptor Size: %d\n", desc_size);
    /* Get Report Descriptor */
    rpt_desc.size = desc_size;
    res = ioctl(fd, HIDIOCGRDESC, &rpt_desc);
    if (res < 0) {
        perror("HIDIOCGRDESC");
    } else {
        debug("Report Descriptor:");
        for (i = 0; i < rpt_desc.size; i++)
        {
            if((i%16)==0)
                debug("\n");
            debug("%02x ", rpt_desc.value[i]);
        }
        debug("\n");
    }
    info("-----------------------------------------------\n");
    
    return 0;
}

static int send_message(int fd, unsigned char * buf, int len)
{
    int res = 0;
    res = write(fd, buf, len);
    debug("send_message res=%d\n",res);
    return res;
}

static void process_voice_data(unsigned char reportID, unsigned char *buf, int len) {
    int ret = -1;
    int offset = 0;
    int i;
    int decode_len = 0;

    unsigned char output_buf[1024] = {0x00};

    //debug("reportID=%d, decode_type=%d, global_reportID=%d, len=%d", 
        //reportID, global_voice_setting.decode_type, global_voice_setting.voice_data_reportID, len);

    if(global_voice_setting.decode_type == DECODE_TYPE_MSBC) {
        while(offset + BEE_MSBC_FRAME_SIZE <= len) {
            //debug("offset is %d now, len is %d", offset, len);
            ret = RS_deal_voice_stream_data(buf + offset, BEE_MSBC_FRAME_SIZE, output_buf, 1024, &decode_len);
            if(ret < 0) {
                offset++;
            }else {
                UIPC_Send_noblock(UIPC_CH_ID_VOICE_DATA, 0, output_buf, decode_len);
                offset += BEE_MSBC_FRAME_SIZE;
            }
        }
    }else if(global_voice_setting.decode_type == DECODE_TYPE_SBC) {
        decode_len = 1024;
        while(offset + BEE_SBC_FRAME_SIZE <= len) {
            ret = sbcDecode(buf + offset, BEE_SBC_FRAME_SIZE, output_buf, &decode_len);
            if(ret < 0) {
                debug("fail to decode: %d", ret);
                offset++;
            }else {
                UIPC_Send_noblock(UIPC_CH_ID_VOICE_DATA, 0, output_buf, decode_len);
                offset += BEE_SBC_FRAME_SIZE;
            }
        }
    }
}

static int read_message(int fd)
{
    unsigned char buf[4096];
    int res,i;

    unsigned char start_rec_command[] = {global_voice_setting.voice_cmd_reportID, 0x01};
    unsigned char stop_rec_command[] = {global_voice_setting.voice_cmd_reportID, 0x00};
    static int voice_kpressed = 0;

/*
    struct hidraw_devinfo info;
 
    res = ioctl(fd, HIDIOCGRAWINFO, &info);
    if (res < 0) {
        perror("HIDIOCGRAWINFO");
    }
*/

    //if( (info.vendor==global_voice_setting.vid) && (info.product==global_voice_setting.pid) )
    //{
        res = read(fd, buf, sizeof(buf));
        if(res > 0)
        {
            //debug("Receive From :%04x,%04x number: %d", info.vendor, info.product, res);
            if( (global_voice_setting.data_mask & DATA_TYPE_MASK_RAW) == DATA_TYPE_MASK_RAW) {
                //just callback and return
                UIPC_Send_noblock(UIPC_CH_ID_VOICE_DATA, 0, buf + 1, res - 1);
                return 0;
            }else if( (global_voice_setting.data_mask & DATA_TYPE_MASK_VOICE) == DATA_TYPE_MASK_VOICE) {
                if((voice_kpressed == 0) && (buf[0] == global_voice_setting.voice_key_reportID) && 
                    (res == global_voice_setting.voice_keypress_data_len + 1) && 
                    (memcmp(buf + 1, global_voice_setting.voice_keypress_data, global_voice_setting.voice_keypress_data_len) == 0))
                {
                    //debug("voice key pressed");
                    if(global_voice_setting.autocmd == AUTOCMD_AUTO) {
                        RS_voice_app_create_output();
                        RS_voice_app_create_origin_output();
                        voice_open_store_file();

                        send_message(fd, start_rec_command,sizeof(start_rec_command));
                        voice_kpressed = 1;
                        frame_counter = 0;
                    }
                    return 0;
                }

                // when voice key is released
                if ((voice_kpressed == 1) && (buf[0] == global_voice_setting.voice_key_reportID) && 
                    (res == global_voice_setting.voice_keyrelease_data_len + 1) && 
                    (memcmp(buf + 1, global_voice_setting.voice_keyrelease_data, global_voice_setting.voice_keyrelease_data_len) == 0))
                {
                    //debug("voice key relesed");

                    if(global_voice_setting.autocmd == AUTOCMD_AUTO) {
                        RS_stop_voice_stream_data();
                        send_message(fd, stop_rec_command,sizeof(stop_rec_command));
                        voice_kpressed = 0;
                        debug("total frame: %d", frame_counter);
                        close(raw_fd);
                        file_counter++;
                        raw_fd = -1;
                    }
                    return 0;
                }

                // store voice data
                if ((voice_kpressed == 1 ) && (buf[0] == global_voice_setting.voice_data_reportID))
                {
                    //debug("deal with voice data");
                    process_voice_data(buf[0], buf + 1, res-1);
                    voice_write_decode_buf(buf + 1, res - 1);
                    frame_counter++;
                    return 0;
                }
            }
        }else {
            warn("Read fd %d failed: %s", fd, strerror(errno));
            return -1;
        }
        
    //}

    return 0;
}

int get_dev_vid_pid_info(int fd, short* vid, short* pid) {
    int res = -1;
    struct hidraw_devinfo info;

    memset(&info, 0x0, sizeof(info));

    res = ioctl(fd, HIDIOCGRAWINFO, &info);
    if (res < 0) {
        debug("failed to get HIDIOCGRAWINFO");
    } else {
        //debug("Raw Info:");
        //debug(" bustype: %d (%s)", info.bustype, bus_str(info.bustype));
        debug(" vendor: 0x%04x, product: 0x%04x", info.vendor, info.product);
        *vid = info.vendor;
        *pid = info.product;
        res = 1;
    }
    return res;
}

static int open_device(const char *device)
{
    int version;
    int fd;
    int clkid = CLOCK_MONOTONIC;
    struct pollfd *new_ufds;
    int *unfds;
    char **new_device_names;
    char **new_unused_device_names;
    char name[80];
    char location[80];
    char idstr[80];
    struct input_id id;
    int res = -1;
    struct hidraw_devinfo info;

    fd = open(device, O_RDWR);
    if(fd < 0) {
        debug("could not open %s, %s\n", device, strerror(errno));
        return -1;
    }
    debug("new fd is %d", fd);

    res = ioctl(fd, HIDIOCGRAWINFO, &info);
    if(res < 0) {
        debug("failed to get hidraw_devinfo, add for now");
    }else if(info.bustype != BUS_BLUETOOTH){
        debug("ingore this device");
        close(fd);
        return -1;
    }else if((info.vendor != global_voice_setting.vid) || (info.product != global_voice_setting.pid)) {
        //res = unlink(fd);
        debug("Not the device we care, add to unused fds");
        unfds = realloc(unused_bt_hid_fds, sizeof(unused_bt_hid_fds[0]) * (nunusedfds + 1));
        if(unfds == NULL) {
            error("unused fds: out of memory");
            return -1;
        }

        new_unused_device_names = realloc(unused_device_names, sizeof(unused_device_names[0]) * (nunusedfds + 1));
        if(new_unused_device_names == NULL) {
            error("unused device_names: out of memory");
            return -1;
        }

        unused_bt_hid_fds = unfds;
        unused_device_names = new_unused_device_names;
        
        unused_bt_hid_fds[nunusedfds] = fd;
        unused_device_names[nunusedfds] = strdup(device);
        nunusedfds++;
        return 0; 
    }

    new_ufds = realloc(ufds, sizeof(ufds[0]) * (nfds + 1));
    if(new_ufds == NULL) {
        error("out of memory\n");
        return -1;
    }

    ufds = new_ufds;
    new_device_names = realloc(device_names, sizeof(device_names[0]) * (nfds + 1));
    if(new_device_names == NULL) {
        debug("out of memory\n");
        return -1;
    }
    device_names = new_device_names;
    //get_hidraw_info(fd);
    ufds[nfds].fd = fd;
    ufds[nfds].events = POLLIN;
    device_names[nfds] = strdup(device);
    nfds++;

    return 0;
}

int close_device(const char *device)
{
    int i;
    for(i = 1; i < nfds; i++) {
        if(strcmp(device_names[i], device) == 0) {
            int count = nfds - i - 1;
            debug("remove device %d: %s\n", i, device);
            free(device_names[i]);
            memmove(device_names + i, device_names + i + 1, sizeof(device_names[0]) * count);
            close(ufds[i].fd);
            memmove(ufds + i, ufds + i + 1, sizeof(ufds[0]) * count);
            nfds--;
            return 0;
        }
    }

    for(i = 0; i < nunusedfds; i++) {
        if(strcmp(unused_device_names[i], device) == 0) {
            int count = nunusedfds - i;
            debug("remove unused device %d: %s\n", i, device);
            free(unused_device_names[i]);
            memmove(unused_device_names + i, unused_device_names + i + 1, sizeof(unused_device_names[0]) * count);
            close(unused_bt_hid_fds[i]);
            memmove(unused_bt_hid_fds + i, unused_bt_hid_fds + i + 1, sizeof(unused_bt_hid_fds[0]) * count);
            nunusedfds--;
            return 0;
        }
    }
    
    error("remote device: %s not found\n", device);
    return -1;
}

static int read_notify(const char *dirname, int nfd)
{
    int res;
    char devname[PATH_MAX];
    char *filename;
    char event_buf[512];
    int event_size;
    int event_pos = 0;
    struct inotify_event *event;

    res = read(nfd, event_buf, sizeof(event_buf));
    if(res < (int)sizeof(*event)) {
        if(errno == EINTR)
            return 0;
        error("could not get event, %s\n", strerror(errno));
        return 1;
    }
    //printf("got %d bytes of event information\n", res);

    strcpy(devname, dirname);
    filename = devname + strlen(devname);
    *filename++ = '/';

    while(res >= (int)sizeof(*event)) {
        event = (struct inotify_event *)(event_buf + event_pos);
        //debug("%d: %08x \"%s\"\n", event->wd, event->mask, event->len ? event->name : "");
        if((event->len)&&(memcmp(event->name, "hidraw", strlen("hidraw"))==0)) {
            debug("read_notify: %d: %08x \"%s\"\n", event->wd, event->mask, event->len ? event->name : "");
            strcpy(filename, event->name);
            if(event->mask & IN_CREATE) {
                open_device(devname);
            }
            else {
                close_device(devname);
            }
        }
        event_size = sizeof(*event) + event->len;
        res -= event_size;
        event_pos += event_size;
    }
    return 0;
}


static int scan_dir(const char *dirname)
{
    char devname[PATH_MAX];
    char *filename;
    DIR *dir;
    struct dirent *de;
    dir = opendir(dirname);
    if(dir == NULL)
        return -1;
    strcpy(devname, dirname);
    filename = devname + strlen(devname);
    *filename++ = '/';
    while((de = readdir(dir))) {
        if(de->d_name[0] == '.' &&
           (de->d_name[1] == '\0' ||
            (de->d_name[1] == '.' && de->d_name[2] == '\0')))
            continue;
        if(memcmp(de->d_name,"hidraw",strlen("hidraw")))
            continue;
        strcpy(filename, de->d_name);
        //debug("scan_dir: %s\n",devname);
        open_device(devname);
    }
    closedir(dir);
    return 0;
}

static void voice_ctrl_recv(void)
{
    unsigned char cmd[256], opcode = 0xFF;
    int n, i;

    n = UIPC_Read(UIPC_CH_ID_VOICE_CTRL, NULL, cmd, 256);

    /* detach on ctrl channel means audioflinger process was terminated */
    if (n == 0)
    {
        debug("CTRL CH DETACHED");
        //the ctrl channel will re-open automatically
        UIPC_Close(UIPC_CH_ID_VOICE_CTRL);
        return;
    }

    debug("receive voice-ctrl-cmd read num_of_data = %d ", n);
    for (i=0; i< n; i++)
        info("data[%d] = %d ", i, cmd[i]);
    //receive start or stop cmd from service
    if( cmd[0] <= OPCODE_MAX)
    {
        opcode = cmd[0];
    }
    else
    {
        //clear the memory
        UIPC_Ioctl(UIPC_CH_ID_VOICE_CTRL, UIPC_REQ_RX_FLUSH,NULL);
        info("it is wrong cmd send from service");
    }

    info("opcode = %d",opcode);
    switch(opcode)
    {
        case OPCODE_SETTING_RESET:              //0   cmd(1)
            break;

        case OPCODE_VOICE_START_CMD:            //1   cmd(1)
            debug("start, len=%d", n);
            break;

        case OPCODE_VOICE_STOP_CMD:             //2   cmd(1)
            debug("stop, len=%d", n);
            break;

        case OPCODE_SET_RCU_INFO:               //3   cmd(1) + vid(2) + pid(2) + vers(2)
            if(n == 7) {
                int vid = (cmd[1] << 8) + cmd[2];
                int pid = (cmd[3] << 8) + cmd[4];
                int vers = (cmd[5] << 8) + cmd[6];
                set_rcu_info(vid,pid,vers);
            }
            break;

        case OPCODE_SET_VOICE_DATA_REPORT_ID:   //4   cmd(1) + reportID(1)
            if(n == 2) {
                set_voice_data_reportID(cmd[1]);
            }
            break;

        case OPCODE_SET_VOICE_CMD_REPORT_ID:    //5   cmd(1) + reportID(1)
            if(n == 2) {
                set_voice_cmd_reportID(cmd[1]);
            }
            break;

        case OPCODE_SET_VOICE_KEY_REPORT_ID:    //6   cmd(1) + reportID(1)
            if(n == 2) {
                set_voice_key_reportID(cmd[1]);
            }
            break;

        case OPCODE_SET_VOICE_KEYPRESS_DATA:    //7   cmd(1) + len(1) + data(n)
            if(n > 2) {
                int len = cmd[1];
                set_voice_keypress_data(len, cmd + 2);
            }
            break;

        case OPCODE_SET_VOICE_KEYRELEASE_DATA:  //8   cmd(1) + len(1) + data(n)
            if(n > 2) {
                int len = cmd[1];
                set_voice_keyrelease_data(len, cmd + 2);
            }
            break;

        case OPCODE_SET_VOICE_STARTCMD_DATA:    //9   cmd(1) + len(1) + data(n)
            debug("set voice start data, len=%d", cmd[1]);
            break;

        case OPCODE_SET_VOICE_STOPCMD_DATA:     //10  cmd(1) + len(1) + data(n)
            debug("set voice stop data, len=%d", cmd[1]);
            break;

        case OPCODE_SET_ENCODE_TYPE:            //11  cmd(1) + type(1)
            if(n == 2) {
                set_encode_type(cmd[1]);
            }
            break;

        case OPCODE_SET_DATA_TYPE_MASK:         //12  cmd(1) + type(1)
            debug("set type mask: 0x%02x", cmd[1]);
            break;

        case OPCODE_SET_AUTO_CMD:               //13  cmd(1) + auto(1)
            if(n == 2) {
                set_auto_cmd(cmd[1]);
            }
            break;

        case OPCODESEND_HIDRAW_DATA:            //14  cmd(1) + reportID(1) + len(1) + data(n)
            debug("send raw data, reportID=0x%02x, len=0x%02x", cmd[1], cmd[2]);
            break;

        case OPCODE_EXIT:
            debug("exit");
            gSocket_Running = 0;
    }

    info("voice-ctrl-cmd DOWN!");
}

static void voice_data_recv(void)
{
    unsigned char cmd[256], opcode = 0xFF;
    int n, i;

    n = UIPC_Read(UIPC_CH_ID_VOICE_DATA, NULL, cmd, 256);

    /* detach on ctrl channel means audioflinger process was terminated */
    if (n == 0)
    {
        debug("DATA CH DETACHED");
        //the ctrl channel will re-open automatically
        UIPC_Close(UIPC_CH_ID_VOICE_DATA);
        return;
    }

    debug("receive voice-data read num_of_data = %d ", n);
}


static void ctrl_socket_cb(tUIPC_CH_ID ch_id, tUIPC_EVENT event)
{

    switch(event)
    {
        case UIPC_OPEN_EVT:
            /*
                in certain case, hid status msg must be sent upon ctrl chan opened
             */
            debug("Jim@control_socket_cb: sending hid status msg upon open");
            break;

        case UIPC_CLOSE_EVT:
            if (gSocket_Running) {
                debug("jim@control_socket_cb: re-open ctrl chan");
                UIPC_Open(UIPC_CH_ID_VOICE_CTRL, ctrl_socket_cb);
            }
            break;

        case UIPC_RX_DATA_EVT:
            info("btif_voice_ctrl_cb, the event is UIPC_RX_DATA_EVT");

        case UIPC_RX_DATA_READY_EVT:
            voice_ctrl_recv();
            break;

        default :
            info("### VOICE-CTRL-CHANNEL EVENT %d NOT HANDLED ###", event);
            break;
    }
}

static void data_socket_cb(tUIPC_CH_ID ch_id, tUIPC_EVENT event)
{

    debug("data_socket_cb");
    switch(event)
    {
        case UIPC_OPEN_EVT:
            debug("Jim@data_socket_cb: sending hid status msg upon open");
            break;

        case UIPC_CLOSE_EVT:
            if (gSocket_Running) {
                debug("jim@data_socket_cb: re-open data chan");
                UIPC_Open(UIPC_CH_ID_VOICE_DATA, data_socket_cb);
            }
            break;

        case UIPC_RX_DATA_EVT:
            info("data_socket_cb, the event is UIPC_RX_DATA_EVT");

        case UIPC_RX_DATA_READY_EVT:
            voice_data_recv();
            info("data_socket_cb, the event is UIPC_RX_DATA_READY_EVT");
            break;

        default :
            info("### VOICE-DATA-CHANNEL EVENT %d NOT HANDLED ###", event);
            break;
    }
}



int init(void) {

    pthread_mutex_init(&mutex_voice_settings, NULL);
    voice_msbc_init();
    sbcInitDecoder();

    gSocket_Running = 1;
    UIPC_Init(NULL);
    UIPC_Open(UIPC_CH_ID_VOICE_CTRL, ctrl_socket_cb);
    UIPC_Open(UIPC_CH_ID_VOICE_DATA, data_socket_cb);

    return 0;
}

void cleanup(void) {

    gSocket_Running = 0;
    UIPC_Close(UIPC_CH_ID_VOICE_DATA);
    UIPC_Close(UIPC_CH_ID_VOICE_CTRL);

    pthread_mutex_lock(&mutex_voice_settings);
    
    
    if(global_voice_setting.voice_keypress_data != NULL)
        free(global_voice_setting.voice_keypress_data);

    if(global_voice_setting.voice_keyrelease_data != NULL)
        free(global_voice_setting.voice_keyrelease_data);

    pthread_mutex_unlock(&mutex_voice_settings);
    //release other resources here
}


int main(void) {
    const char *device_path = "/dev";
    int res;
    int i;

    init();

    nfds = 1;
    ufds = calloc(1, sizeof(ufds[0]));
    ufds[0].fd = inotify_init();
    ufds[0].events = POLLIN;

    res = inotify_add_watch(ufds[0].fd, device_path, IN_DELETE | IN_CREATE);
    if(res < 0) {
        error("could not add watch for %s, %s\n", device_path, strerror(errno));
        return -1;
    }

    res = scan_dir(device_path);
    if(res < 0) {
        error("scan dir failed for %s\n", device_path);
        return -2;
    }

    while(gSocket_Running) {
        poll(ufds, nfds, -1);
        if(ufds[0].revents & POLLIN) {
            //debug("new notify event!!!");
            read_notify(device_path, ufds[0].fd);
        }

        for(i = 1; i < nfds; i++) {
            if(ufds[i].revents) {
                /*
                //debug("new message event, fd = %d, event = %04x.", ufds[i].fd, ufds[i].revents);
                if((ufds[i].revents & POLLHUP) && (ufds[i].revents & POLLERR)) {
                    debug("close, fd=%d, events=0x%04x", ufds[i].fd, ufds[i].revents);
                    close(ufds[i].fd);
                 
                }else if(ufds[i].revents & POLLIN) {
                    //debug("new message event!!!");
                    read_message(ufds[i].fd);
                }
                */
                if(ufds[i].revents & POLLIN) {
                    //debug("new message event!!!");
                    read_message(ufds[i].fd);
                }
            }
        }
    }
    cleanup();
    
    return 0;

}

