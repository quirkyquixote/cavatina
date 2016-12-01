
#ifndef CAVATINA_H_
#define CAVATINA_H_

#include <string.h>

/*
 * A string: references a slice of a larger text.
 */
struct kv_str {
	const char *begin;	/* Pointer to the first character */
	const char *end;	/* Pointer to the character after the last */
};

/*
 * Node in a singly-linked list
 */
struct kv_node {
	struct kv_node *next;
};

/*
 * A list of kv_node.
 */
struct kv_list {
	struct kv_node *first;
	struct kv_node *last;
};

/*
 * A value.
 */
struct kv_val {
	struct kv_node node;		/* Values are nodes in a list */
	struct kv_str name;		/* String associated with the value */
};

/*
 * A key
 */
struct kv_key {
	struct kv_node node;		/* Keys are nodes in a list */
	struct kv_str name;		/* Name of the key */
	struct kv_list vals;		/* List of values */
};

/*
 * A group of keys
 */
struct kv_group {
	struct kv_node node;		/* Groups nodes in a list */
	struct kv_str name;		/* Name of the group */
	struct kv_list keys;		/* List of keys */
};

/*
 * Data from parsing one or more strings
 */
struct kv_parser {
	struct kv_list groups;		/* All groups */
	struct kv_group *cur_group;	/* Group currently being parsed */
	struct kv_key *cur_key;		/* Key currently being parsed */
	struct kv_val *cur_val;		/* Value currently being parsed */
};

/*
 * Initialize parser
 */
struct kv_parser *kv_new(void);
/*
 * Destroy parser
 */
void kv_destroy(struct kv_parser *parser);
/*
 * Parse string and generate groups, keys, and values
 */
int kv_parse(struct kv_parser *parser, const char *buf);
/*
 * Create kv_str from C string
 */
static inline struct kv_str kv_str(const char *str)
{
	struct kv_str s;
	s.begin = str;
	s.end = str + strlen(str);
	return s;
}
/*
 * Compare strings
 */
int kv_strcmp(struct kv_str s1, struct kv_str s2);
/*
 * Get values for the given group and key
 */
struct kv_key *kv_get(struct kv_parser *parser, struct kv_str group, struct kv_str key);
/*
 * Add value for the given group and key
 */
int kv_add(struct kv_parser *parser, struct kv_str group, struct kv_str key, struct kv_str val);
/*
 * Remove key from group
 */
int kv_remove(struct kv_parser *parser, struct kv_str group, struct kv_str key);
/*
 * Get group named name, or NULL
 */
struct kv_group *kv_get_group(struct kv_parser *parser, struct kv_str name);
/*
 * Get group named name; create it if it doesn't exist
 */
struct kv_group *kv_add_group(struct kv_parser *parser, struct kv_str name);
/*
 * Remove group.
 */
int kv_remove_group(struct kv_parser *parser, struct kv_group *group);
/*
 * Remove group by name.
 */
int kv_remove_group_by_name(struct kv_parser *parser, struct kv_str name);
/*
 * Get key named name, or NULL
 */
struct kv_key *kv_get_key(struct kv_group *group, struct kv_str name);
/*
 * Get key named name; create it if it doesn't exist
 */
struct kv_key *kv_add_key(struct kv_group *group, struct kv_str name);
/*
 * Remove key.
 */
int kv_remove_key(struct kv_group *group, struct kv_key *key);
/*
 * Remove key by name.
 */
int kv_remove_key_by_name(struct kv_group *group, struct kv_str name);
/*
 * Get value named name, or NULL
 */
struct kv_val *kv_get_val(struct kv_key *key, struct kv_str name);
/*
 * Get value named name; create it if it doesn't exist
 */
struct kv_val *kv_add_val(struct kv_key *key, struct kv_str name);
/*
 * Remove val.
 */
int kv_remove_val(struct kv_key *key, struct kv_val *val);
/*
 * Remove val by name.
 */
int kv_remove_val_by_name(struct kv_key *key, struct kv_str name);
/*
 * Dump all parsed data to stdout
 */
void kv_dump(struct kv_parser *parser);
/*
 * Initialize list
 */
void kv_list_init(struct kv_list *list);
/* 
 * Append node to list
 */
void kv_list_add(struct kv_list *list, struct kv_node *node);
/*
 * Remove node from list
 */
int kv_list_remove(struct kv_list *list, struct kv_node *node);
/*
 * Iterate on list
 */
#define kv_foreach(iter,list)\
	for ((iter) = (list)->first; (iter) != NULL; (iter) = (iter)->next)
/*
 * Iterate on groups
 */
#define kv_foreach_group(iter,parser)\
	for ((iter) = (void *)(parser)->groups.first; (iter) != NULL;\
			(iter) = (void *)(iter)->node.next)
/*
 * Iterate on keys
 */
#define kv_foreach_key(iter,parser)\
	for ((iter) = (void *)(parser)->keys.first; (iter) != NULL;\
			(iter) = (void *)(iter)->node.next)
/*
 * Iterate on values
 */
#define kv_foreach_val(iter,parser)\
	for ((iter) = (void *)(parser)->vals.first; (iter) != NULL;\
			(iter) = (void *)(iter)->node.next)

#endif
