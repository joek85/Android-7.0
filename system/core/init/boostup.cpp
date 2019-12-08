#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>
#include <stdarg.h>
#include <dirent.h>
#include <limits.h>
#include <errno.h>
#include <pthread.h>
#include <cutils/misc.h>
#include <cutils/sockets.h>
#include <cutils/multiuser.h>
#include <sys/inotify.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/resource.h>
#include <netinet/in.h>
#include <sys/mman.h>
#include <private/android_filesystem_config.h>

#include <selinux/selinux.h>
#include <selinux/label.h>

#include "log.h"
#include "property_service.h"

static int detect_grap   = 3; //for 3s detect rescan

struct boost_list {
	int slot_active;
	int hold_pid;
	int mode_cur;
	int mode_pre;
};

struct boostup_mode {
	int mode_cur;
	int mode_pre;
};
static struct boost_list blist;
static struct boostup_mode bmode;

#define MEDIA_POLICY 		"media.boost.pref"
#define BENCH_POLICY 		"sys.boost.user"
#define SYSTEM_POLICY 		"sys.boost.pref"
#define SYSTEM_INIT_SIGNAL	"sys.boot_completed"
#define BOOST_DEBUG			"sys.boost.debug.enable_switch"
#define BENCHMARK_POLICY	"benchmark.boost.pref"
#define BOOST_MAGIC			"mode"
#define BOOST_CMD		strlen(BOOST_MAGIC)
#define BOOST_ENABLE		1
#define BOOST_MODE_RANGE	10

/* user mode */
#define AW_BOOST_UP_MODE_NORMAL	0xA5
#define AW_BOOST_UP_MODE_MEDIA  0xB1
#define AW_BOOST_UP_MODE_SYSTEM 0xC1
#define AW_BOOST_UP_MODE_BENCHMARK 0xD1

#define IS_ERR_RANGE(range)	(((range>BOOST_MODE_RANGE)||(range<BOOST_INIT_RANGE))?(-1):(0))
#define IS_INVAILD_PARAMS(prams) 	((prams==NULL)?(-1):(0))
#define IS_MATCH_ITEM(item, value)	((strncmp(value, item, strlen(item))==0)?(0):(-1))
#define RETFAULT 		(-1)
#define RETFINE  		(0)
#define BOOST_THREAD_STOP (0xAE)

static bool BOOST_UP_DEBUG = true;
static bool BOOST_UP_INIT  = false;
static int BOOST_UP_LOG_LEAVE = 0;
#define B_LOG(fmt,...) if (BOOST_UP_DEBUG && BOOST_UP_LOG_LEAVE>=1) ERROR(fmt,##__VA_ARGS__)
#define B_INF(fmt,...) if (BOOST_UP_DEBUG && BOOST_UP_LOG_LEAVE>=2) ERROR(fmt,##__VA_ARGS__)
#define B_FUNC_TRACE_START() B_LOG("+%s\n", __func__)
#define B_FUNC_TRACE_END(fmt,...) B_LOG("-%s " fmt,__func__, ##__VA_ARGS__)

/* CPU IDLE CONFIG*/
#define CPU_IDLE_STATE0_PATH		"/sys/devices/system/cpu/cpu0/cpuidle/state0/disable"
#define CPU_IDLE_STATE1_PATH		"/sys/devices/system/cpu/cpu0/cpuidle/state1/disable"
#define CPU_IDLE_STATE_ENABLE		"0"
#define CPU_IDLE_STATE_DISABLE		"1"
#define CPU_IDLE_STATE_MAGIC		(0x5A << 8)
#define CPU_IDLE_MAGIC_DATA(x)		(CPU_IDLE_STATE_MAGIC | (x))
#define CPU_IDLE_STATE0_ENABLE		CPU_IDLE_MAGIC_DATA(0)
#define CPU_IDLE_STATE0_DISABLE		CPU_IDLE_MAGIC_DATA(1)
#define CPU_IDLE_STATE1_ENABLE		CPU_IDLE_MAGIC_DATA(2)
#define CPU_IDLE_STATE1_DISABLE		CPU_IDLE_MAGIC_DATA(3)
#define CPU_DMA_LATENCY_PATH		"/dev/cpu_dma_latency"

/* AnTuTu CONFIG*/
#define ANTUTU_POLICY_ENABLE		(1)
#define ANTUTU_POLICY_DISABLE		(0)

/* cpu governors  */
/* interactive conservative ondemand userspace powersave performance */
typedef enum {
	PERFORMANCE  = 0,
	INTERACTIVE,
	GOVERNOR_MAX,
} cpu_governor_hw_st;

/* cpu police */
#define BOOST_INIT_RANGE		0

typedef struct cpu_hardware_sysctl {
	unsigned int cpu_minfreq;
	unsigned int cpu_maxfreq;
	unsigned int cpu_min_online;
	unsigned int cpu_max_online;
	unsigned int cpu_freq_governor;
}cpu_hw_st;

typedef struct gpu_hardware_sysctl {
	unsigned int gpu_freq;
	unsigned int gpu_performace;
}gpu_hw_st;

typedef struct thermal_hardware_sysctl {
	unsigned int enable;
	unsigned int adjust;
}the_hw_st;

typedef struct mem_hardware_sysctl {
	const char *zone_min_kbytes;
	const char *zone_extral_kbytes;
	const char *block_read_aheah_kbytes;
	const char *dirty_background_ratio;
	const char *dirty_ratio;
	const char *lmk_minfree;
}mem_hw_st;

typedef struct proc_software_sysctl {
	const char *inotify_item;
	const char *inotify_key;
	const char *triger_proc;
	int triger_pid;
	int item_event_mask;
	int key_event_mask;
#define INOTIFY_ITEM 0
#define INOTIFY_KEY  1
	int inotify_flag;
	int (*fn)(int, void*);
	int fn_int_para;
	void *fn_ptr_para;
}proc_sw_st;

typedef enum {
#define CPU_MAGIC       'c'
        CPU_MAGIC_SLOT = 0,
#define GPU_MAGIC       'g'
        GPU_MAGIC_SLOT,
#define THERMAL_MAGIC   't'
        THERMAL_MAGIC_SLOT,
#define MM_MAGIC        'm'
        MM_MAGIC_SLOT,
#define IDLE_MAGIC      'i'
        IDLE_MAGIC_SLOT,
#define EXT_MAGIC       'e'
        EXT_MAGIC_SLOT,
#define PRIO_MAGIC		'p'
		PRIO_MAGIC_SLOT,
// you can add more new magic
	MAX_MAGIC_SLOT,
}plat_hw_magic_slot;

typedef struct platform_hardware_slot {
	int pid;
	int slot_used;
        int slot_active[MAX_MAGIC_SLOT];
        int slot_value [MAX_MAGIC_SLOT];
}plat_hw_slot;

typedef struct platform_ops {
	int (*ops_cpu)(int range);
	int (*ops_gpu)(int range);
	int (*ops_the)(int range);
	int (*ops_mem)(int range);
	int (*ops_etc)(int range);
	int (*ops_idle)(int range);
	int (*ops_prio)(int pid, int range);
}plat_ops_st;

int aw_sysctrl_access(const char *path, int dentry);
int aw_sysctrl_set(const char *path, const char *buf);
int aw_sysctrl_get(const char *path, char *buf, int slen);
int aw_devctrl_set(const char *path, int data);
int aw_devctrl_get(const char *path, int *data);
int aw_create_thread(void *(*fn) (void *), void *para);
int aw_search_process(const char *procs);
int aw_inotify_watch(proc_sw_st *proc);
int aw_inotify_event(const char *path, int file_flag);
static void* aw_boostup_mode_detect(void *para);
int aw_boost_antutu_policy(const char *value);

#define FN_IMPLEMENT(fn) ((fn)?(fn):(0))

#if ((defined SUN8IW7P1))
#define H3_CPU_GOVERNOR_SYS 		"/sys/devices/system/cpu/cpu0/cpufreq/scaling_governor"
#define H3_CPU_MAXFREQ_SYS 			"/sys/devices/system/cpu/cpu0/cpufreq/scaling_max_freq"
#define H3_CPU_MINFREQ_SYS 			"/sys/devices/system/cpu/cpu0/cpufreq/scaling_min_freq"
#define H3_CPU_HOTPLUG_ENABLE_SYS	"/sys/kernel/autohotplug/enable"
#define H3_CPU_HOTPLUG_ENABLE		"1"

