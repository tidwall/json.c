#include "tests.h"

void test_json_parse_testfiles(void) {
    for (size_t i = 0; i < sizeof(names)/sizeof(char *); i++){
        char path[128];
        // if (strcmp(names[i], "update-center.json")) continue;
        // printf("%s\n", names[i]);
        snprintf(path, sizeof(path), "testfiles/%s", names[i]);
        char *src = readfile(path, 0);
        assert(json_valid(src));
        // assert(json_valid_opts(src, -1, JSON_NOCHECKUTF8));
        // assert(json_valid_opts(src, -1, JSON_NOCHECKUTF8|JSON_RETERRPOS) == 0);
        struct json json = json_parse(src);
        assert(json_exists(json));
        touch_all(json_parse(src));
        free(src);
    }
}

void test_json_parse_various(void) {
    assert(json_exists(json_parse("{}")));
    assert(json_exists(json_parse(" {}")));
    assert(json_exists(json_parse(" {} ")));
    assert(json_exists(json_parse(" { } ")));
    assert(json_exists(json_parse(" \" ")));
    assert(json_exists(json_parse(" \"\" ")));
    assert(json_exists(json_parse(" \"1\" ")));
    assert(json_exists(json_parse(" -1 ")));
    assert(json_exists(json_parse(" 0 ")));
    assert(json_exists(json_parse(" 1 ")));
    assert(json_exists(json_parse(" 2 ")));
    assert(json_exists(json_parse(" 3 ")));
    assert(json_exists(json_parse(" 4 ")));
    assert(json_exists(json_parse(" 5 ")));
    assert(json_exists(json_parse(" 6 ")));
    assert(json_exists(json_parse(" 7 ")));
    assert(json_exists(json_parse(" 8 ")));
    assert(json_exists(json_parse(" 9 ")));
    assert(json_exists(json_parse(" true ")));
    assert(json_exists(json_parse(" t ")));
    assert(json_exists(json_parse(" false ")));
    assert(json_exists(json_parse(" f ")));
    assert(json_exists(json_parse(" null ")));
    assert(json_exists(json_parse(" n ")));
    assert(json_exists(json_parsen("{}", 2)));
    assert(json_exists(json_parsen("[1]", 3)));
    assert(json_exists(json_parsen("{} ", 3)));
    assert(json_exists(json_parsen("[1] ", 4)));
    assert(json_exists(json_parsen(" {}", 3)));
    assert(json_exists(json_parsen(" [1]", 4)));
    assert(json_exists(json_parsen(" {} ", 4)));
    assert(json_exists(json_parsen(" [1] ", 5)));
    assert(!json_string_is_escaped(json_parse("\"hello\"")));
    assert(json_string_is_escaped(json_parse("\"hel\\nlo\"")));
}

void test_json_valid_big(void) {
    char *json = readfile("testfiles/twitter.json", 0);
    assert(json_valid(json));
    free(json);
    json = readfile("testfiles/canada.json", 0);
    assert(json_valid(json));
    free(json);
    json = readfile("testfiles/numbers.json", 0);
    assert(json_valid(json));
    free(json);
}

void test_json_valid_invalid(void) {
    assert(!json_validn("{}", -2));
    assert(!json_valid(""));
    assert(!json_valid(" "));
    assert(!json_valid("{"));
    assert(!json_valid(" {"));
    assert(!json_valid(" {\""));
    assert(!json_valid("{\"a\""));
    assert(!json_valid("{\"a\" "));
    assert(!json_valid("{\"a\"}"));
    assert(!json_valid("{\"a\" }"));
    assert(!json_valid("{\"a\":}"));
    assert(!json_valid("{\"a\" :}"));
    assert(!json_valid("{\"a\" : }"));
    assert(!json_valid("{\"a\":1"));
    assert(!json_valid("{\"a\":1,"));
    assert(!json_valid("{\"a\":1,\""));
    assert(!json_valid("{\"a\":1,\""));
    assert(!json_valid("{\"a\":1]"));
    assert(!json_valid("{\"a\":1,\"b\"}"));
    assert(!json_valid("{\"a\":\"he\bllo\"}"));
    assert(!json_valid("{\"a\":\"he\\llo\"}"));
    assert(!json_valid("{\"a\":\"he\\"));
    assert(!json_valid("{\"a\":\"he\\u"));
    assert(!json_valid("{\"a\":\"he\\uZ"));
    assert(!json_valid("{\"a\":\"he\\uz"));
    assert(!json_valid("{\"a\":\"he\\u3454"));
    assert(!json_valid("{\"a\":-"));
    assert(!json_valid("{\"a\":+"));
    assert(!json_valid("{\"a\":-."));
    assert(!json_valid("{\"a\":-.0"));
    assert(!json_valid("{\"a\":-.0"));
    assert(!json_valid("{\"a\":-0."));
    assert(!json_valid("{\"a\":-0.X"));
    assert(!json_valid("{\"a\":-0.0"));
    assert(!json_valid("{\"a\":-0.0e"));
    assert(!json_valid("{\"a\":-0.0E"));
    assert(!json_valid("{\"a\":-0.0E+"));
    assert(!json_valid("{\"a\":-0.0E-"));
    assert(!json_valid("{\"a\":-0.0E*"));
    assert(!json_valid("{\"a\":t"));
    assert(!json_valid("{\"a\":tr"));
    assert(!json_valid("{\"a\":f"));
    assert(!json_valid("{\"a\":fa"));
    assert(!json_valid("{\"a\":n"));
    assert(!json_valid("{\"a\":nu"));
    assert(!json_valid("["));
    assert(!json_valid(" ["));
    assert(!json_valid(" [ "));
    assert(!json_valid(" [ x"));
    assert(!json_valid(" [ true,"));
    assert(!json_valid(" [ true "));
    assert(!json_valid(" [ tru"));
    assert(!json_valid("{   } d"));
    assert(!json_valid("{ 1 } d"));
    assert(!json_valid("{ \"a\": 1, 1 } d"));
    assert(!json_valid("{ \"a\": z, 1 } d"));
    assert(!json_valid("{ \"a\": } d"));
    assert(!json_valid_ex("{\"1\",}", 0).valid);
    assert(json_valid_ex("{\"1\",}", 0).pos == 4);
    assert(json_valid_ex("", 0).pos == 0);
    assert(json_valid_ex("!", 0).pos == 0);
    assert(json_valid_ex("1!", 0).pos == 1);

    assert(json_valid("[1 , 2 , 3 , 4]"));
    assert(json_valid("{\"ke\\ny\": \"value\"}"));
}

