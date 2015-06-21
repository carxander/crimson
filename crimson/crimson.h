/*
 * cjson.h
 *
 *  Created on: Jun 5, 2015
 *      Author: carlos
 */

#ifndef CRIMSON_H_
#define CRIMSON_H_

#include <stdlib.h>

typedef enum
{
	JSON_TYPE_NULL = 0,
	JSON_TYPE_BOOLEAN,
	JSON_TYPE_STRING,
	JSON_TYPE_NUMBER,
	JSON_TYPE_OBJECT,
	JSON_TYPE_ARRAY
} JSON_VALUE_TYPE;

typedef struct
{
	void *val;
	JSON_VALUE_TYPE type;
	void *next;
} JSON_VALUE;

typedef struct
{
	char *key;
	JSON_VALUE *value;
	void *next;
} JSON_PAIR;

typedef struct
{
	JSON_VALUE *first_value;
} JSON_ARRAY;

typedef struct
{
	JSON_PAIR *first_pair;
} JSON_OBJECT;

JSON_VALUE *crimson_new_value(JSON_VALUE_TYPE vtype, void *value);

JSON_PAIR *crimson_new_pair(const char *key, JSON_VALUE *value);

JSON_ARRAY *crimson_new_array(JSON_VALUE *value);

JSON_OBJECT *crimson_new_object(JSON_PAIR *pair);

int crimson_append_value(JSON_ARRAY *array, JSON_VALUE *value);

int crimson_add_pair(JSON_OBJECT *object, JSON_PAIR *pair);

int crimson_append_pair(JSON_OBJECT *object, JSON_PAIR *pair);

void crimson_delete_value(JSON_VALUE *value);

void crimson_delete_array(JSON_ARRAY *array);

void crimson_delete_pair(JSON_PAIR *pair);

void crimson_delete_object(JSON_OBJECT *object);

JSON_PAIR *crimson_get_pair(JSON_OBJECT *obj, const char *key);

int crimson_tostr_value(JSON_VALUE *value, FILE *stream, char **str);

int crimson_tostr_pair(JSON_PAIR *pair, FILE *stream, char **str);

int crimson_tostr_array(JSON_ARRAY *array, FILE *stream, char **str);

int crimson_tostr_object(JSON_OBJECT *object, FILE *stream, char **str);

JSON_OBJECT *crimson_parse_object(char **str);

JSON_VALUE *crimson_get_value(JSON_OBJECT *obj, const char *key);

char *crimson_get_value_str(JSON_OBJECT *obj, const char *key);

int crimson_edit_value(JSON_OBJECT *obj, const char *key, JSON_VALUE_TYPE type, void *nval);

#define crimson_add_string(o, k, s)		crimson_add_pair((o), crimson_new_pair(k, crimson_new_value(JSON_TYPE_STRING, s)))

#define crimson_append_string(o, k, s)	crimson_append_pair((o), crimson_new_pair(k, crimson_new_value(JSON_TYPE_STRING, s)))

int crimson_validate_object(JSON_OBJECT *j_orig, JSON_OBJECT *j_model);

#endif
