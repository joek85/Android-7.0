
#include <errno.h>
#include <cutils/log.h>
#include "displaydRequest.h"

displaydRequest *requestHandler = 0;
int displaydRequest_init(void *perform_pfn)
{
	hwcomposerInterface *interface = new hwcomposerInterface();
	interface->mHWCPerform = reinterpret_cast<HWC_PERFORM>(perform_pfn);

	requestHandler = new displaydRequest(interface);
	if (requestHandler->startListener()) {
		ALOGE("unable to start displayd listen (%s)", strerror(errno));
		return -1;
	}
	return 0;
}