void test_json_iter(void) {
    struct json json;
    json = json_first(json_parse("{}"));
    assert(!json_exists(json));
    json = json_first(json_parsen("{\"a\"}",-2));
    assert(!json_exists(json));
    json = json_first(json_parse("{\"a\"}"));
    assert(json_raw_compare(json, "\"a\"") == 0);
    assert(json_exists(json));
    json = json_first(json_parse("{\"\\a\"}"));
    assert(json_raw_compare(json, "\"\\a\"") == 0);
    assert(json_exists(json));
    json = json_first(json_parse("{\"\\a\\\\\""));
    assert(json_exists(json));
    json = json_first(json_parse("{\"\\a\\\\\"}"));
    assert(json_exists(json));
    json = json_first(json_parse("{\"\\a\\\\"));
    assert(json_exists(json));
    json = json_first(json_parse("{\"\\a\\\""));
    assert(json_exists(json));
    json = json_first(json_parse("{\"hello\" 123"));
    assert(json_raw_compare(json, "\"hello\"") == 0);
    assert(json_exists(json));
    json = json_next(json_first(json_parse("{\"a\":0}")));
    assert(json_exists(json));
    json = json_next(json_first(json_parse("{\"a\":1}")));
    assert(json_exists(json));
    json = json_next(json_first(json_parse("{\"a\":-1}")));
    assert(json_exists(json));
    json = json_next(json_first(json_parse("{\"a\":-1.1}")));
    assert(json_exists(json));
    json = json_next(json_first(json_parse("{\"a\":-1.1e}")));
    assert(json_exists(json));
    json = json_next(json_first(json_parse("{\"a\":-112.1993477198}")));
    assert(json_raw_compare(json, "-112.1993477198") == 0);
    assert(json_exists(json));
    json = json_next(json_first(json_parse("{\"a\":[{},[],\"\"]}")));
    assert(json_exists(json));
    json = json_next(json_first(json_parse("{\"a\":[{},[],\"\"]},\"b\":2")));
    json = json_next(json);
    assert(!json_exists(json));
    json = json_next(json_first(json_parse("{\"a\":{\"b\":[true]}")));
    assert(json_exists(json));
    json = json_next(json_first(json_parse("{\"a\":true")));
    assert(json_raw_compare(json, "true") == 0);
    assert(json_exists(json));
    json = json_next(json_first(json_parse("{\"a\":tru")));
    assert(json_raw_compare(json, "tru") == 0);
    assert(json_exists(json));
    json = json_next(json_first(json_parse("{\"a\":false")));
    assert(json_raw_compare(json, "false") == 0);
    assert(json_exists(json));
    json = json_next(json_first(json_parse("{\"a\":fals")));
    assert(json_exists(json));
    json = json_next(json_first(json_parse("{\"a\":null")));
    assert(json_exists(json));
    json = json_next(json_first(json_parse("{\"a\":nul")));
    assert(json_exists(json));
    json = json_next(json_parse("nul"));
    assert(!json_exists(json));
    json = json_next((struct json) { 0 });
    assert(!json_exists(json));
    json = json_next(json_first(json_parse("{\"a\" : true }")));
    assert(json_exists(json));
    json = json_parse("[1,2,3]");
    assert(json_exists(json));
    json = json_first(json);
    assert(json_exists(json));
    json = json_next(json);
    assert(json_exists(json));
    json = json_next(json_first(json_parse("{\"a\":{")));
    assert(json_raw_compare(json, "{") == 0);
    json = json_parse("{");
    assert(json_raw_compare(json, "{") == 0);
    json = json_parse("{lkjlkjasdfasdfd");
    assert(json_raw_compare(json, "{lkjlkjasdfasdfd") == 0);
    json = json_parse("{lkjlkjasdfasdf");
    assert(json_raw_compare(json, "{lkjlkjasdfasdf") == 0);
    json = json_parse("{lkjlkjasdfasd");
    assert(json_raw_compare(json, "{lkjlkjasdfasd") == 0);
    json = json_parse("{lkjlkjasdfas");
    assert(json_raw_compare(json, "{lkjlkjasdfas") == 0);
    json = json_parse("{lkjlkjasdfa");
    assert(json_raw_compare(json, "{lkjlkjasdfa") == 0);
    json = json_parse("{lkjlkjasdfa}");
    assert(json_raw_compare(json, "{lkjlkjasdfa}") == 0);
    json = json_parse("{lkjlkjasdfalkjlkjasdfalkjlkjasdfa}");
    assert(json_raw_compare(json, "{lkjlkjasdfalkjlkjasdfalkjlkjasdfa}") == 0);
    
    
    // strings
    json = json_parse("\"lkjlkjasdfalkjlkjasdfalkjlkjasdfa\"");
    assert(json_raw_compare(json, "\"lkjlkjasdfalkjlkjasdfalkjlkjasdfa\"") == 0);
    json = json_parse("\"lkjlkjasdfalkjlk\",,,,,,,,,,,,,,");
    assert(json_raw_compare(json, "\"lkjlkjasdfalkjlk\"") == 0);
    json = json_parse("\"lkjlkjasdfalkjl\",,,,,,,,,,,,,,,");
    assert(json_raw_compare(json, "\"lkjlkjasdfalkjl\"") == 0);
    json = json_parse("\"lkjlkjasdfalkj\",,,,,,,,,,,,,,,,");
    assert(json_raw_compare(json, "\"lkjlkjasdfalkj\"") == 0);
    json = json_parse("\"lkjlkjasdfalk\",,,,,,,,,,,,,,,,,");
    assert(json_raw_compare(json, "\"lkjlkjasdfalk\"") == 0);
    json = json_parse("\"lkjlkjasdfal\",,,,,,,,,,,,,,,,,,");
    assert(json_raw_compare(json, "\"lkjlkjasdfal\"") == 0);
    json = json_parse("\"lkjlkjasdfa\",,,,,,,,,,,,,,,,,,,");
    assert(json_raw_compare(json, "\"lkjlkjasdfa\"") == 0);
    json = json_parse("\"lkjlkjasdf\",,,,,,,,,,,,,,,,,,,,");
    assert(json_raw_compare(json, "\"lkjlkjasdf\"") == 0);
    json = json_parse("\"lkjlkjasd\",,,,,,,,,,,,,,,,,,,,,");
    assert(json_raw_compare(json, "\"lkjlkjasd\"") == 0);
    json = json_parse("\"lkjlkjas\",,,,,,,,,,,,,,,,,,,,,,");
    assert(json_raw_compare(json, "\"lkjlkjas\"") == 0);
    json = json_parse("\"lkjlkja\",,,,,,,,,,,,,,,,,,,,,,,");
    assert(json_raw_compare(json, "\"lkjlkja\"") == 0);
    json = json_parse("\"lkjlkj\",,,,,,,,,,,,,,,,,,,,,,,,");
    assert(json_raw_compare(json, "\"lkjlkj\"") == 0);
    json = json_parse("\"lkjlk\",,,,,,,,,,,,,,,,,,,,,,,,,");
    assert(json_raw_compare(json, "\"lkjlk\"") == 0);
    json = json_parse("\"lkjl\",,,,,,,,,,,,,,,,,,,,,,,,,,");
    assert(json_raw_compare(json, "\"lkjl\"") == 0);
    json = json_parse("\"lkj\",,,,,,,,,,,,,,,,,,,,,,,,,,,");
    assert(json_raw_compare(json, "\"lkj\"") == 0);
    json = json_parse("\"lk\",,,,,,,,,,,,,,,,,,,,,,,,,,,,");
    assert(json_raw_compare(json, "\"lk\"") == 0);
    json = json_parse("\"l\",,,,,,,,,,,,,,,,,,,,,,,,,,,,,");
    assert(json_raw_compare(json, "\"l\"") == 0);
    json = json_parse("\"\",,,,,,,,,,,,,,,,,,,,,,,,,,,,,,");
    assert(json_raw_compare(json, "\"\"") == 0);

    // numbers
    json = json_parse("-123909103981029810923810293801931");
    assert(json_raw_compare(json, "-123909103981029810923810293801931") == 0);
    json = json_parse("-,,,,,,,,,,,,,,,,,,,,,,,,,");
    assert(json_raw_compare(json, "-") == 0);
    json = json_parse("-1,,,,,,,,,,,,,,,,,,,,,,,,");
    assert(json_raw_compare(json, "-1") == 0);
    json = json_parse("-12,,,,,,,,,,,,,,,,,,,,,,,,");
    assert(json_raw_compare(json, "-12") == 0);
    json = json_parse("-123,,,,,,,,,,,,,,,,,,,,,,,");
    assert(json_raw_compare(json, "-123") == 0);
    json = json_parse("-1234,,,,,,,,,,,,,,,,,,,,,,");
    assert(json_raw_compare(json, "-1234") == 0);
    json = json_parse("-12345,,,,,,,,,,,,,,,,,,,,,");
    assert(json_raw_compare(json, "-12345") == 0);
    json = json_parse("-123456,,,,,,,,,,,,,,,,,,,,");
    assert(json_raw_compare(json, "-123456") == 0);
    json = json_parse("-1234567,,,,,,,,,,,,,,,,,,,");
    assert(json_raw_compare(json, "-1234567") == 0);
    json = json_parse("-12345678,,,,,,,,,,,,,,,,,,");
    assert(json_raw_compare(json, "-12345678") == 0);
    json = json_parse("-123456789,,,,,,,,,,,,,,,,,");
    assert(json_raw_compare(json, "-123456789") == 0);
    json = json_parse("-1234567890,,,,,,,,,,,,,,,,");
    assert(json_raw_compare(json, "-1234567890") == 0);
    json = json_parse("-12345678901,,,,,,,,,,,,,,,");
    assert(json_raw_compare(json, "-12345678901") == 0);
    json = json_parse("-123456789012,,,,,,,,,,,,,,");
    assert(json_raw_compare(json, "-123456789012") == 0);
    json = json_parse("-1234567890123,,,,,,,,,,,,,");
    assert(json_raw_compare(json, "-1234567890123") == 0);
    json = json_parse("-12345678901234,,,,,,,,,,,,");
    assert(json_raw_compare(json, "-12345678901234") == 0);
    json = json_parse("-123456789012345,,,,,,,,,,,");
    assert(json_raw_compare(json, "-123456789012345") == 0);
    json = json_parse("-1234567890123456,,,,,,,,,,");
    assert(json_raw_compare(json, "-1234567890123456") == 0);

    // squash
    json = json_parse("[12345678901234567890],,,,,");
    assert(json_raw_compare(json, "[12345678901234567890]") == 0);
    json = json_parse("[1234567890123456789],,,,,,");
    assert(json_raw_compare(json, "[1234567890123456789]") == 0);
    json = json_parse("[123456789012345678],,,,,,,");
    assert(json_raw_compare(json, "[123456789012345678]") == 0);
    json = json_parse("[12345678901234567],,,,,,,,");
    assert(json_raw_compare(json, "[12345678901234567]") == 0);
    json = json_parse("[1234567890123456],,,,,,,,,");
    assert(json_raw_compare(json, "[1234567890123456]") == 0);
    json = json_parse("[123456789012345],,,,,,,,,,");
    assert(json_raw_compare(json, "[123456789012345]") == 0);
    json = json_parse("[12345678901234],,,,,,,,,,,");
    assert(json_raw_compare(json, "[12345678901234]") == 0);
    json = json_parse("[1234567890123],,,,,,,,,,,,");
    assert(json_raw_compare(json, "[1234567890123]") == 0);
    json = json_parse("[123456789012],,,,,,,,,,,,,");
    assert(json_raw_compare(json, "[123456789012]") == 0);
    json = json_parse("[12345678901],,,,,,,,,,,,,,");
    assert(json_raw_compare(json, "[12345678901]") == 0);
    json = json_parse("[1234567890],,,,,,,,,,,,,,,");
    assert(json_raw_compare(json, "[1234567890]") == 0);
    json = json_parse("[123456789],,,,,,,,,,,,,,,,");
    assert(json_raw_compare(json, "[123456789]") == 0);
    json = json_parse("[12345678],,,,,,,,,,,,,,,,,");
    assert(json_raw_compare(json, "[12345678]") == 0);
    json = json_parse("[1234567],,,,,,,,,,,,,,,,,,");
    assert(json_raw_compare(json, "[1234567]") == 0);
    json = json_parse("[123456],,,,,,,,,,,,,,,,,,,");
    assert(json_raw_compare(json, "[123456]") == 0);
    json = json_parse("[12345],,,,,,,,,,,,,,,,,,,,");
    assert(json_raw_compare(json, "[12345]") == 0);
    json = json_parse("[1234],,,,,,,,,,,,,,,,,,,,,");
    assert(json_raw_compare(json, "[1234]") == 0);
    json = json_parse("[123],,,,,,,,,,,,,,,,,,,,,,");
    assert(json_raw_compare(json, "[123]") == 0);
    json = json_parse("[12],,,,,,,,,,,,,,,,,,,,,,,");
    assert(json_raw_compare(json, "[12]") == 0);
    json = json_parse("[1],,,,,,,,,,,,,,,,,,,,,,,,");
    assert(json_raw_compare(json, "[1]") == 0);
    json = json_parse("[],,,,,,,,,,,,,,,,,,,,,,,,,");
    assert(json_raw_compare(json, "[]") == 0);

    json = json_first(json_first(json_parse(
        "[[{1},[1],\"1\",true,false,null,-1,9],1]")));
    assert(json_raw_compare(json, "{1}") == 0);
    assert(json_raw(json)[0] == '{');
    assert(json_type(json) == JSON_OBJECT);
    json = json_ensure(json);
    assert(json_raw_compare(json, "{1}") == 0);
    assert(json_raw(json)[0] == '{');
    assert(json_type(json) == JSON_OBJECT);
    json = json_next(json);
    assert(json_raw_compare(json, "[1]") == 0);
    assert(json_type(json) == JSON_ARRAY);
    json = json_next(json);
    assert(json_raw_compare(json, "\"1\"") == 0);
    assert(json_type(json) == JSON_STRING);
    json = json_next(json);
    assert(json_raw_compare(json, "true") == 0);
    assert(json_type(json) == JSON_TRUE);
    json = json_next(json);
    assert(json_raw_compare(json, "false") == 0);
    assert(json_raw_compare(json, "fals") == 1);
    assert(json_raw_compare(json, "falsee") == -1);
    assert(json_type(json) == JSON_FALSE);
    json = json_next(json);
    assert(json_raw_compare(json, "null") == 0);
    assert(json_type(json) == JSON_NULL);
    json = json_next(json);
    assert(json_raw_compare(json, "-1") == 0);
    assert(json_type(json) == JSON_NUMBER);
    json = json_next(json);
    assert(json_raw_compare(json, "9") == 0);
    assert(json_type(json) == JSON_NUMBER);
    json = json_next(json);
    assert(json_raw(json) == NULL);
    assert(json_raw_compare(json, "") == 0);
    assert(json_type(json) == JSON_NULL);


    char *json_str = "[1,2,[3,4,[\"12345\\r\\n67890 abcdefg \\t 123 12345\\r\\n67890 abcdefg \\\\t \\\\\\\" 123\"]]]";
    json = json_parse(json_str);
    assert(json_raw_length(json) == strlen(json_str));
    
    json_str = "[1,2,[3,4,[\"12345\\r\\n67890 abcdefg \\t 123 12345\\r\\n67890 abcdefg \\\\t";
    json = json_parse(json_str);
    assert(json_raw_length(json) == strlen(json_str));


}

