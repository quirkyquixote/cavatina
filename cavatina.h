
#ifndef CAVATINA_H_
#define CAVATINA_H_

/*
 * A string: references a slice of a larger text.
 */
struct kv_str {
	const char *begin;	/* Pointer to the first character */
	const char *end;	/* Pointer to the character after the last */
};

/*
 * A value.
 */
struct kv_val {
	struct kv_val *next;		/* Next value for that key */
	struct kv_str name;		/* String associated with the value */
};

/*
 * A key
 */
struct kv_key {
	struct kv_key *next;		/* Next key for that group */
	struct kv_str name;		/* Name of the key */
	struct kv_val *first_val;	/* First value */
	struct kv_val *last_val;	/* Last value */
};

/*
 * A group of keys
 */
struct kv_group {
	struct kv_group *next;		/* Next group */
	struct kv_str name;		/* Name of the group */
	struct kv_key *first_key;	/* First key */
	struct kv_key *last_key;	/* Last key */
};

/*
 * Data from parsing one or more strings
 */
struct kv_parser {
	struct kv_group *first_group;	/* First group */
	struct kv_group *last_group;	/* Last group */
	struct kv_group *cur_group;	/* Group currently being parsed */
	struct kv_key *cur_key;		/* Key currently being parsed */
	struct kv_val *cur_val;		/* Value currently being parsed */
};

/*
 * Initialize parser
 */
void kv_init(struct kv_parser *parser);
/*
 * Destroy parser
 */
void kv_destroy(struct kv_parser *parser);
/*
 * Parse string and generate groups, keys, and values
 */
int kv_parse(struct kv_parser *parser, const char *buf);
/*
 * Compare strings
 */
int kv_strcmp(struct kv_str *s1, struct kv_str *s2);
/*
 * Get group named name, or NULL
 */
struct kv_group *kv_get_group(struct kv_parser *parser, struct kv_str *name);
/*
 * Get group named name; create it if it doesn't exist
 */
struct kv_group *kv_add_group(struct kv_parser *parser, struct kv_str *name);
/*
 * Get key named name, or NULL
 */
struct kv_key *kv_get_key(struct kv_group *group, struct kv_str *name);
/*
 * Get key named name; create it if it doesn't exist
 */
struct kv_key *kv_add_key(struct kv_group *group, struct kv_str *name);
/*
 * Get value named name, or NULL
 */
struct kv_val *kv_get_val(struct kv_key *key, struct kv_str *name);
/*
 * Get value named name; create it if it doesn't exist
 */
struct kv_val *kv_add_val(struct kv_key *key, struct kv_str *name);
/*
 * Dump all parsed data to stdout
 */
int kv_dump(struct kv_parser *parser);

#endif
