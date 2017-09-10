/*
 * Copyright (c) 2012 Rogerz Zhang <rogerz.zhang@gmail.com>
 *
 * Jansson is free software; you can redistribute it and/or modify
 * it under the terms of the MIT license. See LICENSE for details.
 */

#include <jansson.h>
#include <string.h>
#include "util.h"

static void test_object(json_t *json)
{

    // {
    //  "obj": {
    //      "int": 2,
    //      "arr": [
    //          3,
    //          { "nest": 4},
    //          [5]
    //      ]
    //  }
    // }


        if(json_path_get(json, "$") != json)
                fail("json_path_get() fails to get root json");
    if(json_integer_value(json_path_get(json, "$.obj.int")) != 2)
        fail("json_path_get() fails to get integer");
    if(!json_is_array(json_path_get(json, "$.obj.arr")))
        fail("json_path_get() fails to get array");
    if(json_integer_value(json_path_get(json, "$.obj.arr[0]")) != 3)
        fail("json_path_get() fails to get array member");
    if(json_integer_value(json_path_get(json, "$.obj.arr[1].nest")) !=4)
        fail("json_path_get() fails to get key of array member");
    if(json_integer_value(json_path_get(json, "$.obj.arr[2][0]")) != 5)
        fail("json_path_get() fails to get nest array");

//    if(json_path_get(json, "$this.obj") != NULL)
//        fail("json_path_get() fails to detect error: invalid root path");
    if(json_path_get(json, "$obj") != NULL)
        fail("json_path_get() fails to detect errro: missing '.'");
    if(json_path_get(json, "obj") != NULL)
        fail("json_path_get() fails to detect error: missing root '$'");
    if(json_path_get(json, "$[0]") != NULL)
        fail("json_path_get() fails to detect error: index in json");
    if(json_path_get(json, "$[0].int") != NULL)
        fail("json_path_get() fails to detect error: key in undefined");
    if(json_path_get(json, "$.ob") != NULL)
        fail("json_path_get() fails to detect error: non-exist path");

    json_decref(json);
}

static void test_array(void)
{
    json_t *json;

    // [
    //  10,
    //  { "foo": 11},
    //  [12]
    // ]

    json = json_pack("[i, {s:i}, [i]]", 10, "foo", 11, 12);

        if(json_path_get(json, "$") != json)
                fail("json_path_get() fails to get root array");

    if(json_integer_value(json_path_get(json, "$[0]")) != 10)
        fail("json_path_get() fails to get member in root array");

    if(json_integer_value(json_path_get(json, "$[1].foo")) != 11)
        fail("json_path_get() fails to get key of array member in root array");

    if(json_integer_value(json_path_get(json, "$[2][0]")) != 12)
        fail("json_path_get() fails to get nested array in root array");

    if(json_path_get(json, "[0]") != NULL)
        fail("json_path_get() fails to detect error: missing root in array");
    if(json_path_get(json, "$.[0]") != NULL)
        fail("json_path_get() fails to detect error: extra '.' in array");
    if(json_path_get(json, "$.0") != NULL)
        fail("json_path_get() fails to detect error: index as key");
    if(json_path_get(json, "$[0z]") != NULL)
        fail("json_path_get() fails to detect error: invalid index");
    if(json_path_get(json, "$.foo") != NULL)
        fail("json_path_get() fails to detect error: key in array");
    if(json_path_get(json, "$.foo[0]") != NULL)
        fail("json_path_get() fails to detect error: index on undefined");
    if(json_path_get(json, "$[3]") != NULL)
        fail("json_path_get() fails to detect error: array overflow");

    json_decref(json);

}

static void test_null()
{
    json_t *json = json_pack("[i, {s:i}, [i]]", 10, "foo", 11, 12);
    if(json_path_get(NULL, "$.foo") != NULL)
        fail("json_path_get() fails to detect NULL json");

    if(json_path_get(json, NULL) != NULL)
        fail("json_path_get() fails to detect NULL path");

    if(json_path_get(json, "") != NULL)
        fail("json_path_get() fails to detect empty string");

    json_decref(json);
}

#define dump_error() fprintf(stderr, "error at %d:\n%s\n", error.position, error.text)

