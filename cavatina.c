
#include "cavatina.h"

#include <stdio.h>
#include <stdlib.h>

static void kv_destroy_group(struct kv_group *group);
static void kv_destroy_key(struct kv_key *key);
static void kv_destroy_val(struct kv_val *val);

static int kv_parse_group(struct kv_parser *parser, const char **buf);
static int kv_parse_comment(struct kv_parser *parser, const char **buf);
static int kv_parse_key(struct kv_parser *parser, const char **buf);
static int kv_parse_val(struct kv_parser *parser, const char **buf);

static void kv_dump_str(struct kv_str str);

struct kv_parser *kv_new(void)
{
	return calloc(1, sizeof(struct kv_parser));
}

void kv_destroy(struct kv_parser *parser)
{
	struct kv_node *iter, *next;
	iter = parser->groups.first;
	while (iter != NULL) {
		next = iter->next;
		kv_destroy_group((void *)iter);
		iter = next;
	}
	free(parser);
}

void kv_destroy_group(struct kv_group *group)
{
	struct kv_node *iter, *next;
	iter = group->keys.first;
	while (iter != NULL) {
		next = iter->next;
		kv_destroy_key((void *)iter);
		iter = next;
	}
	free(group);
}

void kv_destroy_key(struct kv_key *key)
{
	struct kv_node *iter, *next;
	iter = key->vals.first;
	while (iter != NULL) {
		next = iter->next;
		kv_destroy_val((void *)iter);
		iter = next;
	}
	free(key);
}

void kv_destroy_val(struct kv_val *val)
{
	free(val);
}

int kv_parse(struct kv_parser *parser, const char *buf)
{
	struct kv_str group;

	for (;;) {
		while (*buf == ' ' || *buf == '\t')
			++buf;
		if (*buf == 0) {
			return 0;
		} else if (*buf == '[') {
			if (kv_parse_group(parser, &buf) != 0)
				return -1;
		} else if (*buf == '#' || *buf == '\n') {
			if (kv_parse_comment(parser, &buf) != 0)
				return -1;
		} else {
			if (kv_parse_key(parser, &buf) != 0)
				return -1;
			if (kv_parse_val(parser, &buf) != 0)
				return -1;
		}
	}
}

int kv_strcmp(struct kv_str s1, struct kv_str s2)
{
	int l1, l2;
	l1 = s1.end - s1.begin;
	l2 = s2.end - s2.begin;
	if (l2 < l1)
		l1 = l2;
	return strncmp(s1.begin, s2.begin, l1);
}

struct kv_group *kv_get_group(struct kv_parser *parser, struct kv_str name)
{
	struct kv_node *iter;
	struct kv_group *group;
	kv_list_foreach(iter, &parser->groups) {
		group = (void *)iter;
		if (kv_strcmp(group->name, name) == 0)
			return group;
	}
	return NULL;
}

struct kv_group *kv_add_group(struct kv_parser *parser, struct kv_str name)
{
	struct kv_group *group;
	group = kv_get_group(parser, name);
	if (group == NULL) {
		group = calloc(1, sizeof(*group));
		group->name = name;
		kv_list_add(&parser->groups, &group->node);
	}
	return group;
}

int kv_remove_group(struct kv_parser *parser, struct kv_group *group)
{
	if (kv_list_remove(&parser->groups, &group->node) != 0)
		return -1;
	kv_destroy_group(group);
	return 0;
}

int kv_remove_group_by_name(struct kv_parser *parser, struct kv_str name)
{
	struct kv_group *group;
	group = kv_get_group(parser, name);
	if (group == NULL)
		return -1;
	return kv_remove_group(parser, group);
}

struct kv_key *kv_get_key(struct kv_group *group, struct kv_str name)
{
	struct kv_node *iter;
	struct kv_key *key;
	kv_list_foreach(iter, &group->keys) {
		key = (void *)iter;
		if (kv_strcmp(key->name, name) == 0)
			return key;
	}
	return NULL;
}

struct kv_key *kv_add_key(struct kv_group *group, struct kv_str name)
{
	struct kv_key *key;
	key = kv_get_key(group, name);
	if (key == NULL) {
		key = calloc(1, sizeof(*key));
		key->name = name;
		kv_list_add(&group->keys, &key->node);
	}
	return key;
}