void test_json_compare(void) {
    char *src = readfile("testfiles/twitter.json", 0);
    assert(json_valid(src));
    struct json json = json_parse(src);
    json = json_first(json);
    assert(json_raw_compare(json, "\"statuses\"") == 0);
    json = json_next(json);
    json = json_first(json);
    json = json_first(json);
    json = json_next(json);
    json = json_next(json);
    json = json_next(json);
    json = json_next(json);
    json = json_next(json);
    json = json_next(json);
    json = json_next(json);
    json = json_next(json);
    assert(json_raw_compare(json, "\"text\"") == 0);
    json = json_next(json);
    char *str = "\"@aym0566x \\n\\n名前:前田あゆみ\\n第一印象:なんか怖っ！"
                "\\n今の印象:とりあえずキモい。噛み合わない\\n好きなところ:ぶす"
                "でキモいとこ😋✨✨\\n思い出:んーーー、ありすぎ😊❤️\\nLINE交換で"
                "きる？:あぁ……ごめん✋\\nトプ画をみて:照れますがな😘✨\\n一言:お"
                "前は一生もんのダチ💖\"";
    assert(json_raw_compare(json, str) == 0);
    free(src);
}

void test_json_string(void) {
    
    struct json json;
    
    json = json_parse("\"\"");
    assert(json_string_length(json) == 0);
    
    json = json_parse("\"h\"");
    assert(json_string_length(json) == 1);

    json = json_parse("\"\\n\"");
    assert(json_string_length(json) == 1);

    json = json_parse("\"\\\\\\n\"");
    assert(json_string_length(json) == 2);
    json = json_parse("\"hello\\\\\\nworld\"");
    assert(json_string_length(json) == 12);

    json = json_parse("\"hello\\\\\\nworld");
    assert(json_string_length(json) == 11);

    json = json_parse("\"");
    assert(json_string_length(json) == 0);
    
    json = json_parse("\",");
    assert(json_string_length(json) == 0);

    json = json_parse("\",:");
    assert(json_string_length(json) == 1);

    json = json_parse("\"hello\"");
    assert(json_string_length(json) == 5);


    json = json_parse("\"12\\\"");
    assert(json_string_length(json) == 2);

    json = json_parse("\"12\\u\"");
    assert(json_string_length(json) == 2);

    json = json_parse("\"12\\u0\"");
    assert(json_string_length(json) == 2);

    json = json_parse("\"12\\u00\"");
    assert(json_string_length(json) == 2);

    json = json_parse("\"12\\u004\"");
    assert(json_string_length(json) == 2);

    json = json_parse("\"12\\u0041\"");
    assert(json_string_length(json) == 3);

    json = json_parse("\"12\\u0041\"");
    assert(json_string_length(json) == 3);

    assert(json_string_compare(json, "12A") == 0);
    assert(json_string_compare(json, "12B") == -1);
    assert(json_string_compare(json, "129") == 1);
    assert(json_string_compare(json, "12A1") == -1);
    assert(json_string_compare(json, "12B1") == -1);
    assert(json_string_compare(json, "1291") == 1);
    assert(json_string_compare(json, "12") == 1);
    assert(json_string_compare(json, "") == 1);
    assert(json_string_compare(json, NULL) == 1);
    assert(json_string_compare(json, "0") == 1);
    assert(json_string_compare(json, "2") == -1);

    char *src = "{\"utf8\":\"Example emoji, KO: \\ud83d\\udd13, "
        "\\ud83c\\udfc3 OK: \\u2764\\ufe0f \"}";
    json = json_next(json_first(json_parse(src)));
    char *exp = "Example emoji, KO: 🔓, 🏃 OK: ❤️ ";
    assert(json_string_compare(json, exp) == 0);

    json = json_parse(src);
    assert(json_string_compare(json, src) == 0);

    json = json_parse("\"");
    assert(json_string_compare(json, "") == 0);


    json = json_parse("{}");
    assert(json_string_length(json) == 2);

    json = json_parse("123");

    char out[64];
    assert(json_string_copy(json, out, 0) == 3);
    
    assert(json_string_copy(json, out, 1) == 3);
    assert(strcmp(out, "") == 0);

    assert(json_string_copy(json, out, 2) == 3);
    assert(strcmp(out, "1") == 0);

    assert(json_string_copy(json, out, 3) == 3);
    assert(strcmp(out, "12") == 0);

    assert(json_string_copy(json, out, 4) == 3);
    assert(strcmp(out, "123") == 0);

    assert(json_string_copy(json, out, 5) == 3);
    assert(strcmp(out, "123") == 0);

    
    json = json_parse("\"\"");
    assert(json_string_copy(json, out, sizeof(out)) == 0);
    assert(strcmp(out, "") == 0);

    json = json_parse("\"\"");
    assert(json_string_copy(json, out, -1) == 0);
    assert(strcmp(out, "") == 0);


    json = json_parse("\"1\"");
    assert(json_string_copy(json, out, sizeof(out)) == 1);
    assert(strcmp(out, "1") == 0);

    json = json_parse("\"1123.1231\"");
    assert(json_string_copy(json, out, sizeof(out)) == 9);
    assert(strcmp(out, "1123.1231") == 0);


    json = json_parse("\"1123.1231\"");
    assert(json_string_copy(json, out, 6) == 9);
    assert(strcmp(out, "1123.") == 0);

    json = json_parse("\"11\\n23.1231\"");
    assert(json_string_copy(json, out, 6) == 10);
    assert(strcmp(out, "11\n23") == 0);

    json = json_parse("\"11\\n23.1231\"");
    assert(json_string_copy(json, out, 64) == 10);
    assert(strcmp(out, "11\n23.1231") == 0);

    json = json_parse("\"");
    assert(json_string_copy(json, out, 64) == 0);
    assert(strcmp(out, "") == 0);


    json = json_parse("\"Example emoji, KO: \\ud83d\\udd13, "
    "\\ud83c\\udfc3 OK: \\u2764\\ufe0f \"");
    assert(json_string_copy(json, out, sizeof(out)) == strlen(exp));
    assert(strcmp(out, exp) == 0);

    json = json_parse("\"Example emoji, KO: \\ud83d\\udd13, "
    "\\ud83c\\udfc3 OK: \\u2764\\ufe0f \"");
    assert(json_string_copy(json, out, sizeof(out)) == strlen(exp));
    assert(strcmp(out, exp) == 0);

    json = json_parse("\"" "1: \\ud83d\\ud83d\\udd13 "
                            "2: \\ud83c\\ud83c\\udfc3 " "\"");
    exp = "1: �� 2: �� ";
    assert(json_string_copy(json, out, sizeof(out)) == strlen(exp));
    assert(strcmp(out, exp) == 0);


    json = json_parse("\" \\u00A3 \"");
    exp = " £ ";
    assert(json_string_copy(json, out, sizeof(out)) == strlen(exp));
    assert(strcmp(out, exp) == 0);


    json = json_parse("\" \\r\\n\\u00A3\\/\\b\\f\\t\\\"\\\\ \"");
    exp = " \r\n£/\b\f\t\"\\ ";
    assert(json_string_copy(json, out, sizeof(out)) == strlen(exp));
    assert(strcmp(out, exp) == 0);
}

