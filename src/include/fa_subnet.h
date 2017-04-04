#ifndef FA_SUBNET_H
#define FA_SUBNET_H

#include "fa_table.h"

/* node id is last four bytes of gateway HW addr */
struct fa_subnet {
	list_t list;
	struct fa_node gateway;
	list_t node_list;
	u32 count;
};

#define fa_subnet_init(subnet) do { \
	fa_node_init(&(subnet)->gateway, 0); \
	list_init(&(subnet)->node_list); \
	(subnet)->count = 0; \
} while(0)

#define fa_subnet_insert_node(subnet, node) do { \
	list_add(&(node)->list, &(subnet)->node_list); \
	(subnet)->count ++; \
} while(0)

#define fa_subnet_erase_node(subnet, node) do { \
	list_del(&(node)->list, &(subnet)->node_list); \
	(subnet)->list --; \
} while(0)

#define fa_subnet_has_gateway(subnet) ((subnet)->gateway.count > 0)

#endif

