#include <netinet/ether.h>
#include <netinet/in.h>
#include <fa_net.h>

int fa_send_pkt(const u8* ifname, u8 *buf, u16 buflen)
{
	struct sockaddr from;
	u16 len;
	int fd = socket(PF_PACKET, SOCK_PACKET, 0x300);

	if(fd < 0)
		return -1;

	/* build the address */
	mem_set(&from, 0 , sizeof(from));
	strncpy(from.sa_data, ifname, sizeof(from.sa_data));

	/* send the data */
	len = sendto(fd, buf, buflen, 0, &from, sizeof(from));
	close(fd);
	
	return len;
}

int fa_send(struct fa_ctx* ctx)
{
	struct fa_pkt pkt;
	struct ether_header *eth = &pkt.eh;
	struct fa_arphdr *arph = &pkt.arph;

	/* hardcoded for ethernet */
	mem_cpy(eth->ether_dhost, ctx->eth.dest, ETH_ALEN);
	mem_cpy(eth->ether_shost, ctx->eth.source, ETH_ALEN);
	eth->ether_type = htons(ETHERTYPE_ARP);

	/* build the arp payload */
	arph->ar_hrd = htons(ARPHRD_ETHER);
	arph->ar_pro = htons(ETHERTYPE_IP);
	arph->ar_hln = 6;
	arph->ar_pln = 4;
	arph->ar_op = htons(ARPOP_REPLY);
	
	mem_cpy(arph->ar_sha, ctx->eth.source, ETH_ALEN);
	mem_cpy(arph->ar_tha, ctx->eth.dest, ETH_ALEN);
	mem_cpy(arph->ar_sip, &ctx->ip.source, sizeof(arph->ar_sip));
	mem_cpy(arph->ar_tip, &ctx->ip.dest, sizeof(arph->ar_tip));
	
	return fa_send_pkt(ctx->ifname, (u8*)&pkt, sizeof(pkt));
}

static unsigned str_to_ipaddr(const char* s, u32* ip)
{  
	struct in_addr ia;
	int a, b, c, d;
	
	if(sscanf(s, "%d.%d.%d.%d", &a, &b, &c, &d) == 4 &&
		(a >= 0 && a < 256) &&
		(b >= 0 && a < 256) &&
		(c >= 0 && c < 256) &&
		(d >= 0 && d < 256)) {
		if(inet_aton(s, &ia)) {
			*ip = ia.s_addr;
			return 1;
		}
	}
	*ip = 0;
	return 0;
}

static void fa_buf_build(struct fa_buf* buf, u8* s)
{
	u8 i;
	u8* end = s;
	buf->count = 0;
	
	for(i = 0; i < FA_TYPE_MAX; i ++) {
		while(isspace(*s))
			s ++;
		buf->fa_type[i] = s;
		end = s;
		while(!isspace(*end) && *end)
			end ++;
		buf->count ++;
		if(*end)
			*end = 0;
		else
			break;
		s = end+1;
	}
}

int fa_ctx_read(struct fa_ctx* ctx, u8* s)
{
	struct fa_buf buf;
	
	mem_set(ctx, 0, sizeof(*ctx));

	fa_buf_build(&buf, s);
	if(buf.count < FA_TYPE_MAX)
		return -1;
	
	if(strlen(buf.fa_type[FA_IFNAME])+1 > sizeof(ctx->ifname))
		return -1;
	
	strcpy(ctx->ifname, buf.fa_type[FA_IFNAME]);
	if(!ether_aton_r(buf.fa_type[FA_ETH_SRC],
		(struct ether_addr*)ctx->eth.source))
		return -1;
	if(!ether_aton_r(buf.fa_type[FA_ETH_DEST],
		(struct ether_addr*)ctx->eth.dest))
		return -1;
	
	if(!str_to_ipaddr(buf.fa_type[FA_IP_SRC], &ctx->ip.source))
		return -1;
	if(!str_to_ipaddr(buf.fa_type[FA_IP_DEST], &ctx->ip.dest))
		return -1;

	if(buf.fa_type[FA_INTERVAL])
		ctx->interval = atoi(buf.fa_type[FA_INTERVAL]);

	return 0;
}