#define H3_CPU_ROOMAGE_SYS         "/sys/devices/platform/soc/cpu_budget_cooling/roomage"

#define H3_EMMC_BLOCK  			"/sys/block/mmcblk0"
#define H3_NAND_BLOCK  			"/sys/block/nandm"
#define H3_NAND_BLOCK_UDISK_READ_AHEAD	"/sys/block/nandm/queue/read_ahead_kb"
#define H3_NAND_BLOCK_SYSTEM_READ_AHEAD	"/sys/block/nandd/queue/read_ahead_kb"
#define H3_EMMC_BLOCK_SYSTEM_READ_AHEAD	"/sys/block/mmcblk0/queue/read_ahead_kb"

#define H3_MM_MINFREE_KBYTES   		"/proc/sys/vm/min_free_kbytes"
#define H3_MM_EXTRA_FREE_KBYTES   	"/proc/sys/vm/extra_free_kbytes"
#define H3_MM_DIRTY_RATIO      		"/proc/sys/vm/dirty_ratio"
#define H3_MM_DIRTY_BACK_RATIO		"/proc/sys/vm/dirty_background_ratio"
#define H3_MM_LMK_MINFREE           "/sys/module/lowmemorykiller/parameters/minfree"

#define H3_GPU_SCENECTRL_SYS           "/sys/devices/platform/gpu/scenectrl/command"
#define H3_GPU_PERFERMENCE             "1"
#define H3_GPU_NORMAL                  "0"

#define H3_THS_CPU_ENABLE_SYS			"/sys/devices/virtual/thermal/thermal_zone0/mode"

#define IMPLEMENT_PLATFORM_OPS(strname, fn)	\
	static plat_ops_st platform = { \
		.ops_cpu = strname##_cpu_##fn, \
		.ops_gpu = strname##_gpu_##fn, \
		.ops_the = strname##_the_##fn, \
		.ops_mem = strname##_mem_##fn, \
		.ops_idle = strname##_idle_##fn, \
		.ops_etc = strname##_etc_##fn, \
		.ops_prio = strname##_prio_##fn, \
	}
#endif

#if ((defined SUN50IW6P1))
#define H6_CPU_GOVERNOR_SYS 		"/sys/devices/system/cpu/cpu0/cpufreq/scaling_governor"
#define H6_CPU_MAXFREQ_SYS 			"/sys/devices/system/cpu/cpu0/cpufreq/scaling_max_freq"
#define H6_CPU_MINFREQ_SYS 			"/sys/devices/system/cpu/cpu0/cpufreq/scaling_min_freq"
#define H6_CPU_HOTPLUG_ENABLE_SYS	"/sys/kernel/autohotplug/enable"
#define H6_CPU_HOTPLUG_ENABLE		"1"

#define H6_CPU_ROOMAGE_SYS         "/sys/devices/soc/cpu_budget_cool/roomage"

#define H6_EMMC_BLOCK  			"/sys/block/mmcblk0"
#define H6_NAND_BLOCK  			"/sys/block/nandm"
#define H6_NAND_BLOCK_UDISK_READ_AHEAD	"/sys/block/nandm/queue/read_ahead_kb"
#define H6_NAND_BLOCK_SYSTEM_READ_AHEAD	"/sys/block/nandd/queue/read_ahead_kb"
#define H6_EMMC_BLOCK_SYSTEM_READ_AHEAD	"/sys/block/mmcblk0/queue/read_ahead_kb"

#define H6_MM_MINFREE_KBYTES   		"/proc/sys/vm/min_free_kbytes"
#define H6_MM_EXTRA_FREE_KBYTES   	"/proc/sys/vm/extra_free_kbytes"
#define H6_MM_DIRTY_RATIO      		"/proc/sys/vm/dirty_ratio"
#define H6_MM_DIRTY_BACK_RATIO		"/proc/sys/vm/dirty_background_ratio"
#define H6_MM_LMK_MINFREE           "/sys/module/lowmemorykiller/parameters/minfree"

#define H6_GPU_SCENECTRL_SYS           "/sys/devices/gpu/scenectrl/command"
#define H6_GPU_PERFERMENCE             "1"
#define H6_GPU_NORMAL                  "0"

#define H6_THS_CPU_ENABLE_SYS			"/sys/devices/virtual/thermal/thermal_zone0/mode"
#define H6_THS_GPU_ENABLE_SYS			"/sys/devices/virtual/thermal/thermal_zone1/mode"

#define H6_VE_SMP_AFFINITY			"/proc/irq/121/smp_affinity"
#define H6_VE_IRQ_POLICY			"0xf"

#define IMPLEMENT_PLATFORM_OPS(strname, fn)	\
	static plat_ops_st platform = { \
		.ops_cpu = strname##_cpu_##fn, \
		.ops_gpu = strname##_gpu_##fn, \
		.ops_the = strname##_the_##fn, \
		.ops_mem = strname##_mem_##fn, \
		.ops_idle = strname##_idle_##fn, \
		.ops_etc = strname##_etc_##fn, \
		.ops_prio = strname##_prio_##fn, \
	}
#endif

#ifdef SUN50IW2P1
#define H5_CPU_GOVERNOR_SYS 		"/sys/devices/system/cpu/cpu0/cpufreq/scaling_governor"
#define H5_CPU_MAXFREQ_SYS 			"/sys/devices/system/cpu/cpu0/cpufreq/scaling_max_freq"
#define H5_CPU_MINFREQ_SYS 			"/sys/devices/system/cpu/cpu0/cpufreq/scaling_min_freq"
#define H5_CPU_ROOMAGE_SYS         "/sys/devices/soc.0/cpu_budget_cool.16/roomage"
#define H5_CPU_HOTPLUG_ENABLE_SYS	"/sys/kernel/autohotplug/enable"
#define H5_CPU_HOTPLUG_ENABLE		"1"

#define H5_EMMC_BLOCK  			"/sys/block/mmcblk0"
#define H5_NAND_BLOCK  			"/sys/block/nandm"
#define H5_NAND_BLOCK_UDISK_READ_AHEAD	"/sys/block/nandm/queue/read_ahead_kb"
#define H5_NAND_BLOCK_SYSTEM_READ_AHEAD	"/sys/block/nandd/queue/read_ahead_kb"
#define H5_EMMC_BLOCK_SYSTEM_READ_AHEAD	"/sys/block/mmcblk0/queue/read_ahead_kb"

#define H5_MM_MINFREE_KBYTES   		"/proc/sys/vm/min_free_kbytes"
#define H5_MM_EXTRA_FREE_KBYTES   	"/proc/sys/vm/extra_free_kbytes"
#define H5_MM_DIRTY_RATIO      		"/proc/sys/vm/dirty_ratio"
#define H5_MM_DIRTY_BACK_RATIO		"/proc/sys/vm/dirty_background_ratio"
#define H5_MM_LMK_MINFREE           "/sys/module/lowmemorykiller/parameters/minfree"

#define IMPLEMENT_PLATFORM_OPS(strname, fn)	\
	static plat_ops_st platform = { \
		.ops_cpu = strname##_cpu_##fn, \
		.ops_gpu = strname##_gpu_##fn, \
		.ops_the = strname##_the_##fn, \
		.ops_mem = strname##_mem_##fn, \
		.ops_etc = strname##_etc_##fn, \
		.ops_idle = NULL, \
		.ops_prio = NULL, \
	}
#endif

#ifdef SUN50IW1P1
#define H64_CPU_GOVERNOR_SYS 		"/sys/devices/system/cpu/cpu0/cpufreq/scaling_governor"
#define H64_CPU_MAXFREQ_SYS 		"/sys/devices/system/cpu/cpu0/cpufreq/scaling_max_freq"
#define H64_CPU_MINFREQ_SYS 		"/sys/devices/system/cpu/cpu0/cpufreq/scaling_min_freq"
#define H64_CPU_ROOMAGE_SYS         	"/sys/devices/soc.0/cpu_budget_cool.16/roomage"
#define H64_CPU_HOTPLUG_ENABLE_SYS	"/sys/kernel/autohotplug/enable"
#define H64_CPU_HOTPLUG_ENABLE		"1"
#define H64_MM_MINFREE_KBYTES           "/proc/sys/vm/min_free_kbytes"
#define H64_MM_EXTRA_FREE_KBYTES        "/proc/sys/vm/extra_free_kbytes"
#define H64_MM_DIRTY_RATIO              "/proc/sys/vm/dirty_ratio"
#define H64_MM_DIRTY_BACK_RATIO         "/proc/sys/vm/dirty_background_ratio"
#define H64_BLOCK_UDISK_READ_AHEAD      "/sys/block/nando/queue/read_ahead_kb"
#define H64_BLOCK_SYSTEM_READ_AHEAD     "/sys/block/nandd/queue/read_ahead_kb"
#define H64_GPU_SCENETRL_SYS            "/sys/bus/platform/drivers/mali-utgard/1c40000.gpu/dvfs/scenectrl"
#define H64_GPU_ANDROID_SYS             "/sys/bus/platform/drivers/mali-utgard/1c40000.gpu/dvfs/android"
#define H64_GPU_PERFERMENCE             "1"
#define H64_GPU_NORMAL                  "0"
#define H64_DRAMFREQ_GOVERNOR			"/sys/class/devfreq/dramfreq/governor"
#define DRAMFREQ_POLICY					"temptrigger"

