
#include <stdlib.h>
#include <unistd.h>
#include "display/policySelector.h"

int start_displayinit_thread(void)
{
	int pid = fork();

	/* parent process */
	if(pid > 0 || pid < 0)
		return pid > 0 ? 0 : -1;

	/* child process */
	/* open external display and copy bootlogo */
	policySelector *display = new policySelector(new DisplayManager());
	display->setup(display->getPolicy());
	delete display;

	exit(0);
	return 0;
}


