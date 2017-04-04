#ifndef FA_MON_H
#define FA_MON_H

#include "fa_subnet.h"

struct fa_mon {
	list_t subnet_list;
	u32 count;
	struct fa_table table;
};

#define fa_mon_init(mon) do { \
	list_init(&(mon)->subnet_list); \
	fa_table_init(&(mon)->table); \
	(mon)->count = 0; \
} while(0)

#define fa_mon_insert_subnet(mon, subnet) do { \
	list_add(&(subnet)->list, &(mon)->subnet_list); \
	(mon)->count ++; \
} while(0)

#define fa_mon_erase_subnet(mon, subnet) do { \
	list_del(&(subnet)->list); \
	(mon)->count --; \
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