#define IMPLEMENT_PLATFORM_OPS(strname, fn)	\
	static plat_ops_st platform = { \
		.ops_cpu = strname##_cpu_##fn, \
		.ops_gpu = strname##_gpu_##fn, \
		.ops_the = strname##_the_##fn, \
		.ops_mem = strname##_mem_##fn, \
		.ops_etc = NULL, \
	}

/* cpu_sys user guide:
1. H64 cpu scaling_available_frequencies
   480000 600000 720000 816000 1008000 1104000 1152000 1200000
2. cpu_sys param4 as H64 cpu scaling available governor
   support: interactive conservative ondemand userspace powersave performance
3. cpu_sys[0] as the default config after system finished booting
*/
static cpu_hw_st cpu_sys[BOOST_MODE_RANGE] = {
	{0, 1152000, 0, 4, 1},
	{0, 1152000, 1, 4, 1},
	{0, 1152000, 2, 4, 1},
	{0, 1152000, 3, 4, 1},
	{0, 1152000, 4, 4, 1},
	{1152000, 1152000, 4, 4, 1},
	{0},
};

static char *cpu_governor[GOVERNOR_MAX] = {
	"performance",
	"interactive",
};

/* gpu_sys user guide:
1. H64 gpu available frequen tables
   H64 P1 support 552 456 360 144
2. gpu_sys. param1 just for gpu enable performance swaitch
   '0': do nothing ; '1': switch to performance mode
*/
static gpu_hw_st gpu_sys[BOOST_MODE_RANGE] = {
	{552, 0},
	{552, 1},
};
/* H64 thermal available range  tables  */
/* H64 P1 65 80 90 100 110  */
static the_hw_st the_sys[BOOST_MODE_RANGE] = {
	{1, 0},
};
/* H64 mem scaling_available_frequencies  */
static mem_hw_st mem_sys[BOOST_MODE_RANGE] = {
	{"6646", "32400", "32", "2", "5"},
};

int h64_cpu_opreate(int range)
{
	int ret = 0;
	char temp[128];

	if (IS_ERR_RANGE(range))
		return RETFAULT;

	if (range == BOOST_INIT_RANGE && BOOST_UP_INIT == false)
	{
		ret = aw_sysctrl_set(H64_CPU_GOVERNOR_SYS, cpu_governor[cpu_sys[range].cpu_freq_governor]);
		ret += aw_sysctrl_set(H64_CPU_HOTPLUG_ENABLE_SYS, H64_CPU_HOTPLUG_ENABLE);
		BOOST_UP_INIT = true;
	}

	memset(temp, 0, sizeof(temp));
	snprintf(temp, sizeof(temp), "%d %d %d %d %d %d %d %d %c",
				cpu_sys[range].cpu_minfreq, cpu_sys[range].cpu_min_online,
				0, 0,
				cpu_sys[range].cpu_maxfreq, cpu_sys[range].cpu_max_online,
				0, 0, '\0');
	ret += aw_sysctrl_set(H64_CPU_ROOMAGE_SYS, temp);
	return ret;
}

int h64_gpu_opreate(int range)
{
	int ret = 0;

	if (IS_ERR_RANGE(range))
		return RETFAULT;

	if (gpu_sys[range].gpu_performace == 1)
	{
		ret = aw_sysctrl_set(H64_GPU_SCENETRL_SYS, H64_GPU_PERFERMENCE);
		ret += aw_sysctrl_set(H64_GPU_ANDROID_SYS, H64_GPU_PERFERMENCE);
	}
	else if (gpu_sys[range].gpu_performace == 0)
	{
		ret = aw_sysctrl_set(H64_GPU_ANDROID_SYS, H64_GPU_NORMAL);
	}
	else
	{

	}
	return 0;
}

int h64_the_opreate(int range)
{
	if (IS_ERR_RANGE(range))
		return RETFAULT;

	return 0;
}

int h64_mem_opreate(int range)
{
	int ret = 0;

	if (IS_ERR_RANGE(range))
		return RETFAULT;

	if (range == BOOST_INIT_RANGE)
	{
		ret = aw_sysctrl_set(H64_BLOCK_SYSTEM_READ_AHEAD, mem_sys[range].block_read_aheah_kbytes);
		ret += aw_sysctrl_set(H64_BLOCK_UDISK_READ_AHEAD, mem_sys[range].block_read_aheah_kbytes);
		ret += aw_sysctrl_set(H64_DRAMFREQ_GOVERNOR, DRAMFREQ_POLICY);
	}
	ret += aw_sysctrl_set(H64_MM_MINFREE_KBYTES, mem_sys[range].zone_min_kbytes);
	ret += aw_sysctrl_set(H64_MM_EXTRA_FREE_KBYTES, mem_sys[range].zone_extral_kbytes);
	ret += aw_sysctrl_set(H64_MM_DIRTY_BACK_RATIO, mem_sys[range].dirty_background_ratio);
	ret += aw_sysctrl_set(H64_MM_DIRTY_RATIO, mem_sys[range].dirty_ratio);

	return ret;
}

IMPLEMENT_PLATFORM_OPS(h64, opreate);


int aw_platform_init(int enable)
{
	int ret = 0;
	if (enable == BOOST_ENABLE)
	{
		ret = FN_IMPLEMENT(platform.ops_cpu(BOOST_INIT_RANGE));
		ret += FN_IMPLEMENT(platform.ops_mem(BOOST_INIT_RANGE));
	}
	return ret;
}

#endif

#ifdef SUN50IW2P1
/* cpu_sys user guide:
1. H5 cpu scaling_available_frequencies
   480000 600000 720000 816000 1008000 1104000 1152000 1200000
2. cpu_sys param4 as H64 cpu scaling available governor
   support: interactive conservative ondemand userspace powersave performance
3. cpu_sys[0] as the default config after system finished booting
*/
static cpu_hw_st cpu_sys[BOOST_MODE_RANGE] = {
	{0,			1008000, 0, 4, 1},
	{0,			1008000, 1, 4, 1},
	{0,			1008000, 2, 4, 1},
	{0,			1008000, 3, 4, 1},
	{0,			1008000, 4, 4, 1},
	{816000,	816000, 1, 4, 1},
	{1008000,	1008000, 4, 4, 1},
	{0, 0, 0, 0, 0},
};

static const char *cpu_governor[GOVERNOR_MAX] = {
	"performance",
	"interactive",
};

/* H5 mem limited adjust   */
static mem_hw_st mem_sys[BOOST_MODE_RANGE] = {
	{"6646", "43200", "32", "2", "5",
	 "18432,27648,36864,46080,51200,56320"},
};

static proc_sw_st proc_sw[BOOST_MODE_RANGE] = {
	{"/mnt/sdcard/", "/mnt/sdcard/monkey.log", "commands.monkey", 0,
	  IN_CREATE, IN_OPEN, 0, NULL, 0, NULL},
};

int h5_cpu_opreate(int range)
{
	int ret = 0;
	char temp[128];

	if (IS_ERR_RANGE(range))
		return RETFAULT;

	if (range == BOOST_INIT_RANGE && BOOST_UP_INIT == false)
	{
		ret = aw_sysctrl_set(H5_CPU_GOVERNOR_SYS, cpu_governor[cpu_sys[range].cpu_freq_governor]);
		ret += aw_sysctrl_set(H5_CPU_HOTPLUG_ENABLE_SYS, H5_CPU_HOTPLUG_ENABLE);
		BOOST_UP_INIT = true;
	}

	memset(temp, 0, sizeof(temp));
	snprintf(temp, sizeof(temp), "%d %d %d %d %d %d %d %d %c",
				cpu_sys[range].cpu_minfreq, cpu_sys[range].cpu_min_online,
				0, 0,
				cpu_sys[range].cpu_maxfreq, cpu_sys[range].cpu_max_online,
				0, 0, '\0');
	ret += aw_sysctrl_set(H5_CPU_ROOMAGE_SYS, temp);
	return ret;
}

