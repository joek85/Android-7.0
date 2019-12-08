/*
Copyright (C) 2013 Christoph Thoma (thoma.christoph@gmx.at)

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include "../Si4703_Breakout.h"
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
int main(int argc, char *argv[])
{
	extern char *optarg;
        extern int optind, optopt;
	int c;
        int resetPin = 23; //GPIO_23
        int sdaPin = 0; //GPIO_0 (SDA)
	int chann=0;
	int vol=1;
	while ((c = getopt(argc, argv, "v:r:c:p:n:d:q:m:t:f:F:C:Q:s:")) != -1) {
	switch(c) {
		    case 'c':
			chann = atoi(optarg);
			break;
		    case 'v':
			vol = atoi(optarg);
			break;
		}
    	}

        Si4703_Breakout radio;
        radio.powerOn();
        radio.setVolume(vol);
        radio.setChannel(chann); //FM4 104.0Mhz (Austria)

        char rdsBuffer[100] = {0};
        radio.readRDS(rdsBuffer, 1000); //timeout 15sec
        printf("Listening to station: %s %.1f\n", rdsBuffer, radio.getChannel()/10.0);

        //radio.printRegisters();

        return 0;
}
