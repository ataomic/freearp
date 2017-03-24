#ifndef FA_MON_H
#define FA_MON_H

#include "fa_table.h"

/* node id is last four bytes of gateway HW addr */
struct fa_monitor {
	struct fa_node gateway;
	list_t node_list;
	u32 count;
};

#define fa_monitor_init(monitor) do { \
	fa_node_init(&(monitor)->gateway, 0); \
	list_init(&(monitor)->node_list); \
	(monitor)->count = 0; \
} while(0)

#define fa_monitor_gateway_exists(monitor) ((monitor)->gateway.count > 0)

struct fa_mon {
	list_t monitor_list;
	u32 count;
	struct fa_table table;
};

#define fa_mon_init(mon) do { \
	list_init(&(mon)->monitor_list); \
	fa_table_init(&(mon)->table); \
	(mon)->count = 0; \
} while(0)

extern struct fa_mon g_fa_mon;

#ifdef __cplusplus
extern "C" {
#endif

void fa_mon_ginit();

#ifdef __cplusplus
}
#endif

#endif