int h5_gpu_opreate(int range)
{
	if (IS_ERR_RANGE(range))
		return RETFAULT;

	return 0;
}

int h5_the_opreate(int range)
{
	if (IS_ERR_RANGE(range))
		return RETFAULT;

	return 0;
}

int h5_watch_callback(int para, void *ptr)
{
	int pid;
	int retry = 2;
	char comm[128];
	proc_sw_st *tmp;

	if (!ptr)
		return -1;

	tmp = (proc_sw_st *)ptr;
	if (!tmp->triger_proc)
		return -1;

	B_FUNC_TRACE_START();
	/*make sure the monitored process is still exist, if exist, just return*/
	if (tmp->triger_pid) {
		snprintf(comm, sizeof(comm), "/proc/%d/comm", tmp->triger_pid);
		if (!aw_sysctrl_access(comm, 0)) {
			return -1;
		} else {
			tmp->triger_pid = 0;
		}
	}

	/*make sure the notify event is crrect,if not, just return*/
	if (tmp->inotify_flag == INOTIFY_ITEM) {
		if (aw_sysctrl_access(tmp->inotify_key, 0)<0)
		return -1;
	}

	while(retry--) {
		pid = aw_search_process(tmp->triger_proc);
		if (pid < 0)
			sleep(3);
	}

	if (pid < 0)
		return -1;

	tmp->triger_pid = pid;
	B_FUNC_TRACE_END("ret %d %s\n", tmp->triger_pid, tmp->triger_proc);
	return 0;
}

void *h5_watch_handle(void* para)
{
	if (!para)
		return NULL;

	B_FUNC_TRACE_START();
	proc_sw_st *tmp = (proc_sw_st *)para;
	tmp->fn = h5_watch_callback;
	tmp->fn_int_para = 0;
	tmp->fn_ptr_para = (void *)tmp;
	aw_inotify_watch(tmp);

	return NULL;
}

int h5_etc_opreate(int range)
{
	if (IS_ERR_RANGE(range))
		return RETFAULT;

	B_FUNC_TRACE_START();
	if (range == BOOST_INIT_RANGE) {
		aw_create_thread(h5_watch_handle, (void *)&proc_sw[BOOST_INIT_RANGE]);
	}
	return 0;
}

int h5_mem_opreate(int range)
{
	int ret = 0;

	if (IS_ERR_RANGE(range))
		return RETFAULT;

	if (range == BOOST_INIT_RANGE)
	{
		if (opendir(H5_EMMC_BLOCK))
			ret = aw_sysctrl_set(H5_EMMC_BLOCK_SYSTEM_READ_AHEAD, mem_sys[range].block_read_aheah_kbytes);

		if (opendir(H5_NAND_BLOCK)) {
			ret = aw_sysctrl_set(H5_NAND_BLOCK_SYSTEM_READ_AHEAD, mem_sys[range].block_read_aheah_kbytes);
			ret += aw_sysctrl_set(H5_NAND_BLOCK_UDISK_READ_AHEAD, mem_sys[range].block_read_aheah_kbytes);
		}

		ret += aw_sysctrl_set(H5_MM_MINFREE_KBYTES, mem_sys[range].zone_min_kbytes);
		ret += aw_sysctrl_set(H5_MM_EXTRA_FREE_KBYTES, mem_sys[range].zone_extral_kbytes);
		ret += aw_sysctrl_set(H5_MM_DIRTY_BACK_RATIO, mem_sys[range].dirty_background_ratio);
		ret += aw_sysctrl_set(H5_MM_DIRTY_RATIO, mem_sys[range].dirty_ratio);
		chmod("H5_MM_LMK_MINFREE", 0640);
		ret += aw_sysctrl_set(H5_MM_LMK_MINFREE, mem_sys[range].lmk_minfree);
		chmod("H5_MM_LMK_MINFREE", 0220);

	}

	return ret;
}

IMPLEMENT_PLATFORM_OPS(h5, opreate);

int aw_platform_init(int enable)
{
	int ret = 0;
	if (enable == BOOST_ENABLE)
	{
		ret = FN_IMPLEMENT(platform.ops_cpu(BOOST_INIT_RANGE));
		ret += FN_IMPLEMENT(platform.ops_mem(BOOST_INIT_RANGE));
		ret += FN_IMPLEMENT(platform.ops_etc(BOOST_INIT_RANGE));
	}
	return ret;
}
#endif

#if ((defined SUN50IW6P1))
/* cpu_sys user guide:
1. H6 cpu scaling_available_frequencies
   480000 600000 720000 816000 1008000 1104000 1152000 1200000
2. cpu_sys param4 as H64 cpu scaling available governor
   support: interactive conservative ondemand userspace powersave performance
3. cpu_sys[0] as the default config after system finished booting
*/
static cpu_hw_st cpu_sys[BOOST_MODE_RANGE] = {
	{0,			1800000,	1,	4,	1},
	{1800000,	1800000,	4,	4,	1},
	{816000,	1800000,	1,	4,	1},
	{816000,	1800000,	2,	4,	1},
	{480000,	1800000,	2,	4,	1},
};

static const char *cpu_governor[GOVERNOR_MAX] = {
	"performance",
	"interactive",
};

/* H6 mem limited adjust   */
static mem_hw_st mem_sys[BOOST_MODE_RANGE] = {
	{"6646", "86400", "32", "2", "5",
	 "18432,27648,36864,46080,51200,56320"},
};

int h6_cpu_opreate(int range)
{
	int ret = 0;
	char temp[128];

	if (IS_ERR_RANGE(range))
		return RETFAULT;

	if (range == BOOST_INIT_RANGE && BOOST_UP_INIT == false)
	{
		ret = aw_sysctrl_set(H6_CPU_GOVERNOR_SYS, cpu_governor[cpu_sys[range].cpu_freq_governor]);
		ret = aw_sysctrl_set(H6_CPU_HOTPLUG_ENABLE_SYS, H6_CPU_HOTPLUG_ENABLE);
		ret = aw_sysctrl_set(H6_VE_SMP_AFFINITY, H6_VE_IRQ_POLICY);
		BOOST_UP_INIT = true;
	}
	memset(temp, 0, sizeof(temp));
	snprintf(temp, sizeof(temp), "%d %d %d %d %d %d %d %d %c",
				cpu_sys[range].cpu_minfreq, cpu_sys[range].cpu_min_online,
				0, 0,
				cpu_sys[range].cpu_maxfreq, cpu_sys[range].cpu_max_online,
				0, 0, '\0');
	ret += aw_sysctrl_set(H6_CPU_ROOMAGE_SYS, temp);
	return ret;
}

int h6_gpu_opreate(int range)
{
	int ret = 0;

	if (IS_ERR_RANGE(range))
		return RETFAULT;

	if (!range)
	{
		ret = aw_sysctrl_set(H6_GPU_SCENECTRL_SYS, H6_GPU_NORMAL);
	}
	else if (range == 1)
	{
		ret = aw_sysctrl_set(H6_GPU_SCENECTRL_SYS, H6_GPU_PERFERMENCE);
	}

	return 0;
}

int h6_the_opreate(int range)
{
	if (IS_ERR_RANGE(range))
		return RETFAULT;

	if (range == 1)
	{
		aw_sysctrl_set(H6_THS_CPU_ENABLE_SYS, "enabled");
		aw_sysctrl_set(H6_THS_GPU_ENABLE_SYS, "enabled");
	}
	else if (range == 0)
	{
		aw_sysctrl_set(H6_THS_CPU_ENABLE_SYS, "disabled");
		aw_sysctrl_set(H6_THS_GPU_ENABLE_SYS, "disabled");
	}
	return 0;
}

