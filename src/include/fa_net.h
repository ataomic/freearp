#ifndef FA_NET_H
#define FA_NET_H

#include <net/ethernet.h>
#include <net/if_arp.h>

#include "fa_sys.h"

struct fa_arphdr
  {
    u16 ar_hrd;		/* Format of hardware address.  */
    u16 ar_pro;		/* Format of protocol address.  */
    u8 ar_hln;		/* Length of hardware address.  */
    u8 ar_pln;		/* Length of protocol address.  */
    u16 ar_op;		/* ARP opcode (command).  */
    /* Ethernet looks like this : This bit is variable sized
       however...  */
    u8 ar_sha[ETH_ALEN];	/* Sender hardware address.  */
    u8 ar_sip[4];		/* Sender IP address.  */
    u8 ar_tha[ETH_ALEN];	/* Target hardware address.  */
    u8 ar_tip[4];		/* Target IP address.  */
};

struct fa_pkt {
	struct ether_header eh;
	struct fa_arphdr arph;
};

enum {
	FA_IFNAME = 0,
	FA_ETH_SRC,
	FA_ETH_DEST,
	FA_IP_SRC,
	FA_IP_DEST,
	FA_INTERVAL,
	FA_TYPE_MAX,
};

struct fa_buf {
	u8* fa_type[FA_TYPE_MAX];
	u32 count:4;
	u32 unused:28;
};

struct fa_ctx {
	struct {
		u8 source[ETH_ALEN];
		u8 dest[ETH_ALEN];
	}eth;
	struct {
		u32 source;
		u32 dest;
	}ip;
	u8 ifname[14];
	u8 interval;
	u8 unused;
};

#ifdef __cplusplus
extern "C" {
#endif

int fa_send_pkt(const u8* ifname, u8 *buf, u16 buflen);
int fa_build_arp(struct fa_ctx* ctx);
int fa_ctx_read(struct fa_ctx* ctx, u8* s);

#ifdef __cplusplus
}
#endif

#endif