void test_json_number(void) {
    assert(json_double(json_parse("123")) == 123.0);
    assert(json_double(json_parse("123.5")) == 123.5);
    double x = json_double(json_parse("9812310293810923810238012" 
                                      "1092830198230912091820398"));
    assert(x > 98123000000000000000000000000000000000000000000000.0 && 
           x < 98124000000000000000000000000000000000000000000000.0);
    char *num = "8172398129831729837192837198237918273918273918723981723987129"
                "3871928371989823749823948273498701384712340586081645038701802"
                "0918273465123480867945245098740914063560156801347860514360809"
                "6784134210837412903847120938462108375612308756123470132874081"
                "0191238746128376197234512376457865786512384765123846123408021"
                "2349812386408172636780123675812678356781234608721683046120834"
                "7890423546739146389756457208961735243452614356178964839043689"
                "9213912801874646754579129010190846859786732678947890345789045"
                "2349239189032598061345680136480568031468905163890457891237894";
    assert(json_double(json_parse(num)) == 0.0);
    assert(json_double(json_parse("\"")) == 0);
    assert(json_double(json_parse("\"123^\"")) == 0);
    assert(json_double(json_parse("null")) == 0);
    assert(json_double(json_parse("true")) == 1);
    assert(json_double(json_parse("false")) == 0);
    assert(json_int64(json_parse("\"")) == 0);
    assert(json_int64(json_parse("\"\"")) == 0);
    assert(json_int64(json_parse("123")) == 123);
    assert(json_int64(json_parse("-123")) == -123);
    assert(json_int64(json_parse("123.5")) == 123);
    assert(json_int64(json_parse("-123.5")) == -123);
    assert(json_int64(json_parse("123.500000000000000000")) == 123);
    assert(json_int64(json_parse("-123.500000000000000000")) == -123);
    assert(json_int64(json_parse("1e10")) == 10000000000);
    assert(json_int64(json_parse("-1e10")) == -10000000000);
    assert(json_int64(json_parse("1e1000")) == INT64_MAX);
    assert(json_int64(json_parse("-1e1000")) == INT64_MIN);
    assert(json_int64(json_parse("null")) == 0);
    assert(json_int64(json_parse("true")) == 1);
    assert(json_int64(json_parse("false")) == 0);
    assert(json_uint64(json_parse("\"")) == 0);
    assert(json_uint64(json_parse("\"\"")) == 0);
    assert(json_uint64(json_parse("123")) == 123);
    assert(json_uint64(json_parse("-123")) == 0);
    assert(json_uint64(json_parse("123.5")) == 123);
    assert(json_uint64(json_parse("-123.5")) == 0);
    assert(json_uint64(json_parse("123.500000000000000000")) == 123);
    assert(json_uint64(json_parse("-123.500000000000000000")) == 0);
    assert(json_uint64(json_parse("1e10")) == 10000000000);
    assert(json_uint64(json_parse("-1e10")) == 0);
    assert(json_uint64(json_parse("1e1000")) == UINT64_MAX);
    assert(json_uint64(json_parse("-1e1000")) == 0);
    assert(json_uint64(json_parse("null")) == 0);
    assert(json_uint64(json_parse("true")) == 1);
    assert(json_uint64(json_parse("false")) == 0);

    assert(json_int(json_parse("19283")) == 19283);
    assert(json_int(json_parse("-19283")) == -19283);
    assert(json_int(json_parse("-91029310293102938102983")) == INT_MIN);
    assert(json_int(json_parse("91029310293102938102983")) == INT_MAX);
}