int h6_mem_opreate(int range)
{
	int ret = 0;

	if (IS_ERR_RANGE(range))
		return RETFAULT;

	if (range == BOOST_INIT_RANGE)
	{
		if (opendir(H6_EMMC_BLOCK))
			ret = aw_sysctrl_set(H6_EMMC_BLOCK_SYSTEM_READ_AHEAD, mem_sys[range].block_read_aheah_kbytes);

		if (opendir(H6_NAND_BLOCK)) {
			ret = aw_sysctrl_set(H6_NAND_BLOCK_SYSTEM_READ_AHEAD, mem_sys[range].block_read_aheah_kbytes);
			ret += aw_sysctrl_set(H6_NAND_BLOCK_UDISK_READ_AHEAD, mem_sys[range].block_read_aheah_kbytes);
		}

		ret += aw_sysctrl_set(H6_MM_MINFREE_KBYTES, mem_sys[range].zone_min_kbytes);
		ret += aw_sysctrl_set(H6_MM_EXTRA_FREE_KBYTES, mem_sys[range].zone_extral_kbytes);
		ret += aw_sysctrl_set(H6_MM_DIRTY_BACK_RATIO, mem_sys[range].dirty_background_ratio);
		ret += aw_sysctrl_set(H6_MM_DIRTY_RATIO, mem_sys[range].dirty_ratio);
		chmod("H6_MM_LMK_MINFREE", 0640);
		ret += aw_sysctrl_set(H6_MM_LMK_MINFREE, mem_sys[range].lmk_minfree);
		chmod("H6_MM_LMK_MINFREE", 0220);
	}
	return ret;
}

int h6_idle_opreate(int range)
{
	int ret = 0;

	if (IS_ERR_RANGE(range))
		return RETFAULT;

	B_INF("h6 idle_opreate range=%x\n", range);
	if (range <= 0xff && range > 10) {		// 11~255: cpu_dma_latency
		ret += aw_devctrl_set(CPU_DMA_LATENCY_PATH, range);
	} else if ((range & 0xff) ==  CPU_IDLE_STATE_MAGIC) {
		switch(range) {
			case CPU_IDLE_STATE0_ENABLE:
				aw_sysctrl_set(CPU_IDLE_STATE0_PATH, CPU_IDLE_STATE_ENABLE);
				break;
			case CPU_IDLE_STATE0_DISABLE:
				aw_sysctrl_set(CPU_IDLE_STATE0_PATH, CPU_IDLE_STATE_DISABLE);
				break;
			case CPU_IDLE_STATE1_ENABLE:
				aw_sysctrl_set(CPU_IDLE_STATE1_PATH, CPU_IDLE_STATE_ENABLE);
				break;
			case CPU_IDLE_STATE1_DISABLE:
				aw_sysctrl_set(CPU_IDLE_STATE1_PATH, CPU_IDLE_STATE_DISABLE);
				break;
			default:
				;
		}
	} else {
		//no things
	}

	return ret;
}

int h6_etc_opreate(int range)
{

	if (IS_ERR_RANGE(range))
		return RETFAULT;

	return 0;
}

int h6_prio_opreate(int pid, int range)
{
	if (range == 0xff) {
		int maxpri;
		struct sched_param param;
		maxpri = sched_get_priority_max(SCHED_FIFO);
		param.sched_priority = maxpri;
		if (sched_setscheduler(pid, SCHED_FIFO, &param) == -1) {
			ERROR("sched_setscheduler %s!\n", strerror(errno));
			return -1;
		}
	} else if (range >= 100 && range <= 120){
		if (setpriority(PRIO_PROCESS, pid, (range - 120))) {
			ERROR("setpriority %s!\n", strerror(errno));
			return -1;
		}
	}
	return 0;
}
IMPLEMENT_PLATFORM_OPS(h6, opreate);


int aw_platform_init(int enable)
{
	int ret = 0;
	if (enable == BOOST_ENABLE)
	{
		ret = FN_IMPLEMENT(platform.ops_cpu(BOOST_INIT_RANGE));
		ret += FN_IMPLEMENT(platform.ops_mem(BOOST_INIT_RANGE));

		//test
		ret += FN_IMPLEMENT(platform.ops_etc(BOOST_INIT_RANGE));
	}
	return ret;
}

#endif

#if ((defined SUN8IW7P1))
/* cpu_sys user guide:
1. H6 cpu scaling_available_frequencies
   480000 600000 720000 816000 1008000 1104000 1152000 1200000
2. cpu_sys param4 as H64 cpu scaling available governor
   support: interactive conservative ondemand userspace powersave performance
3. cpu_sys[0] as the default config after system finished booting
*/
static cpu_hw_st cpu_sys[BOOST_MODE_RANGE] = {
	{0,			1200000,	1,	4,	1},
	{1200000,	1200000,	4,	4,	1},
	{816000,	1200000,	1,	4,	1},
	{480000,	1200000,	2,	4,	1},
};

static const char *cpu_governor[GOVERNOR_MAX] = {
	"performance",
	"interactive",
};

/* H3 mem limited adjust   */
static mem_hw_st mem_sys[BOOST_MODE_RANGE] = {
	{"6646", "86400", "32", "2", "5",
	 "18432,27648,36864,46080,51200,56320"},
};

int h3_cpu_opreate(int range)
{
	int ret = 0;
	char temp[128];

	if (IS_ERR_RANGE(range))
		return RETFAULT;

	if (range == BOOST_INIT_RANGE && BOOST_UP_INIT == false)
	{
		ret = aw_sysctrl_set(H3_CPU_GOVERNOR_SYS, cpu_governor[cpu_sys[range].cpu_freq_governor]);
		ret = aw_sysctrl_set(H3_CPU_HOTPLUG_ENABLE_SYS, H3_CPU_HOTPLUG_ENABLE);
		BOOST_UP_INIT = true;
	}
	memset(temp, 0, sizeof(temp));
	snprintf(temp, sizeof(temp), "%d %d %d %d %d %d %d %d %c",
				cpu_sys[range].cpu_minfreq, cpu_sys[range].cpu_min_online,
				0, 0,
				cpu_sys[range].cpu_maxfreq, cpu_sys[range].cpu_max_online,
				0, 0, '\0');
	ret += aw_sysctrl_set(H3_CPU_ROOMAGE_SYS, temp);
	return ret;
}

int h3_gpu_opreate(int range)
{
	int ret = 0;

	if (IS_ERR_RANGE(range))
		return RETFAULT;

	if (!range)
	{
		ret = aw_sysctrl_set(H3_GPU_SCENECTRL_SYS, H3_GPU_NORMAL);
	}
	else if (range == 1)
	{
		ret = aw_sysctrl_set(H3_GPU_SCENECTRL_SYS, H3_GPU_PERFERMENCE);
	}

	return 0;
}

int h3_the_opreate(int range)
{
	if (IS_ERR_RANGE(range))
		return RETFAULT;

	if (range == 1)
	{
		aw_sysctrl_set(H3_THS_CPU_ENABLE_SYS, "enabled");
	}
	else if (range == 0)
	{
		aw_sysctrl_set(H3_THS_CPU_ENABLE_SYS, "disabled");
	}
	return 0;
}

int h3_mem_opreate(int range)
{
	int ret = 0;

	if (IS_ERR_RANGE(range))
		return RETFAULT;

	if (range == BOOST_INIT_RANGE)
	{
		if (opendir(H3_EMMC_BLOCK))
			ret = aw_sysctrl_set(H3_EMMC_BLOCK_SYSTEM_READ_AHEAD, mem_sys[range].block_read_aheah_kbytes);

		if (opendir(H3_NAND_BLOCK)) {
			ret = aw_sysctrl_set(H3_NAND_BLOCK_SYSTEM_READ_AHEAD, mem_sys[range].block_read_aheah_kbytes);
			ret += aw_sysctrl_set(H3_NAND_BLOCK_UDISK_READ_AHEAD, mem_sys[range].block_read_aheah_kbytes);
		}

		ret += aw_sysctrl_set(H3_MM_MINFREE_KBYTES, mem_sys[range].zone_min_kbytes);
		ret += aw_sysctrl_set(H3_MM_EXTRA_FREE_KBYTES, mem_sys[range].zone_extral_kbytes);
		ret += aw_sysctrl_set(H3_MM_DIRTY_BACK_RATIO, mem_sys[range].dirty_background_ratio);
		ret += aw_sysctrl_set(H3_MM_DIRTY_RATIO, mem_sys[range].dirty_ratio);
		chmod("H6_MM_LMK_MINFREE", 0640);
		ret += aw_sysctrl_set(H3_MM_LMK_MINFREE, mem_sys[range].lmk_minfree);
		chmod("H3_MM_LMK_MINFREE", 0220);
	}
	return ret;
}

int h3_idle_opreate(int range)
{
	int ret = 0;
	return ret;
}

