#include <stdlib.h>
#include <check.h>
#include "../src/pghx/logicaldecoding.h"
#include "../src/pghx/errors.h"
#include "../src/pghx/ldtestparser.h"

START_TEST(test_ld_reader_init)
{
    pghx_ld_reader r;
    pghx_ld_reader *pr = &r;
    int res;

    res = pghx_ld_reader_init(pr);

    ck_assert_int_eq(res, 1);
    ck_assert_int_eq(pr->last_status, -1);
    ck_assert_str_eq(pr->slot, "test_slot");

    //free(r);
}
END_TEST

// TODO: more tests...
START_TEST(error_h)
{
    pghx_error_info[0] = "new_error";
}
END_TEST

#define FIELDS_NR 3

START_TEST(test_ld_test_parser)
{
    pghx_ld_test_parser p;
    pghx_ld_test_parser *pp = &p;
    pghx_ld_test_event *e;
    char *input;
    char *keys[FIELDS_NR];
    char *types[FIELDS_NR];
    char *values[FIELDS_NR];

    int i;

    pghx_ld_test_parser_init(pp);

    input = strdup("BEGIN 1");
    e = pghx_ld_test_parser_parse(pp, input);
    if (e == NULL)
        ck_abort_msg(pghx_error_info[0]);
    ck_assert_int_eq(e->verb, PGHX_LD_TP_VERB_BEGIN);
    ck_assert_int_eq(e->txid, 1);
    free(input);

    input = strdup(
            "table public.data: INSERT: key_0[type_0]:value_0 key_1[type_1]:'value ''1'''");
    e = pghx_ld_test_parser_parse(pp, input);
    if (e == NULL)
        ck_abort_msg(pghx_error_info[0]);
    ck_assert_str_eq(e->schema, "public");
    ck_assert_str_eq(e->table, "data");
    ck_assert_int_eq(e->txid, 1);
    ck_assert_int_eq(e->verb, PGHX_LD_TP_VERB_INSERT);
    ck_assert_str_eq(e->keys[0], "key_0");
    ck_assert_str_eq(e->keys[1], "key_1");
    ck_assert_str_eq(e->types[0], "type_0");
    ck_assert_str_eq(e->types[1], "type_1");
    ck_assert_str_eq(e->values[0], "value_0");
    ck_assert_str_eq(e->values[1], "'value ''1'''");
    free(input);

    input = strdup("COMMIT 1");
    e = pghx_ld_test_parser_parse(pp, input);
    if (e == NULL)
        ck_abort_msg(pghx_error_info);
    ck_assert_int_eq(e->verb, PGHX_LD_TP_VERB_COMMIT);
    ck_assert_int_eq(e->txid, 1);
    free(input);

    input = strdup("BEGIN 2");
    e = pghx_ld_test_parser_parse(pp, input);
    if (e == NULL)
        ck_abort_msg(pghx_error_info[0]);
    ck_assert_int_eq(pp->verb, PGHX_LD_TP_VERB_BEGIN);
    ck_assert_int_eq(pp->txid, 2);
    free(input);

    input = strdup(
            "table public2.data2: INSERT: key_0[type_0]:'value ''0''' key_1[type_1]:value_1");
    e = pghx_ld_test_parser_parse(pp, input);
    if (e == NULL)
        ck_abort_msg(pghx_error_info);
    ck_assert_int_eq(e->txid, 2);
    ck_assert_str_eq(e->schema, "public2");
    ck_assert_str_eq(e->table, "data2");
    ck_assert_int_eq(e->verb, PGHX_LD_TP_VERB_INSERT);
    for(i=0; i<FIELDS_NR; i++)
    {
         if (i == 0)
         {
            ck_assert_str_eq(e->keys[i], "key_0");
            ck_assert_str_eq(e->types[i], "type_0");
            ck_assert_str_eq(e->values[i], "'value ''0'''");
            continue;
         }
         if (i == 1)
         {
            ck_assert_str_eq(e->keys[i], "key_1");
            ck_assert_str_eq(e->types[i], "type_1");
            ck_assert_str_eq(e->values[i], "value_1");
            continue;
         }
         // check that the arrays are null terminated
         if (e->keys[i] == NULL && e->types[i] == NULL && e->values[i] == NULL)
             break;
         // we should never reach this line
         ck_assert_int_eq(1,0);
    }
    free(input);
    ck_abort_msg("ciiocou");

    input = strdup(
            "table public.data: UPDATE: key_0[type_0]:value_0 key_1[type_1]:'value ''1'''");
    e = pghx_ld_test_parser_parse(pp, input);
    if (e == NULL)
        ck_abort_msg(pghx_error_info);
    ck_assert_int_eq(e->txid, 2);
    ck_assert_str_eq(e->schema, "public");
    ck_assert_str_eq(e->table, "data");
    ck_assert_int_eq(e->verb, PGHX_LD_TP_VERB_UPDATE);
    ck_assert_str_eq(e->keys[0], "key_0");
    ck_assert_str_eq(e->keys[1], "key_1");
    ck_assert_str_eq(e->types[0], "type_0");
    ck_assert_str_eq(e->types[1], "type_1");
    ck_assert_str_eq(e->values[0], "value_0");
    ck_assert_str_eq(e->values[1], "'value ''1'''");
    free(input);

    input = strdup(
            "table public.data: DELETE: id[integer]:42");
    e = pghx_ld_test_parser_parse(pp, input);
    if (e == NULL)
        ck_abort_msg(pghx_error_info);
    ck_assert_int_eq(e->txid, 2);
    ck_assert_str_eq(e->schema, "public");
    ck_assert_str_eq(e->table, "data");
    ck_assert_int_eq(e->verb, PGHX_LD_TP_VERB_DELETE);
    ck_assert_str_eq(e->keys[0], "id");
    ck_assert_str_eq(e->types[0], "integer");
    ck_assert_str_eq(e->values[0], "42");
    free(input);
}
END_TEST

Suite * pghx_suite(void)
{
    Suite *s;
    TCase *tc_core;

    s = suite_create("PGHX");

    /* Core test case */
    tc_core = tcase_create("Core");

    tcase_add_test(tc_core, test_ld_reader_init);
    tcase_add_test(tc_core, error_h);
    tcase_add_test(tc_core, test_ld_test_parser);
    suite_add_tcase(s, tc_core);

    return s;
}

int main(void)
{
    int number_failed;
    Suite *s;
    SRunner *sr;

    s = pghx_suite();
    sr = srunner_create(s);

    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
