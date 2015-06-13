/*
 * main.c
 *
 *  Created on: Jun 5, 2015
 *      Author: carlos
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "crimson.h"

int main(void) {

	char *json = "{\"Name\":\"Carlos\",\"Age\":24,       \n\t     \"Married\":true, \"Phones\":[\"809-599-1111\",\"809-599-2222\",\"809-599-3333\"]}";
	char *json2 = "{\"Name\":\"Richard\",\"Age\":21,       \n\t     \"Married\":false}";

	char *parser = json;
	JSON_OBJECT *obj = crimson_parse_object(&parser);

	if (obj == NULL)
	{
		puts("Error near:");
		puts(parser);
		return 0;
	}

	parser = json2;
	JSON_OBJECT *obj2 = crimson_parse_object(&parser);
	if (obj == NULL)
	{
		puts("Error near:");
		puts(parser);
		return 0;
	}

	crimson_add_pair(obj, crimson_new_pair("Name", crimson_new_value(JSON_TYPE_STRING, "Alexander")));

	char *str;
	crimson_tostr_object(obj, NULL, &str);
	puts(str);
	free(str);

	crimson_delete_object(obj);
	crimson_delete_object(obj2);



	return EXIT_SUCCESS;
}

