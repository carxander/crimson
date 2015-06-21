/*
 * main.c
 *
 *  Created on: Jun 5, 2015
 *      Author: carlos
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "crimson.h"

int main(void) {

	FILE *tst;
	tst = fopen("JSON", "r");
	char json[40000];
	memset(json, 0, 40000);
	fgets(json, 40000, tst);
	fclose(tst);

	char *parser = json;
	JSON_OBJECT *obj = crimson_parse_object(&parser);

	if (obj == NULL)
	{
		printf("Error in char #%d, near:\n%s\n          ^", (int)(parser-json), parser-10);
		return 0;
	}

	//crimson_edit_value(obj, "ENTITYS", JSON_TYPE_STRING, "Cero cero");

	JSON_OBJECT *obj2;
	obj2 = crimson_new_object(NULL);
	crimson_add_string(obj2, "RSP_CODE2", "TT");
	crimson_add_string(obj2, "ENTITYS", "TEST2");

	if (crimson_validate_object(obj2, obj) == 0)
	{
		puts("Json valido\n");
	}
	else
	{
		puts("Json invalido\n");
	}
	crimson_delete_object(obj2);

	char *tostr;
	crimson_tostr_object(obj, NULL, &tostr);
	puts(tostr);
	free(tostr);
	crimson_delete_object(obj);

	return EXIT_SUCCESS;
}