void test_json_bool(void) {
    assert(json_bool(json_parse("true")) == true);
    assert(json_bool(json_parse("\"true\"")) == true);
    assert(json_bool(json_parse("false")) == false);
    assert(json_bool(json_parse("-1")) == true);
    assert(json_bool(json_parse("0")) == false);
    assert(json_bool(json_parse("0.0")) == false);
    assert(json_bool(json_parse("0.01")) == true);
    assert(json_bool(json_parse("\"\"")) == false);
    assert(json_bool(json_parse("\"True\"")) == true);
    assert(json_bool(json_parse("\"TRUE\"")) == true);
    assert(json_bool(json_parse("\"TrUE\"")) == false);
    assert(json_bool(json_parse("\"1\"")) == true);
    assert(json_bool(json_parse("\"0\"")) == false);
}

void test_json_array(void) {
    assert(json_array_count(json_parse("[]")) == 0);
    assert(json_array_count(json_parse("[1,2,3,4]")) == 4);
    assert(json_array_count(json_parse("{\"1\":\"2\"}")) == 0);
    assert(json_int64(json_array_get(json_parse("[1,2,3,4]"), -1)) == 0);
    assert(json_int64(json_array_get(json_parse("[1,2,3,4]"), 0)) == 1);
    assert(json_int64(json_array_get(json_parse("[1,2,3,4]"), 1)) == 2);
    assert(json_int64(json_array_get(json_parse("[1,2,3,4]"), 2)) == 3);
    assert(json_int64(json_array_get(json_parse("[1,2,3,4]"), 3)) == 4);
    assert(json_int64(json_array_get(json_parse("[1,2,3,4]"), 4)) == 0);
}