int h3_etc_opreate(int range)
{

	if (IS_ERR_RANGE(range))
		return RETFAULT;

	return 0;
}

int h3_prio_opreate(int pid, int range)
{
	if (range == 0xff) {
		int maxpri;
		struct sched_param param;
		maxpri = sched_get_priority_max(SCHED_FIFO);
		param.sched_priority = maxpri;
		if (sched_setscheduler(pid, SCHED_FIFO, &param) == -1) {
			ERROR("sched_setscheduler %s!\n", strerror(errno));
			return -1;
		}
	} else if (range >= 100 && range <= 120){
		if (setpriority(PRIO_PROCESS, pid, (range - 120))) {
			ERROR("setpriority %s!\n", strerror(errno));
			return -1;
		}
	}
	return 0;
}
IMPLEMENT_PLATFORM_OPS(h3, opreate);


int aw_platform_init(int enable)
{
	int ret = 0;
	if (enable == BOOST_ENABLE)
	{
		ret = FN_IMPLEMENT(platform.ops_cpu(BOOST_INIT_RANGE));
		ret += FN_IMPLEMENT(platform.ops_mem(BOOST_INIT_RANGE));
	}
	return ret;
}

#endif

int aw_set_roomage(const char *path, char *buf)
{
	char tmp[128];
	int i;
	int fp;
	for (i = 20; i > 0; i--) {
		snprintf(tmp, sizeof(tmp), path, i);
		fp = open(tmp, O_RDONLY);
		if (fp < 0)
			continue;
		close(fp);
		aw_sysctrl_set(tmp, buf);
		return 0;
	}
	return -1;
}

int aw_get_magic(char magic, int magic_value,  plat_hw_slot *slot)
{
	int ret = -1;
	switch (magic)
	{
		case CPU_MAGIC:
			ret = CPU_MAGIC_SLOT;
		break;
		case GPU_MAGIC:
			ret = GPU_MAGIC_SLOT;
		break;
		case THERMAL_MAGIC:
			ret = THERMAL_MAGIC_SLOT;
		break;
		case MM_MAGIC:
			ret = MM_MAGIC_SLOT;
		break;
		case IDLE_MAGIC:
			ret = IDLE_MAGIC_SLOT;
		break;
		case EXT_MAGIC:
			ret = EXT_MAGIC_SLOT;
		break;
		case PRIO_MAGIC:
			ret = PRIO_MAGIC_SLOT;
		break;
		default:
			B_LOG("unknow magic %c\n", magic);
			goto out;
		break;
	}
	slot->slot_active[ret] = 1;
	slot->slot_value[ret] = magic_value;
	B_LOG("get magic:%c value:%d \n", magic, magic_value);
out:
	return ret;
}
/*
aw_get_para user guid
1. boost just support  mode{x}{$PID:x}{c:x}{g:x}{t:x}{m:x}
   mode{x} : x you can set any magic character you want
   {$PID:x}: pid is the major process pid ,x is the numbers of platform item
   {c:x}   : c is the cpu item of platform, x is the range you seltect
   {g:x}   : g is the gpu item of platform, x is the range you seltect
   {t:x}   : t is the thermal item of platform, x is the range you seltect
   {m:x}   : m is the memory item of platform, x is the range you seltect

   for example process 1009 want to control cpu and gpu to special mode, it can set
   : mode1009:4:c:1:g:1
   for example process 2880 want to control all item to special mode, it can set
   : mode2880:4:c:1:g:1:t:3:m:2
*/
int aw_get_para(const char *value, plat_hw_slot *hw_slot)
{
	/* moden1009:3:c:1:g:1:t:0:m:0 */
	char para_magic[8] = { 0 };
	int mpid, slot = 0;
	int ret = 0;
	char magic_a, magic_b, magic_c, magic_d, magic_e;
	int value_a, value_b, value_c, value_d, value_e;

	magic_a = magic_b = magic_c = magic_d = 0;
	value_a = value_b = value_c = value_d = 0;
	ret = sscanf(value, "%[a-z]%d:%d:%c:%d:%c:%d:%c:%d:%c:%d:%c:%d",
					para_magic,
					&mpid, &slot,
					&magic_a, &value_a,
					&magic_b, &value_b,
					&magic_c, &value_c,
					&magic_d, &value_d,
					&magic_e, &value_e);
	if (ret < 0)
	{
		ERROR("boost prop value err:%d %s!\n", ret, strerror(errno));
		goto err_out;
	}
	B_LOG("magic=%s, pid=%d, slot=%d ret=%d, %c=%d , %c=%d, %c=%d, %c=%d %c=%d\n",
		para_magic, mpid, slot, ret, magic_a, value_a, magic_b, value_b,
		magic_c, value_c, magic_d, value_d, magic_e, value_e);
	if (ret > 0)
	{
		hw_slot->pid = mpid;
		hw_slot->slot_used = slot;
		if (magic_a)
			aw_get_magic(magic_a, value_a, hw_slot);
		if (magic_b)
			aw_get_magic(magic_b, value_b, hw_slot);
		if (magic_c)
			aw_get_magic(magic_c, value_c, hw_slot);
		if (magic_d)
			aw_get_magic(magic_d, value_d, hw_slot);
		if (magic_e)
			aw_get_magic(magic_e, value_e, hw_slot);
	}
	return 0;
err_out:
	 return -1;
}

#ifdef FS_READ_WTITE
int aw_sysctrl_set(const char *path, char *buf)
{
	FILE *sysctrl_fd;
	unsigned int ret = 0;

	sysctrl_fd = fopen(path, "w");
	if (sysctrl_fd == NULL)
	{
		ERROR("boost open %s fail,%s!\n",path, strerror(errno));
		goto err_out;
	}

	ret = fwrite(buf, 1, strlen(buf), sysctrl_fd);
	if (ret != strlen(buf))
	{
		ERROR("boost write fail,%s!\n",strerror(errno));
		fclose(sysctrl_fd);
		goto err_out;
	}

	fflush(sysctrl_fd);
	fclose(sysctrl_fd);

	return 0;
err_out:
	return -1;
}

int aw_sysctrl_get(const char *path, char *buf, int slen)
{
	FILE *sysctrl_fd;
	int ret = 0;

	sysctrl_fd = fopen(path, "r");
	if (sysctrl_fd == NULL)
	{
		ERROR("boost open fail,%s!\n",strerror(errno));
		goto err_out;
	}

	ret = fread(buf, slen, 1, sysctrl_fd);
	if (ret < 0)
	{
		ERROR("boost read fail,%s!\n",strerror(errno));
		fclose(sysctrl_fd);
		goto err_out;
	}

	fclose(sysctrl_fd);
	return 0;
err_out:
	return -1;
}

#else

int aw_sysctrl_access(const char *path, int dentry)
{
	DIR *dir;
	int fd;

	if (!path)
		return -1;

	if (dentry) {
		dir = opendir(path);
		if(!dir)
			return -1;
		else {
			closedir(dir);
			return 0;
		}
	}

	fd = open(path, O_RDONLY);
	if (fd<0)
		return -1;
	else {
		close(fd);
		return 0;
	}

	return -1;
}

int aw_sysctrl_get(const char *path, char *buf, int slen)
{
	int fd;
	int numread;

	fd = open(path, O_RDONLY);
	if (fd == -1)
	{
		ERROR("boost open fail,%s!\n",strerror(errno));
		goto err_out;
	}
	numread = read(fd, buf, slen - 1);
	if (numread < 1) {
		ERROR("boost read fail,%s!\n",strerror(errno));
		close(fd);
		goto err_out;
	}

	buf[numread] = '\0';
	close(fd);
	B_LOG(" get %s %s seccess!\n", path, buf);
	return 0;
err_out:
	return -1;
}

int aw_sysctrl_set(const char *path, const char *buf)
{
	int fd;
	int numwrite;

	fd = open(path, O_WRONLY);
	if (fd == -1)
	{
		ERROR("boost open %s fail,%s!\n",path, strerror(errno));
		goto err_out;
	}
	numwrite = write(fd, buf, strlen(buf));
	if (numwrite < 1)
	{
		ERROR("boost read  %s fail,%s!\n",path,strerror(errno));
		close(fd);
		goto err_out;
	}
	close(fd);
	B_LOG(" set %s %s seccess!\n", path, buf);
	return 0;
err_out:
	return -1;
}

#endif

