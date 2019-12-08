#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <sys/inotify.h>
#include <sys/types.h>
#include <cutils/properties.h>
#include <sys/prctl.h>
#include <dlfcn.h>

static int BOOST_UP_LOG_LEAVE = 0;
#define B_LOG(fmt,...) if (BOOST_UP_LOG_LEAVE>=1) printf(fmt,##__VA_ARGS__)
#define B_INF(fmt,...) if (BOOST_UP_LOG_LEAVE>=2) printf(fmt,##__VA_ARGS__)
#define B_FUNC_TRACE_START() B_LOG("+%s\n", __func__)
#define B_FUNC_TRACE_END(fmt,...) B_LOG("-%s " fmt,__func__, ##__VA_ARGS__)

#define BENCHMARK_POLICY	"benchmark.boost.pref"

#define RETFAULT 		(-1)
#define RETFINE  		(0)

static int BOOSTUP_BOARD;

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

/* AnTuTu CONFIG*/
#define ANTUTU_POLICY_ENABLE		(1)
#define ANTUTU_POLICY_DISABLE		(0)

static proc_sw_st proc_sw[] = {
	{"/data/data/", "/data/data/com.antutu.ABenchMark/", "tutu.ABenchMark",
	 0, IN_CREATE, IN_OPEN, 0, NULL, 0, NULL},
};

int aw_sysctrl_access(const char *path, int dentry)
{
	DIR *dir;
	int fd;

	if (!path)
		return -1;

	B_LOG("aw_sysctrl_access %s\n", path);
	if (dentry) {
		dir = opendir(path);
		B_LOG("aw_sysctrl_access 1\n");
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

	if (!path) {
		B_LOG("err: aw_inotify_event, path is null\n");
		return RETFAULT;
	}

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

int aw_boost_system_factor(char *path, char *name, int pid)
{
	int (*boostup_cpu_entry)(int, int);
	const char *errorInfo;
	void *libm_handle;
	int ret = -1;

	if (!path || !name) {
		B_LOG("aw_boost_system_factor: para is null\n");
		return -1;
	}
	libm_handle = dlopen(path, RTLD_LAZY);
	if (!libm_handle) {
		B_LOG("aw_use_so: libm_handle is null\n");
		return -1;
	}
	boostup_cpu_entry = dlsym(libm_handle, name);
	errorInfo = dlerror();
	if (errorInfo != NULL) {
		B_LOG("aw_use_so: dlsym error: %s\n", name);
		dlclose(libm_handle);
		return -1;
	}
	if (boostup_cpu_entry != NULL)
		ret = boostup_cpu_entry(pid, BOOST_UP_LOG_LEAVE);

	dlclose(libm_handle);
	return ret;
}

int aw_boost_antutu_policy(int mode, int pid)
{
	int ret;
	char policy[128];

	memset(policy, 0, sizeof(policy));

	switch (mode) {
		case ANTUTU_POLICY_ENABLE:
			snprintf(policy, sizeof(policy), "mode%d:1", pid);
			break;
		case ANTUTU_POLICY_DISABLE:
			snprintf(policy, sizeof(policy), "mode%d:0", pid);
			break;
		default:
			return -1;
	}
	ret = property_set(BENCHMARK_POLICY, policy);
	B_LOG("aw_boost_antutu_policy: policy=%s\n", policy);
	return ret;
}

void *aw_antutu_watch(void *ptr)
{
	proc_sw_st *tmp;
	int retry = 2;
	int pid = -1;
	int benchmark_3d_pid = 0;
	int antutu_policy = 0;
	char comm[64];
	char bm3_comm[64];
	char oom_adj[32];

	if (!ptr) {
		B_LOG("aw_antutu_watch para is null\n");
		return NULL;
	}

	tmp = (proc_sw_st *)ptr;

	B_LOG("aw_antutu_watch first\n");
	while (1) {
		/* check antutu dir */
		if (aw_sysctrl_access(tmp->inotify_key, 1)) {
			/* watch /data/app */
			if (aw_inotify_event(tmp->inotify_item, tmp->item_event_mask) < 0)
				return NULL;
			continue;
		}

		B_LOG("aw_antutu_watch second\n");
		snprintf(comm, sizeof(comm), "/proc/%d/comm", tmp->triger_pid);
		while (1) {
			if (aw_check_process(comm, tmp->triger_proc)) {
				/* watch /data/data/com.antutu.ABenchMark */
				if (aw_inotify_event(tmp->inotify_key, tmp->key_event_mask) < 0)
					return NULL;

				/* find com.antutu.ABenchMark pid */
				retry = 2;
				while(retry--) {
					pid = aw_search_process(tmp->triger_proc);
					if (pid < 0)
						sleep(3);
				}
				if (pid < 0) {
					if (antutu_policy) {
						/* antutu is not exist, disable super power mode*/
						aw_boost_antutu_policy(ANTUTU_POLICY_DISABLE, tmp->triger_pid);
						antutu_policy = 0;
					}
					continue;
				}
				tmp->triger_pid = pid;
				snprintf(comm, sizeof(comm), "/proc/%d/comm", tmp->triger_pid);
				snprintf(oom_adj, sizeof(oom_adj), "/proc/%d/oom_adj", tmp->triger_pid);
			}

			/* use other factor */
			if (!aw_boost_system_factor("/system/lib/libbm_core.so", "boostup_cpu_entry", pid)) {
				while (!aw_check_process(comm, tmp->triger_proc)) {
					sleep(5);
				}
				break;
			}

			if (!aw_check_process(oom_adj, "0")) {
				/* antutu will runing, enable super power mode */
				aw_boost_antutu_policy(ANTUTU_POLICY_ENABLE, tmp->triger_pid);
				antutu_policy = 1;
			}

			/* wait antutu exit */
			while (!aw_check_process(comm, tmp->triger_proc)) {
				if (!aw_check_process(oom_adj, "0")) {
					if (!antutu_policy) {
						aw_boost_antutu_policy(ANTUTU_POLICY_ENABLE, tmp->triger_pid);
						antutu_policy = 1;
					}
				} else {
					if (antutu_policy) {
						/* Watch BenchMark 3D */
						if (aw_check_process(bm3_comm, ".benchmark.full")) {
							benchmark_3d_pid = aw_search_process(".benchmark.full");
							if (benchmark_3d_pid < 0){
								aw_boost_antutu_policy(ANTUTU_POLICY_DISABLE, tmp->triger_pid);
								antutu_policy = 0;
							} else {
								snprintf(bm3_comm, sizeof(bm3_comm), "/proc/%d/comm", benchmark_3d_pid);
							}
						} else {
							/* when test BenchMark 3D, continued use super power */
							sleep(10);
						}
					}
				}
				sleep(5);
			}
			if (antutu_policy) {
				/* antutu exit, disable super power mode*/
				aw_boost_antutu_policy(ANTUTU_POLICY_DISABLE, tmp->triger_pid);
				antutu_policy = 0;
			}
		}
	}
	return NULL;
}

int main(int argc, char **argv)
{
	if (argc > 2 && argv[1]) {
		if (!strncmp("-d", argv[1], strlen("-d"))) {
			BOOST_UP_LOG_LEAVE = 2;
		}
	}
	aw_antutu_watch((void *)&proc_sw[0]);
	return 0;
}