void test_json_object(void) {
    char *json_str = readfile("testfiles/twitter.json", 0);
    struct json json;
    json = json_parse(json_str);
    json = json_object_get(json, "statuses");
    json = json_array_get(json, 5);
    assert(json_int64(json_object_get(json, "id")) == 505874918039228416);
    assert(json_int64(json_object_get(json, "id_str")) == 505874918039228416);
    assert(json_int64(json_object_get(json, "na")) == 0);
    free(json_str);
}

void strings_should_match(const char *json_str, const char *exp) {
    struct json json = json_parse(json_str);
    char buf[1024];
    size_t len = json_string_copy(json, buf, sizeof(buf));
    assert(len == strlen(exp));
    assert(strcmp(exp, buf) == 0);
    assert(json_string_length(json) == len);
    assert(json_string_compare(json, exp) == 0);
}

void test_json_utf8(void) {
    // some of these tests are from:
    //  - https://github.com/simdjson/simdjson/blob/master/tests/unicode_tests.cpp
    //  - https://github.com/crossbario/autobahn-testsuite/tree/master/autobahntestsuite/autobahntestsuite/case
    const char *bad[] = {
        "\"\xc3\x28\"",                                 // 0
        "\"\xa0\xa1\"",                                 // 1
        "\"\xe2\x28\xa1\"",                             // 2
        "\"\xe2\x82\x28\"",                             // 3
        "\"\xf0\x28\x8c\xbc\"",                         // 4
        "\"\xf0\x90\x28\xbc\"",                         // 5
        "\"\xf0\x28\x8c\x28\"",                         // 6
        "\"\xc0\x9f\"",                                 // 7
        "\"\xf5\xff\xff\xff\"",                         // 8
        "\"\xed\xa0\x81\"",                             // 9
        "\"\xf8\x90\x80\x80\x80\"",                     // 10
        "\"123456789012345\xed\"",                      // 11
        "\"123456789012345\xf1\"",                      // 12
        "\"123456789012345\xc2\"",                      // 13
        "\"\xC2\x7F\"",                                 // 14
        "\"\xce\"",                                     // 6.6.1
        "\"\xce\xba\xe1\"",                             // 6.6.3
        "\"\xce\xba\xe1\xbd\"",                         // 6.6.4
        "\"\xce\xba\xe1\xbd\xb9\xcf\"",                 // 6.6.6
        "\"\xce\xba\xe1\xbd\xb9\xcf\x83\xce\"",         // 6.6.8
        "\"\xce\xba\xe1\xbd\xb9\xcf\x83\xce\xbc\xce\"", // 6.6.10
        "\"\xdf\"",                                     // 6.14.6
        "\"\xef\xbf\"",                                 // 6.14.7
        "\"\x80\"",
        "\"\x91\x85\x95\x9e\"",
        "\"\x6c\x02\x8e\x18\"",
        "\"\x25\x5b\x6e\x2c\x32\x2c\x5b\x5b\x33\x2c\x34\x2c\x05\x29\x2c\x33\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x5b\x5b\x5b\x5b\x5b\x5b\x5b\x5b\x5b\x5b\x5b\x5b\x5b\x5b\x5b\x5b\x5b\x5d\x2c\x35\x2e\x33\x2c\x39\x2e\x33\x2c\x37\x2e\x33\x2c\x39\x2e\x34\x2c\x37\x2e\x33\x2c\x39\x2e\x33\x2c\x37\x2e\x33\x2c\x39\x2e\x34\x5d\x5d\x5d\x5d\x5d\x5d\x5d\x5d\x5d\x5d\x5d\x5d\x5d\x5d\x5d\x5d\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x20\x01\x01\x01\x01\x01\x02\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x23\x0a\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x7e\x7e\x0a\x0a\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x5b\x5b\x5b\x5b\x5b\x5b\x5b\x5b\x5b\x5b\x5b\x5b\x5b\x5b\x5b\x5b\x5b\x5d\x2c\x37\x2e\x33\x2c\x39\x2e\x33\x2c\x37\x2e\x33\x2c\x39\x2e\x34\x2c\x37\x2e\x33\x2c\x39\x2e\x33\x2c\x37\x2e\x33\x2c\x39\x2e\x34\x5d\x5d\x5d\x5d\x5d\x5d\x5d\x5d\x5d\x5d\x5d\x5d\x5d\x5d\x5d\x01\x01\x80\x01\x01\x01\x79\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\"",
        "\"[[[[[[[[[[[[[[[\x80\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x010\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\"",
        "\"\x20\x0b\x01\x01\x01\x64\x3a\x64\x3a\x64\x3a\x5b\x5b\x5b\x5b\x5b\x5b\x5b\x5b\x5b\x5b\x5b\x5b\x5b\x5b\x5b\x5b\x5b\x5b\x5b\x5b\x5b\x5b\x5b\x5b\x5b\x5b\x5b\x5b\x5b\x5b\x5b\x5b\x5b\x5b\x5b\x5b\x5b\x5b\x5b\x5b\x5b\x30\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x80\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\"",
        "\"\xdf",                                       // end early
        "\"\xf4\x8f\xbf\xbf\"",                         // restricted to utf-16
    };
    const char *good[] = {
        "\"a\"",
        "\"\xc3\xb1\"",
        "\"\xe2\x82\xa1\"",
        "\"\xf0\x90\x8c\xbc\"",
        "\"\xc2\x80\"",         // 6.7.2
        "\"\xf0\x90\x80\x80\"", // 6.7.4
        "\"\xee\x80\x80\"",     // 6.11.2
        "\"\xef\xbb\xbf\"",
    };

    for (size_t i = 0; i < sizeof(good)/sizeof(good[0]); i++) {
        assert(json_valid(good[i]));
    }
    for (size_t i = 0; i < sizeof(bad)/sizeof(bad[0]); i++) {
        assert(!json_valid(bad[i]));
    }
    strings_should_match(
        "\"a\\\\\\/\\b\\f\\n\\r\\t\\\"b\"",
        "a\\/\b\f\n\r\t\"b"
    );
}

