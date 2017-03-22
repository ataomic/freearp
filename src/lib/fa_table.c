#include <fa_table.h>

__inline u32 fa_addr_hcode(const struct fa_addr* addr, u32 id)
{
	return 0;
}

int fa_addr_cmp(const struct fa_addr* a, const struct fa_addr* b)
{
	int ret = fa_int_cmp(a->type, b->type);
	if(ret != 0)
		return ret;
	switch(a->type) {
		case FA_ADDR_HW:
		case FA_ADDR_IP:
		case FA_ADDR_HOST:
			break;
	}

	return -1;
}

void fa_addr_copy(struct fa_addr* a, const struct fa_addr* b)
{
}

void fa_addr_cleanup(struct fa_addr* a)
{
}

void fa_addr_replace(struct fa_addr* a, struct fa_addr* b)
{
}

struct fa_addr* fa_table_find_addr(struct fa_table* table, u32 id,
	const struct fa_addr* addr)
{
	fa_addr* fa;
	u32 hindex = fa_addr_hcode(addr, id) % fa_table_size(table);
	list_t *bucket = &table->buckets[hindex], *pos;

	list_for_each(pos, bucket) {
		fa = fa_addr_entry(pos);
		if(fa_addr_equals(fa, addr))
			return fa;
	}

	return NULL;
}

void fa_table_insert_addr(struct fa_table* table, u32 id,
	struct fa_addr* addr)
{
	u32 hindex = fa_addr_hcode(addr, id) % fa_table_size(table);
	list_t* bucket = &table->buckets[hindex];
	
	list_add(&addr->list, bucket);
	table->count ++;
}

void fa_table_erase_addr(struct fa_table* table, u32 id,
	struct fa_addr* addr)
{
	list_del_init(&addr->list);
	table->count --;
}

int fa_table_insert_node(struct fa_table* table, struct fa_node* node)
{
	int i;

	for(i = 0; i < node->count; i ++) {
		addr = node->addr+i;
		if(!list_empty(&addr->list) &&
			fa_table_find_node(table, node->id, addr) != node) {
			return FA_DUP;
		}
	}

	for(i = 0; i < node->count; i ++) {
		if(!list_empty(&addr->list))
			fa_table_insert_addr(table, node->id, addr);
	}

	return FA_OK;
}

void fa_table_erase_node(struct fa_table* table, struct fa_node* node)
{
	int i;

	for(i = 0; i < node->count; i ++) {
		addr = node->addr+i;
		if(!list_empty(&addr->list))
			fa_table_erase_addr(table, addr);
	}
}

int fa_node_add_addr(struct fa_node* node, const struct fa_addr* addr)
{
	int i;

	for(i = 0; i < node->count; i ++) {
		if(fa_addr_equals(node->addr+i, addr))
			return FA_DUP;
	}
	if(node->count+1 > FA_ADDR_MAX)
		return FA_FAIL;
	
	fa_addr_copy(node->addr+node->count, addr);
	node->count ++;
	
	return FA_OK;
}

void fa_node_del_addr(struct fa_node* node, const struct fa_addr* addr)
{
	struct fa_addr* fa;

	for(i = 0; i < node->count; i ++) {
		fa = node->addr+i;
		if(fa_addr_equals(fa, addr)) {
			fa_addr_cleanup(fa);
			if(i < node->count-1) {
				fa_addr_copy(fa, node->addr+node->count-1);
				fa_addr_replace(fa, node->addr+node->count-1);
			}
			node->count --;
			break;
		}
	}
}