static void aw_boostup_set_mode(int mpid, int mode)
{
	int cur_mode = 0;
	int pre_mode = 0;
	int active = 0;
	int detect_pid = 0;

	switch(mode) {
		case AW_BOOST_UP_MODE_NORMAL:
				cur_mode = AW_BOOST_UP_MODE_NORMAL;
				pre_mode = bmode.mode_cur;
				detect_pid = mpid;
				active = 0;
				break;
		case AW_BOOST_UP_MODE_MEDIA:
				cur_mode = AW_BOOST_UP_MODE_MEDIA;
				pre_mode = bmode.mode_cur;
				detect_pid = mpid;
				active = 1;
				break;
		case AW_BOOST_UP_MODE_BENCHMARK:
				cur_mode = AW_BOOST_UP_MODE_BENCHMARK;
				pre_mode = bmode.mode_cur;
				detect_pid = mpid;
				active = 1;
				break;
		case AW_BOOST_UP_MODE_SYSTEM:
				cur_mode = AW_BOOST_UP_MODE_SYSTEM;
				pre_mode = bmode.mode_cur;
				detect_pid = mpid;
				active = 1;
				break;
		default: ;
	}

	bmode.mode_cur = cur_mode;
    bmode.mode_pre = pre_mode;
    blist.hold_pid = detect_pid;
    blist.mode_cur = cur_mode;
    blist.mode_pre = pre_mode;
    blist.slot_active = active;
    B_LOG("cur=%d, pre=%d, pid=%d, active=%d \n", cur_mode, pre_mode, detect_pid, active);
}

static int aw_boost_normal(void)
{
	int ret = 0;

	if (bmode.mode_cur != AW_BOOST_UP_MODE_NORMAL)
		ret = FN_IMPLEMENT(platform.ops_cpu(BOOST_INIT_RANGE));

	aw_boostup_set_mode(0, AW_BOOST_UP_MODE_NORMAL);
	return ret;
}

static void* aw_boostup_mode_detect(void *para)
{
	char dirname[20];
	DIR *dir;

	B_LOG("%s: boost thread pid %d tid %d \n",__func__, getpid(), (int)pthread_self());
	while(1)
	{
		if (blist.slot_active)
		{
			B_LOG("%s: boost mode_index %d pid %d\n",__func__,blist.mode_cur, blist.hold_pid);
			snprintf(dirname,sizeof(dirname),"/proc/%d",blist.hold_pid);
			dir = opendir(dirname);
			if(dir == NULL)
			{
				aw_boost_normal();
			}
			else
				closedir(dir);
		}
#if 0
		if ((user_detect == USER_ACTIVE) || (sys_detect = USER_ACTIVE))
		{
		}
#endif
		sleep(detect_grap);
	}

	return NULL;
}

int aw_init_boostup(void)
{
	int err = 0;
	pthread_t bst = 0;
	err = pthread_create(&bst, NULL, aw_boostup_mode_detect, NULL);
	if (err != 0)
	{
		ERROR(" ERR: boostup thread init fail, %s !\n", strerror(err));
		return -1;
	}
	pthread_detach(bst);
	return 0;
}

int aw_boost_set_policy(const char *value, plat_hw_slot *hw_slot_p)
{
	int ret;

	if (IS_MATCH_ITEM(BOOST_MAGIC, value))
		return -1;

	ret = aw_get_para(value, hw_slot_p);
	if (ret < 0)
		return -1;

	if (hw_slot_p->slot_active[CPU_MAGIC_SLOT]) {
		ret += FN_IMPLEMENT(platform.ops_cpu(hw_slot_p->slot_value[CPU_MAGIC_SLOT]));
		hw_slot_p->slot_active[CPU_MAGIC_SLOT] = 0;
	}
	if (hw_slot_p->slot_active[GPU_MAGIC_SLOT]) {
		ret += FN_IMPLEMENT(platform.ops_gpu(hw_slot_p->slot_value[GPU_MAGIC_SLOT]));
		hw_slot_p->slot_active[GPU_MAGIC_SLOT] = 0;
	}
	if (hw_slot_p->slot_active[THERMAL_MAGIC_SLOT]) {
		ret += FN_IMPLEMENT(platform.ops_the(hw_slot_p->slot_value[THERMAL_MAGIC_SLOT]));
		hw_slot_p->slot_active[THERMAL_MAGIC_SLOT] = 0;
	}
	if (hw_slot_p->slot_active[MM_MAGIC_SLOT]) {
		ret += FN_IMPLEMENT(platform.ops_mem(hw_slot_p->slot_value[MM_MAGIC_SLOT]));
		hw_slot_p->slot_active[MM_MAGIC_SLOT] = 0;
	}
	if (hw_slot_p->slot_active[PRIO_MAGIC_SLOT]) {
		ret += FN_IMPLEMENT(platform.ops_prio(hw_slot_p->pid, hw_slot_p->slot_value[PRIO_MAGIC_SLOT]));
		hw_slot_p->slot_active[PRIO_MAGIC_SLOT] = 0;
	}
	return 0;
}

int aw_devctrl_set(const char *path, int data)
{
	int fd;
	int numwrite;

	fd = open(path, O_WRONLY);
	if (fd == -1)
	{
		ERROR("boost open %s fail,%s!\n",path, strerror(errno));
		goto err_out;
	}
	numwrite = write(fd, &data, sizeof(int));
	if (numwrite < 1)
	{
		ERROR("boost read  %s fail,%s!\n",path,strerror(errno));
		close(fd);
		goto err_out;
	}
	close(fd);
	B_LOG(" set %s %d seccess!\n", path, data);
	return 0;
err_out:
	return -1;
}

int aw_devctrl_get(const char *path, int *data)
{
	int fd;
	int numread;

	fd = open(path, O_RDONLY);
	if (fd == -1)
	{
		ERROR("boost open fail,%s!\n",strerror(errno));
		goto err_out;
	}
	numread = read(fd, data,  sizeof(int));
	if (numread < 1) {
		ERROR("boost read fail,%s!\n",strerror(errno));
		close(fd);
		goto err_out;
	}
	close(fd);
	B_LOG(" get %s %d seccess!\n", path, *data);
	return 0;
err_out:
	return -1;
}

int aw_create_thread(void *(*fn) (void *), void *para)
{
	pthread_t thr;
    //pthread_attr_t attr;
	//pthread_attr_init(&attr);
	//pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

	if (pthread_create(&thr, NULL, fn, para)) {
		ERROR("%s, %s !\n", __func__, strerror(errno));
		return RETFAULT;
	}
	pthread_detach(thr);
	return RETFINE;
}

int aw_search_process(const char *procs)
{
	#define COMM_LEN	128
	int id = 0;
	int procs_pid = 0;
	int procs_start = 1000;
	int procs_end = 0xffff;
	int procs_target = 0;
	char procs_com[COMM_LEN];
	char comm[COMM_LEN];
	DIR* proc_dir;
	struct dirent *proc_entry;
	FILE *fp;

	if (!procs)
		return RETFAULT;

    proc_dir = opendir("/proc");
    if (proc_dir == NULL)
        return RETFAULT;

	B_FUNC_TRACE_START();
	B_LOG("%s procs_start %d\n",__func__, procs_start);
_search:
    while((proc_entry = readdir(proc_dir)) != NULL) {
        id = atoi(proc_entry->d_name);
        if ((id != 0) && (id > procs_start) && (id < procs_end)){
            snprintf(procs_com, sizeof(procs_com), "/proc/%d/comm", id);
            fp = fopen(procs_com, "r");
            if (fp) {
                fgets(comm, sizeof(comm), fp);
                fclose(fp);
                fp = NULL;
				if(!strncmp(comm, procs, strlen(procs))) {
					B_LOG("get process %s\n", comm);
					procs_target = id;
					goto _exit;
                }
            }
        }
    }

	if ((procs_start < 0) && (!procs_pid)) { //not found
		closedir(proc_dir);
		return RETFAULT;
	}

	if (!procs_pid) {  //retry
		procs_end = procs_start;
		procs_start -= 200;
		goto _search;
	}

_exit:
	closedir(proc_dir);
	B_FUNC_TRACE_END("%d\n", procs_target);
	return procs_target;
}

int aw_check_process(const char *procs, const char *name)
{
	FILE *fp;
	char comm[128];

	if (!procs | !name)
		return -1;

	B_FUNC_TRACE_START();

	fp = fopen(procs, "r");
	if (fp) {
		fgets(comm, sizeof(comm), fp);
		fclose(fp);
        fp = NULL;
		if(!strncmp(comm, name, strlen(name))) {
			B_LOG("get process %s\n", comm);
			return 0;
		}
	}
	return -1;
}

