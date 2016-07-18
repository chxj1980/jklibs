//
// Created by v on 16-7-18.
// Author: jmdvirus@roamter.com
//

#include <stdio.h>

#include "json-c/json.h"
const char *filename = "/tmp/jsonc.json";

int demo_json_parse_from_file()
{

    json_object *j_object;
    j_object = json_object_from_file(filename);

    printf("j_object to string: %s\n", json_object_to_json_string(j_object));

    json_object *j_string;
    json_object_object_get_ex(j_object, "test2", &j_string);
    printf("j_string: %s\n", json_object_to_json_string(j_string));

    printf("j_string type: %d\n", json_object_get_type(j_string));

    json_object_put(j_string);
    json_object_put(j_object);

    return 0;
}

int demo_json_generate() {
    json_object *j_string;
    j_string = json_object_new_string("Hello");
    printf("json string: %s\n", json_object_get_string(j_string));
    printf("json string. ToString() = %s\n", json_object_to_json_string(j_string));
//    json_object_put(j_string);

    json_object *j_int;
    j_int = json_object_new_int(32);
    printf("json int: %d\n", json_object_get_int(j_int));
    printf("json int toString: %s\n", json_object_to_json_string(j_int));
//    json_object_put(j_int);

    json_object *j_array;
    j_array = json_object_new_array();
    json_object_array_add(j_array, json_object_new_int(55));
    json_object_array_add(j_array, json_object_new_int(58));

    json_object *j_object;
    j_object = json_object_new_object();
    json_object_object_add(j_object, "test1", j_string);
    json_object_object_add(j_object, "test2", j_int);
    json_object_object_add(j_object, "test3", j_array);

    json_object_object_foreach(j_object, key, val) {
        printf("Object key: %s, %s\n", key, json_object_get_string(val));
    }
    printf("\n");

    printf("j_object= %s\n", json_object_to_json_string(j_object));

    json_object_to_file(filename, j_object);

    json_object_put(j_object);
    json_object_put(j_array);
    json_object_put(j_int);
    json_object_put(j_string);
    return 0;
}

int main()
{
    demo_json_generate();
    demo_json_parse_from_file();
}