void esc_should_match_len_len(char *input, int inputlen, char *exp, int explen) {
    bool limit = true;
    if (explen < 0) {
        limit = false;
        explen = 1024;
    }
    char *got = malloc(explen+1);
    assert(got);
    memset(got, 0, explen+1);
    size_t n;
    if (inputlen == -1) {
        n = json_escape(input, got, explen);
    } else {
        n = json_escapen(input, inputlen, got, explen);
    }
    // printf("exp [%s], got [%s]\n", exp, got);
    if (!limit) {
        assert(n == strlen(exp));
    }
    assert(strcmp(got, exp) == 0);
    free(got);
}

void esc_should_match_len(char *input, int inputlen, char *exp) {
    esc_should_match_len_len(input, inputlen, exp, -1);
}

void esc_should_match(char *input, char *exp) {
    esc_should_match_len(input, -1, exp);
}

void tjson_comp(struct json tjson, struct json tjsonesc) {
    assert(json_type(tjson) == json_type(tjsonesc));
    assert(json_exists(tjson) == json_exists(tjsonesc));
    if (json_type(tjson) == JSON_ARRAY || json_type(tjson) == JSON_OBJECT) {
        struct json val = json_first(tjson);
        struct json valesc = json_first(tjsonesc);
        while (1) {
            assert(json_type(val) == json_type(valesc));
            assert(json_exists(val) == json_exists(valesc));
            if (!json_exists(val)) break;
            tjson_comp(val, valesc);
            val = json_next(val);
            valesc = json_next(valesc);
        }
    } else {
        long tlen = json_string_length(tjson);
        char *cstr = malloc(tlen+1);
        assert(cstr);
        json_string_copy(tjson, cstr, tlen+1);
        long tlenesc = json_string_length(tjsonesc);
        char *cstresc = malloc(tlenesc+1);
        assert(cstresc);
        json_string_copy(tjsonesc, cstresc, tlenesc+1);

        if (tlen == tlenesc) {
        } else {
            printf(">>>>>>>>>%s<<<<<<<<\n>>>>>>>>>%s<<<<<<<<\n", cstr, cstresc);
            assert(tlen == tlenesc);
        }
        free(cstresc);
        free(cstr);
    }
}

