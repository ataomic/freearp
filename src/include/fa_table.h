#ifndef FA_TABLE_H
#define FA_TABLE_H

#define FA_BUCKET_MAX 0x3fful
#define FA_ADDR_MAX 8

enum {
	FA_ADDR_NONE = 0,
	FA_ADDR_HW,
	FA_ADDR_IP,
	FA_ADDR_HOST,
	FA_ADDR_T_MAX,
};

typedef struct fa_node fa_node_t;

struct fa_addr {
	list_t list;
	u32 type:8;
	u32 unused:24;
	fa_node_t* owner;
	union {
		u8 hw[ETH_ALEN];
		u32 ip;
		u8* host;
	} u;
};

#define fa_addr_init(addr) do { \
	list_init(&(addr)->list); \
} while(0)

struct fa_node {
	struct fa_addr addr[FA_ADDR_MAX];
	u32 id;
	u32 count:8;
	u32 unused:24;
};

#define fa_node_init(node, nid) do { \
	int _ct_i; \
	for(_ct_i = 0; _ct_i < FA_ADDR_MAX; i ++) \
		fa_addr_init((node)->addr+_ct_i); \
	(node)->count = 0; \
} while(0)

struct fa_table {
	list_t buckets[FA_BUCKET_MAX];
	u32 count;
};

#define fa_table_size(table) \
	(sizeof((table)->buckets)/sizeof((table)->buckets[0]))

#define fa_table_init(table) do { \
	int _ct_i; \
	for(_ct_i = 0; _ct_i < fa_table_size(table); _ct_i ++) \
		list_init((table)->buckets+i); \
	(table)->count = 0; \
} while(0)

#define fa_addr_equals(a, b) (fa_addr_cmp(a, b) == 0)

#ifdef __cplusplus
extern "C" {
#endif

__inline u32 fa_addr_hcode(const struct fa_addr* addr, u32 id);
int fa_addr_cmp(const struct fa_addr* a, const struct fa_addr* b);
void fa_addr_copy(struct fa_addr* a, const struct fa_addr* b);
void fa_addr_cleanup(struct fa_addr* a);
void fa_addr_replace(struct fa_addr* a, struct fa_addr* b);

struct fa_addr* fa_table_find_addr(struct fa_table* table, u32 id,
	const struct fa_addr* addr);
void fa_table_insert_addr(struct fa_table* table, u32 id,
	struct fa_addr* addr);
void fa_table_erase_addr(struct fa_table* table, u32 id,
	struct fa_addr* addr);

struct fa_node* fa_table_find_node(struct fa_table* table, u32 id,
	const struct fa_addr* addr);
int fa_table_insert_node(struct fa_table* table, struct fa_node* node);
void fa_table_erase_node(struct fa_table* table, struct fa_node* node);

int fa_node_add_addr(struct fa_node* node, const struct fa_addr* addr);
void fa_node_del_addr(struct fa_node* node, const struct fa_addr* addr);

#ifdef __cplusplus
}
#endif

#endif

