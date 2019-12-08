#ifndef MODULE_INFO_H
#define MODULE_INFO_H

typedef struct {
    int vendor_id;
    char mod_name[20];
}module_info_t;

extern module_info_t module_info;

extern void aw_get_wifi_module_info(void);

#endif
