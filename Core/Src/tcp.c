/*
 * tcp.c
 *
 *  Created on: May 2, 2024
 *      Author: silvermight
 */
#include "tcp.h"

#include "lwip.h"
#include "lwip/api.h"
#include "lwip/tcp.h"
#include "memp.h"
#include <string.h>

// Forward declarations
static err_t accept_callback(void *arg, struct tcp_pcb *newpcb, err_t err);
static err_t tcp_server_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p,
                             err_t err);

void tcpserver_init() {
  struct tcp_pcb *tpcb;

  tpcb = tcp_new();
  if (!tpcb) {
    return;
  }


  ip_addr_t ipaddr;
  // WANDA IP
  IP4_ADDR(&ipaddr, 192, 168, 1, 10);

  
  if (tcp_bind(tpcb, IP_ADDR_ANY, 35912) != ERR_OK) {
    memp_free(MEMP_TCP_PCB, tpcb);
    return;
  }


  tcp_arg(tpcb, NULL);

  tpcb = tcp_listen(tpcb);
  if(!tpcb) {
    return;
  }


  tcp_accept(tpcb, accept_callback);
}


static err_t accept_callback(void *arg, struct tcp_pcb *newpcb, err_t err) {
  LWIP_UNUSED_ARG(arg);
  LWIP_UNUSED_ARG(err);

  tcp_recv(newpcb, tcp_server_recv);
  tcp_arg(newpcb, NULL);


  return ERR_OK;
}

static err_t tcp_server_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p,
                             err_t err) {

  LWIP_UNUSED_ARG(arg);

  if (!p) {
    tcp_close(tpcb);
    tcp_recv(tpcb, NULL);
    return ERR_OK;
  }

  tcp_recved(tpcb, p->tot_len);


  /* reinterpret as an integer
     * REQUIRES LITTLE ENDIAN, or htonl()
  */
  uint32_t command;
  memcpy(&command, p->payload, sizeof(uint32_t));

  int reply = (int) command + 1;
  /* reserialize */

  char newbuf[sizeof(uint32_t)];
  memcpy(newbuf, &reply, sizeof(uint32_t));
  err = tcp_write(tpcb, newbuf, sizeof(uint32_t), 1);

  /* free the received pbuf */
  pbuf_free(p);

  return ERR_OK;
}
