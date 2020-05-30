
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>

#include <linux/can.h>
#include <linux/can/raw.h>

int main(int argc, char **argv)
{
	int s, i; 
	int nbytes;
	struct sockaddr_can addr;
	struct ifreq ifr;
	struct can_frame frame;

	unsigned long AA;
	unsigned long BB;
	unsigned int Battery_Voltage;
	int16_t Battery_Current;
	uint16_t CC;
	int16_t DD;
	
	printf("ChargePort Packet Decoder\r\n");

	if ((s = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) {
		perror("Socket");
		return 1;
	}

	strcpy(ifr.ifr_name, "can0" );
	ioctl(s, SIOCGIFINDEX, &ifr);

	memset(&addr, 0, sizeof(addr));
	addr.can_family = AF_CAN;
	addr.can_ifindex = ifr.ifr_ifindex;

	if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
		perror("Bind");
		return 1;
	}

	do {
	
		nbytes = read(s, &frame, sizeof(struct can_frame));
	
		if (nbytes < 0) {
			perror("Read");
			return 1;
		}
	
		//printf("0x%03X [%d] ",frame.can_id, frame.can_dlc);
		
		switch (frame.can_id) {
		
			case 0x102:
				// Always 7 bytes of zero, ignore.
				if (frame.can_dlc != 7 |
					frame.data[0] != 0 |
					frame.data[1] != 0 |
					frame.data[2] != 0 |
					frame.data[3] != 0 |
					frame.data[4] != 0 |
					frame.data[5] != 0 |
					frame.data[6] != 0 ) {
						printf("Unexpected data in can_id 0x%03X:",frame.can_id);
						for (i = 0; i < frame.can_dlc; i++)
							printf("%02X ",frame.data[i]);
						printf("\r\n");
				}
				break;
				
			case 0x112:
				// Assume low level BMS information; requires further decoding.
#if 0
				printf("0x%03X: ",frame.can_id);
				CC = ((frame.data[1] << 8) | frame.data[0]);
				printf("CC = %d\n",CC);
#endif
				break;
				
			case 0x123:
				// Assume low level BMS information; requires further decoding.
#if 0
				printf("0x%03X: ",frame.can_id);
				if (frame.data[4] != 0x38) printf("[Byte 4 = 0%X02] ",frame.data[4]);
				CC = ((frame.data[3] << 8) | frame.data[2]);
				printf("CC = %d\n",CC);
#endif 
				break;
				
			case 0x202:
				// Assume low level BMS information; requires further decoding.
				// DD appears to be a signed battery current. Current reverses when 
				// load such as air-conditioning is switched on.
#if 0
				printf("0x%03X: ",frame.can_id);
				printf("Byte 2 = 0x%02X Byte 5 = 0x%02X, ",(frame.data[1]), (frame.data[4]));

				printf("AA = %d ",frame.data[0]);

				DD = ((frame.data[3] << 8) | frame.data[2]);
				printf("DD = %d ",DD);

				printf("\r\n");
#endif
				break;
				
			case 0x213:
				// Assume low level BMS information; requires further decoding.
#if 0
				printf("0x%03X: ",frame.can_id);
				CC = ((frame.data[1] << 8) | frame.data[0]);
				printf("CC = %d ",CC);
				printf("AA = %d \r\n", frame.data[4]);
#endif
				break;
				
			case 0x214:
				// Always 00 00 00 60 00 00 00 00, ignore.
				if (frame.can_dlc != 8 |
					frame.data[0] != 0 |
					frame.data[1] != 0 |
					frame.data[2] != 0 |
					frame.data[3] != 0x60 |
					frame.data[4] != 0 |
					frame.data[5] != 0 |
					frame.data[6] != 0 | 
					frame.data[7] != 0) {
						printf("Unexpected data in can_id 0x%03X:",frame.can_id);
						for (i = 0; i < frame.can_dlc; i++)
							printf("%02X ",frame.data[i]);
						printf("\r\n");
				}
				break;
				
			case 0x235:
				// Appears to contain HV battery voltage and current.
				// Current goes negative when loads such as air conditioning is switched on.
				printf("0x%03X: ",frame.can_id);
				if (frame.data[4] != 0x3F) printf("[Byte 4 = 0%X02] ",frame.data[4]);
				if (frame.data[5] != 0x00) printf("[Byte 5 = 0%X02] ",frame.data[5]);
				if (frame.data[6] != 0x00) printf("[Byte 6 = 0%X02] ",frame.data[6]);
				if (frame.data[7] != 0x00) printf("[Byte 7 = 0%X02] ",frame.data[7]);
				Battery_Voltage = ((frame.data[1] << 8) | frame.data[0]);
				Battery_Current = ((frame.data[3] << 8) | frame.data[2]);
				printf("HV Battery %.1fV Current %.1fA\r\n",(float)Battery_Voltage/10,(float)Battery_Current/10);
				break;
				
			case 0x302:
				// Includes State of Charge (SoC) and some flags
#if 1
				printf("0x%03X: ",frame.can_id);
				printf("SoC = %.1f%% ", ((float)(frame.data[2] * 100)/255));
				switch (frame.data[3]) {
					case 0x44:
						printf("[Charging]\r\n");
						break;
					case 0x40:
						printf("[Not Charging]\r\n");
						break;
					default:
						printf("Unknown flags %02X\r\n",frame.data[3]);
						break;
				}
#endif			
				if (frame.can_dlc != 7 |
					frame.data[0] != 0x00 |
					frame.data[1] != 0x00 |
					frame.data[4] != 0x09 |
					frame.data[5] != 0x00 |
					frame.data[6] != 0x00 | 
					frame.data[7] != 0x00) {
						printf("Unexpected data in can_id 0x%03X:",frame.can_id);
						for (i = 0; i < frame.can_dlc; i++)
							printf("%02X ",frame.data[i]);
						printf("\r\n");
				}
				break;
				
			case 0x30C:
				// Single byte frame from car to enable EVSE power (relay) 
				if (frame.can_dlc == 1) {
					//printf("0x%03X: ",frame.can_id);
					switch (frame.data[0]) {
						case 0x00: 
							//printf("EVSE power disabled\r\n");
							break;
						case 0x01: 
							//printf("EVSE power enabled\r\n");
							break;
						default:
							printf("Unknown command in can_id 0x%03X:",frame.can_id);
					}
				} else {
					printf("Unexpected data in can_id 0x%03X:",frame.can_id);
					for (i = 0; i < frame.can_dlc; i++)
						printf("%02X ",frame.data[i]);
					printf("\r\n");						
				}
				break;
				
			case 0x31C:
				// Frame from TWC advertising current capacity (and voltage).
#if 1
				printf("0x%03X: ",frame.can_id);
				printf("TWC Advertising %2.1fA @ %3dV\r\n", (float)frame.data[0]/2, frame.data[2]);
#endif
				if (frame.can_dlc != 8 |
					frame.data[1] != 0x00 |
					frame.data[3] != 0x20 |
					frame.data[4] != 0x00 |
					frame.data[5] != 0x00 |
					frame.data[6] != 0x00 | 
					frame.data[7] != 0x00) {
						printf("Unexpected data in can_id 0x%03X:",frame.can_id);
						for (i = 0; i < frame.can_dlc; i++)
							printf("%02X ",frame.data[i]);
						printf("\r\n");
					}
				break;
				
			case 0x322:
				// Cycles VIN number, ignore for now.
				break;
				
			case 0x32C:
				// Cycles two blocks of unknown data, ignore for now.
				// Appears to originate from TWC.
				break;
				
			case 0x3D2:
#if 0
				// Appears to be two 32 bit numbers sent from car.
				// AA increments smoothly when charging. 
				// BB increments more slowly when not charging.
				printf("0x%03X: ",frame.can_id);
				
				AA = ((frame.data[3] << 24) | 
					  (frame.data[2] << 16) |
					  (frame.data[1] << 8)  |
					   frame.data[0] );
				
				BB = ((frame.data[7] << 24) | 
					  (frame.data[6] << 16) |
					  (frame.data[5] << 8)  |
					   frame.data[4] );
				
				printf("AA = %8ld BB = %8ld\r\n",AA,BB);
#endif
				break;
				
			case 0x45C:
				// Appears to be acknowledgment from car to 0x46C supend charging.
				// Only seen once on the first 0x46C:00 00 10 00 00 00 00 00 packet.
				printf("0x%03X: Ack?\r\n",frame.can_id);
				break;
				
			case 0x46C:
				// Normally sent every 1 second from TWC and contains payload of all zeros.
				// Can be used to suspend charging, i.e. when loss of heartbeat
				// communications between load sharing TWCs by sending 0x10 as 3nd byte.
				switch (frame.data[2]) {
					case 0x00: 
						//printf("0x%03X: Normal\r\n", frame.can_id);
						break;
					case 0x10: 
						printf("0x%03X: Suspend charging\r\n",frame.can_id);
						break;
					default:
						printf("Unknown flag 0x02X%",frame.data[2]);
						break;
				}
				
				if (frame.can_dlc != 8 |
					frame.data[0] != 0 |
					frame.data[1] != 0 |
					frame.data[3] != 0 |
					frame.data[4] != 0 |
					frame.data[5] != 0 |
					frame.data[6] != 0 | 
					frame.data[7] != 0) {
					printf("Unexpected data in can_id 0x%03X:",frame.can_id);
					for (i = 0; i < frame.can_dlc; i++)
						printf("%02X ",frame.data[i]);
					printf("\r\n");
					}
				break;
				
			case 0x504:
				// Cycles TWC version and model, ignore for now.
				break;
				
			case 0x514:
				// Cycles TWC serial number, ignore for now.
				break;
				
			case 0x54C:
				// Always 04 00 00 00 00 00 00 00, ignore.
				// Appears to be sent from car
				if (frame.can_dlc != 8 |
					frame.data[0] != 0x04 |
					frame.data[1] != 0 |
					frame.data[2] != 0 |
					frame.data[3] != 0 |
					frame.data[4] != 0 |
					frame.data[5] != 0 |
					frame.data[6] != 0 | 
					frame.data[7] != 0) {
						printf("Unexpected data in can_id 0x%03X:",frame.can_id);
						for (i = 0; i < frame.can_dlc; i++)
							printf("%02X ",frame.data[i]);
						printf("\r\n");
				}
				break;
				
			default:
				// Catch all for unknown packets
				printf("Unknown frame can_id 0x%03X:",frame.can_id);
				for (i = 0; i < frame.can_dlc; i++)
					printf("%02X ",frame.data[i]);
				printf("\r\n");
		}
	
	} while (1);
	
	if (close(s) < 0) {
		perror("Close");
		return 1;
	}

	return 0;
}
