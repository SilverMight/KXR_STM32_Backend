/*
 * udp.c
 *
 *  Created on: May 3, 2024
 *      Author: silvermight
 */

#include "udp.h"

#include <string.h>
#include <stdio.h>
#include "FreeRTOS.h"
#include "cmsis_os.h"
#include "lwip/udp.h"
#include "lwip/ip4_addr.h"
#include "lwip/api.h"


static struct udp_pcb *upcb;
static int idx = 0;
// Forward declarations
static void udpsend_thread();
int send_data(void * data, int data_size);

static char smsg[100];

void udp_client_init() {
	err_t err;


	upcb = udp_new(); 
	if (!upcb) {
		return;
	}

	ip_addr_t cosmoIp;
	// NOTE: CHANGE IP TO BE IP OF COSMO
	IP_ADDR4(&cosmoIp, 192, 168, 1, 90);



	err = udp_bind(upcb, IP_ADDR_ANY, 70);  // Bind the connection to port
	
	err = udp_connect(upcb, &cosmoIp, 4000);  // Connect to the client
	if(err != ERR_OK) {
		return;
	}


	// Create a new thread to send data
	// check if the thread is created successfully
	if (sys_thread_new("udpsend_thread", udpsend_thread, NULL, DEFAULT_THREAD_STACKSIZE, osPriorityNormal) == NULL) {
		return;
	}
}

static void udpsend_thread() {
	for (;;) {
		sprintf(smsg, "index value = %d\n", idx++);
		SCB_CleanDCache_by_Addr((uint32_t*)(((uint32_t)smsg) & ~(uint32_t)0x1F), sizeof(smsg)+32);
		send_data(smsg, strlen(smsg));
		osDelay(100);
	}
}

int send_data(void * data, int data_size) {
	err_t err;
	struct pbuf *data_packet; // packet we send over the network

	data_packet = pbuf_alloc(PBUF_TRANSPORT, data_size, PBUF_RAM);

	if(data_packet == NULL) {
		return -1;
	}

	if(pbuf_take(data_packet, data, data_size) != ERR_OK) {
		return -1;
	}

	err = udp_send(upcb, data_packet);
	if (err != ERR_OK) {
		return -1;
	}

	pbuf_free(data_packet);
	return 0;
}