int aw_inotify_event(const char *path, int file_flag)
{
	char event_buf[1024];
	struct inotify_event *event;
	int errno_t;
	int ifd = 0;
	int wfd = 0;

	if (!path)
		return RETFAULT;

	B_FUNC_TRACE_START();
	B_LOG("%s\n",path);

	ifd = inotify_init();
	wfd = inotify_add_watch(ifd, path, file_flag);
	if (wfd<0) {
		B_LOG("aw_inotify_event:%s, err:%s\n", path, strerror(errno));
		return RETFAULT;
	}
	while (1) {
		int res = read(ifd, event_buf, sizeof(event_buf));
		errno_t = errno;
#if 0
		//test
		int index = 0;
		while (index < res) {
			event = (struct inotify_event *)(event_buf + index);
			index = sizeof(struct inotify_event) + event->len;
			B_LOG("inotify envent: 0x%08x\n", event->mask);
		}
#endif
		if (res < (int)sizeof(*event)) {
			if(errno_t == EINTR)
				continue;
			B_LOG("watch could not read event, %s\n",__func__);
			break;
		}
		inotify_rm_watch(ifd, wfd);
		close(ifd);
		return 0;
	}
	inotify_rm_watch(ifd, wfd);
	close(ifd);
	return -1;
}

int aw_inotify_watch(proc_sw_st *proc)
{
	char event_buf[512];
	struct inotify_event *event;
	int errno_t;
	int ret = -1;
	int wfd;
	int (*fn_call)(int, void*) = NULL;


	if (!proc)
		return RETFAULT;

	if ((!proc->inotify_item)|| (!proc->fn))
		return RETFAULT;

	B_FUNC_TRACE_START();

	int ifd = inotify_init();
	if (ifd < 0) {
		B_LOG("inotify_init: %s\n", strerror(errno));
		goto error_close_ifd;
	}

	if (!aw_sysctrl_access(proc->inotify_key, 0)) {
		wfd = inotify_add_watch(ifd, proc->inotify_key, proc->key_event_mask);
		if (wfd<0) {
			B_LOG("watch %s\n", strerror(errno));
			goto error_close_ifd;
		}
		proc->inotify_flag = INOTIFY_KEY;
	} else {
		wfd = inotify_add_watch(ifd, proc->inotify_item, proc->item_event_mask);
		if (wfd<0) {
			B_LOG("watch %s\n", strerror(errno));
			goto error_close_ifd;
		}
		proc->inotify_flag = INOTIFY_ITEM;
	}
	B_LOG("inotify flag %d\n", proc->inotify_flag);
	while(1) {
		int res = read(ifd, event_buf, sizeof(event_buf));
		errno_t = errno;
		if (res < (int)sizeof(*event)) {
			if(errno_t == EINTR)
				continue;
			B_LOG("watch could not read event, %s\n",__func__);
			break;
		}
		fn_call =  proc->fn;
		ret = fn_call(proc->fn_int_para, proc->fn_ptr_para);
		if (ret==BOOST_THREAD_STOP) {
			B_LOG("%pF ret%d error stop!\n", proc->fn, ret);
			break;
		}
	}

	inotify_rm_watch(ifd, wfd);

error_close_ifd:
	close(ifd);
	B_FUNC_TRACE_END("%d\n", ret);
	return ret;
}

int aw_boost_system_policy(const char *value)
{
	plat_hw_slot hw_slot;
	if (aw_boost_set_policy(value, &hw_slot))
		return -1;
	if (hw_slot.slot_used)
		aw_boostup_set_mode(hw_slot.pid, AW_BOOST_UP_MODE_SYSTEM);
	return 0;
}

int aw_boost_media_policy(const char *value)
{
	plat_hw_slot hw_slot;
	if (aw_boost_set_policy(value, &hw_slot))
		return -1;

	if (hw_slot.slot_used)
		aw_boostup_set_mode(hw_slot.pid, AW_BOOST_UP_MODE_MEDIA);
	return 0;
}

int aw_boost_antutu_policy(const char *value)
{
	plat_hw_slot hw_slot;
	int ret;
	char policy[128];
	int mode;

	if (IS_INVAILD_PARAMS(value))
		return RETFAULT;

	/*
	 *	hw_slot.pid: 		antutu pid
	 *	hw_slot.slot_use:	0: antutu polic disabled; 1: antutu policy enabled
	 */
	memset(&hw_slot, 0, sizeof(hw_slot));
	ret = aw_get_para(value, &hw_slot);
	if (ret < 0)
		goto err_out;

	mode = hw_slot.slot_used;
	switch (mode) {
		case ANTUTU_POLICY_ENABLE:
#ifdef SUN50IW2P1
			snprintf(policy, sizeof(policy), "mode%d:1:c:1:t:0:p:255", hw_slot.pid);
#elif defined SUN50IW6P1
			snprintf(policy, sizeof(policy), "mode%d:1:c:1:t:0:p:255:g:1", hw_slot.pid);
#elif defined SUN8IW7P1
			snprintf(policy, sizeof(policy), "mode%d:1:c:1:t:0:p:100", hw_slot.pid);
#endif
			break;
		case ANTUTU_POLICY_DISABLE:
#ifdef SUN50IW2P1
			snprintf(policy, sizeof(policy), "mode%d:1:c:0:t:1", hw_slot.pid);
#elif defined SUN50IW6P1
			snprintf(policy, sizeof(policy), "mode%d:1:c:0:t:1:g:0", hw_slot.pid);
#elif defined SUN8IW7P1
			snprintf(policy, sizeof(policy), "mode%d:1:c:0:t:1", hw_slot.pid);
#endif
			break;
		default:
			B_LOG("aw_boost_antutu_policy error: no pollicy\n");
			return -1;
	}
	B_LOG("aw_boost_antutu_policy policy=%s\n",policy);

	memset(&hw_slot, 0, sizeof(hw_slot));
	aw_boost_set_policy(policy, &hw_slot);

	return 0;
err_out:
	return -1;
}

int aw_boost_benchmark_policy(const char *value)
{
	plat_hw_slot hw_slot;
	if (aw_boost_set_policy(value, &hw_slot))
		return -1;

	if (hw_slot.slot_used)
		aw_boostup_set_mode(hw_slot.pid, AW_BOOST_UP_MODE_BENCHMARK);
	return 0;
}

int aw_boost_system_init(const char *value)
{
	int init_flag;

	if (IS_INVAILD_PARAMS(value))
		return RETFAULT;

	sscanf(value, "%d", &init_flag);
	if (init_flag == BOOST_ENABLE) {
		aw_platform_init(init_flag);
		aw_init_boostup();
	}
	return 0;
}

/*
 func:  aw_boost_debug_switch
 param: "1" enable boost debug ,"0": close boost debug
 for example: use adh shell or consle to sent conmand
 setprop sys.boost.debug.enable_switch 1 to enable debug
 setprop sys.boost.debug.enable_switch 0 to close debug
*/
int aw_boost_debug_switch(const char *value)
{
	int flag;

	B_LOG("aw_boost_debug_switch\n");
	if (IS_INVAILD_PARAMS(value))
		return RETFAULT;

	sscanf(value, "%d", &flag);
	if (flag <= 2 && flag >= 0)
		BOOST_UP_LOG_LEAVE = flag;

	return 0;
}

int aw_boost_up_perf(const char *name, const char *value)
{
	if (IS_INVAILD_PARAMS(name) || IS_INVAILD_PARAMS(value))
		return RETFAULT;

	B_LOG("setprop name:%s, value:%s\n", name, value);
	if (!IS_MATCH_ITEM(MEDIA_POLICY, name))
		aw_boost_media_policy(value);

	if (!IS_MATCH_ITEM(BENCH_POLICY, name))
		aw_boost_benchmark_policy(value);

	if (!IS_MATCH_ITEM(SYSTEM_POLICY, name))
		aw_boost_system_policy(value);

	if (!IS_MATCH_ITEM(SYSTEM_INIT_SIGNAL, name))
		aw_boost_system_init(value);

	if (!IS_MATCH_ITEM(BENCHMARK_POLICY, name))
		aw_boost_antutu_policy(value);

	if (!IS_MATCH_ITEM(BOOST_DEBUG, name))
		aw_boost_debug_switch(value);

	return 0;
}