static void test_set()
{
    json_error_t error;
    /*
        { "key": 11
            , "obj": {key: 12}
            , "arr": [13, {key:15}]
         }
      */
    json_t *json = json_pack("{si,s:{si},s:[i,{s:i}]}", "key", 11, "obj", "key", 12, "arr", 13, "key", 15);
    json_t *value = json_integer(21);

    /* normal cases */
    if(json_path_set(json, "$.key", value, 0, &error) < 0
            || !json_equal(json_path_get(json, "$.key"), value)) {
        dump_error();
        fail("json_path_set() fails to set $.key");
    }

    if(json_path_set(json, "$.obj.key", value, 0, &error) < 0
            || !json_equal(json_path_get(json, "$.obj.key"), value)) {
        dump_error();
        fail("json_path_set() fails to set $.obj.key");
    }

    if(json_path_set(json, "$.arr[0]", value, 0, &error) <0 
            || !json_equal(json_path_get(json, "$.arr[0]"), value)) {
        dump_error();
        fail("json_path_set() fails to set $.arr[0]");
    }

    if(json_path_set(json, "$.arr[1].key", value, 0, &error) < 0
            || !json_equal(json_path_get(json, "$.arr[1].key"), value)) {
        dump_error();
        fail("json_path_set() fails to set $.arr[1].key");
    }

    if(json_path_set(json, "$.new_key", value, 0, &error) < 0
            || !json_equal(json_path_get(json, "$.new_key"), value)) {
        dump_error();
        fail("json_path_set() fails to set new key");
    }

    if(json_path_set(json, "$.obj.new_key", value, 0, &error) < 0
            || !json_equal(json_path_get(json, "$.obj.new_key"), value)) {
        dump_error();
        fail("json_path_set() fails to set $.obj.new_key");
    }

    if(json_path_set(json, "$.arr[1].new_key", value, 0, &error) < 0
            || !json_equal(json_path_get(json, "$.arr[1].new_key"), value)) {
        dump_error();
        fail("json_path_set() fails to set $.obj.arr[1].new_key");
    }

    /* error cases */

    if(json_path_set(NULL, "$.key", value, 0, &error) == 0 || strcmp(error.text, "invalid argument"))
        fail("json_path_set() fails to detect error: invalid argument");

    if(json_path_set(json, NULL, value, 0, &error) == 0 || strcmp(error.text, "invalid argument"))
        fail("json_path_set() fails to detect error: invalid argument");

    if(json_path_set(json, "$.key", NULL, 0, &error) == 0 || strcmp(error.text, "invalid argument"))
        fail("json_path_set() fails to detect error: invalid argument");

    if(json_path_set(json, "$", json_integer(21), 0, &error) == 0 || strcmp(error.text, "invalid path")) {
        fprintf(stderr, "error at %d:\n%s", error.position, error.text);
        fail("json_path_set() fails to detect error: invalid path");
    }

    if(json_path_set(json, "obj", json_integer(21), 0, &error) == 0 || strcmp(error.text, "path should start with $")) {
        dump_error();
        fail("json_path_set() fails to detect error: path should start with $");
    }

    if(json_path_set(json, "$obj.key", json_integer(21), 0, &error) == 0 || strcmp(error.text, "unexpected trailing chars")) {
        dump_error();
        fail("json_path_set() fails to detect error: unexpected trailing chars");
    }

    if(json_path_set(json, "$.arr[0", json_integer(21), 0, &error) == 0 || strcmp(error.text, "missing ']'?")) {
        dump_error();
        fail("json_path_set() fails to detect error: missing ']'?");
    }

    if(json_path_set(json, "$..obj", json_integer(21), 0, &error) == 0 || strcmp(error.text, "empty token")) {
        dump_error();
        fail("json_path_set() fails to detect error: empty token");
    }

    if(json_path_set(json, "$.arr.0", json_integer(21), 0, &error) == 0 || strcmp(error.text, "object expected")) {
        dump_error();
        fail("json_path_set() fails to detect error: object expected");
    }

    if(json_path_set(json, "$.obj[0]", json_integer(21), 0, &error) == 0 || strcmp(error.text, "array expected")) {
        dump_error();
        fail("json_path_set() fails to detect error: array expected");
    }

    if(json_path_set(json, "$.new_arr[0]", json_integer(21), 0, &error) == 0 || strcmp(error.text, "new array is not allowed")) {
        dump_error();
        fail("json_path_set() fails to detect error: new array is not allowed");
    }

    if(json_path_set(json, "$.arr[3]", json_integer(21), 0, &error) == 0 || strcmp(error.text, "array index out of bound")) {
        dump_error();
        fail("json_path_set() fails to detect error: array index out of bound");
    }
}

static void run_tests()
{
    json_t *json;
    json_error_t err;
    json = json_pack("{s:{s:i,s:[i,{s:i},[i]]}}", "obj", "int", 2, "arr", 3, "nest", 4, 5);
    test_object(json);
    json = json_load_file("./test/test_object.json", 0, &err);
    test_object(json);
    test_array();
    test_null();
    test_set();
}