int kv_remove_key(struct kv_group *group, struct kv_key *key)
{
	if (kv_list_remove(&group->keys, &key->node) != 0)
		return -1;
	kv_destroy_key(key);
	return 0;
}

int kv_remove_key_by_name(struct kv_group *group, struct kv_str name)
{
	struct kv_key *key;
	key = kv_get_key(group, name);
	if (key == NULL)
		return -1;
	return kv_remove_key(group, key);
}

struct kv_val *kv_get_val(struct kv_key *key, struct kv_str name)
{
	struct kv_node *iter;
	struct kv_val *val;
	kv_list_foreach(iter, &key->vals) {
		val = (void *)iter;
		if (kv_strcmp(val->name, name) == 0)
			return val;
	}
	return NULL;
}

struct kv_val *kv_add_val(struct kv_key *key, struct kv_str name)
{
	struct kv_val *val;
	val = kv_get_val(key, name);
	if (val == NULL) {
		val = calloc(1, sizeof(*val));
		val->name = name;
		kv_list_add(&key->vals, &val->node);
	}
	return val;
}

int kv_remove_val(struct kv_key *key, struct kv_val *val)
{
	if (kv_list_remove(&key->vals, &val->node) != 0)
		return -1;
	kv_destroy_val(val);
	return 0;
}

int kv_remove_val_by_name(struct kv_key *key, struct kv_str name)
{
	struct kv_val *val;
	val = kv_get_val(key, name);
	if (val == NULL)
		return -1;
	return kv_remove_val(key, val);
}

int kv_parse_group(struct kv_parser *parser, const char **buf)
{
	struct kv_str str;

	str.begin = *buf + 1;
	str.end = strchr(str.begin, ']');
	if (str.end == NULL) {
		fprintf(stderr, "Expected '['");
		return -1;
	}
	*buf = str.end + 1;
	parser->cur_group = kv_add_group(parser, str);
	return 0;
}

int kv_parse_comment(struct kv_parser *parser, const char **buf)
{
	struct kv_str str;

	str.begin = *buf;
	str.end = strchr(str.begin, '\n');
	if (str.end == NULL) {
		str.end = str.begin + strlen(str.begin);
		*buf = str.end;
	} else {
		*buf = str.end + 1;
	}
	return 0;
}

int kv_parse_key(struct kv_parser *parser, const char **buf)
{
	struct kv_str str;

	str.begin = *buf;
	str.end = str.begin;
	while (strchr(" \t\n\r=", *str.end) == NULL)
		++str.end;
	*buf = str.end;
	parser->cur_key = kv_add_key(parser->cur_group, str);
	return 0;
}

int kv_parse_val(struct kv_parser *parser, const char **buf)
{
	struct kv_str str;

	while (**buf == ' ' || **buf == '\t')
		++*buf;
	if (**buf != '=') {
		fprintf(stderr, "Expected '='\n");
		return -1;
	}
	++*buf;
	while (**buf == ' ' || **buf == '\t')
		++*buf;
	str.begin = *buf;
	str.end = strchr(str.begin, '\n');
	*buf = str.end + 1;
	parser->cur_val = kv_add_val(parser->cur_key, str);
	return 0;
}

void kv_dump(struct kv_parser *parser)
{
	struct kv_node *group_iter, *key_iter, *val_iter;
	struct kv_group *group;
	struct kv_key *key;
	struct kv_val *val;

	kv_list_foreach(group_iter, &parser->groups) {
		group = (void *)group_iter;
		putchar('\n');
		putchar('[');
		kv_dump_str(group->name);
		putchar(']');
		putchar('\n');
		kv_list_foreach(key_iter, &group->keys) {
			key = (void *)key_iter;
			kv_list_foreach(val_iter, &key->vals) {
				val = (void *)val_iter;
				kv_dump_str(key->name);
				putchar('=');
				kv_dump_str(val->name);
				putchar('\n');
			}
		}
	}
}

void kv_dump_str(struct kv_str str)
{
	const char *p;
	for (p = str.begin; p != str.end; ++p)
		putchar(*p);
}