void test_json_escape_string(void) {
    esc_should_match("hello", "\"hello\"");
    esc_should_match("", "\"\"");
    esc_should_match("\r\\", "\"\\r\\\\\"");
    esc_should_match("\"\\\n\b\x01\f\r\t/", "\"\\\"\\\\\\n\\b\\u0001\\f\\r\\t/\"");
    esc_should_match_len("\"\\\n\b\x00\f\r\t/", 9, "\"\\\"\\\\\\n\\b\\u0000\\f\\r\\t/\"");
    esc_should_match("abc<>&def", "\"abc\\u003c\\u003e\\u0026def\"");
    esc_should_match("abc(🔓)def", "\"abc(🔓)def\"");
    esc_should_match("abc(❤️)def", "\"abc(❤️)def\"");

    // some invalid input should result in valid JSON, but with the error
    // character.
    esc_should_match("\xc3", "\"�\"");
    esc_should_match("\xc3\xc3", "\"��\"");

    esc_should_match_len_len("hello", -1, "\"hello\"", -1);
    esc_should_match_len_len("hello", -1, "\"h", 3);
    esc_should_match_len_len("hello", -1, "\"", 2);
    esc_should_match_len_len("hello", -1, "", 1);
    esc_should_match_len_len("hello", -1, "", 0);

    // test that every unescaped utf8 string in twitterescaped.json and
    // twitter.json match exactly.
    char *tsrc = readfile("testfiles/twitter.json", 0);
    char *tsrcesc = readfile("testfiles/twitterescaped.json", 0);
    struct json tjson = json_parse(tsrc);
    struct json tjsonesc = json_parse(tsrcesc);
    tjson_comp(tjson, tjsonesc);
    free(tsrcesc);
    free(tsrc);
}

void test_json_get(void) {
    char *str = readfile("testfiles/twitter.json", 0);
    assert(json_string_compare(json_get(str, "statuses.52.id"), 
        "505874877148958721") == 0);
    // json = json_get(json, "statuses", "52a", 0);
    assert(json_string_compare(json_get(str, "statuses.52a.id"), 
        "") == 0);
    assert(json_exists(json_get(str, "statuses")));
    assert(!json_exists(json_get(str, "statuses.52.id.1")));
    assert(!json_exists(json_get(str, "statuses..id")));
    assert(!json_exists(json_get(str, "statuses2")));
    assert(!json_exists(json_get(str, NULL)));
    // printf("%.*s\n", (int)json_raw_length(json), json_raw(json));
    free(str);
    
}

void test_json_max_depth(void) {
    {
        int depth = 1000;
        char *jstr = malloc(depth*2+1);
        assert(jstr);
        for (int i = 0; i < depth;i++){
            jstr[i] = '[';
            jstr[depth+i] = ']';
        }
        jstr[depth*2] = '\0';
        assert(json_valid(jstr));
        free(jstr);
    }
    {
        int depth = 10000;
        char *jstr = malloc(depth*2+1);
        assert(jstr);
        for (int i = 0; i < depth;i++){
            jstr[i] = '[';
            jstr[depth+i] = ']';
        }
        jstr[depth*2] = '\0';
        assert(!json_valid(jstr));
        free(jstr);
    }
}

// long isvalid(const char *str_, long len);


int main(int argc, char **argv) {
    do_test(test_json_parse_testfiles);
    do_test(test_json_parse_various);
    do_test(test_json_valid_big);
    do_test(test_json_valid_invalid);
    do_test(test_json_compare);
    do_test(test_json_iter);
    do_test(test_json_string);
    do_test(test_json_number);
    do_test(test_json_array);
    do_test(test_json_object);
    do_test(test_json_bool);
    do_test(test_json_utf8);
    do_test(test_json_escape_string);
    do_test(test_json_get);
    do_test(test_json_max_depth);
}
