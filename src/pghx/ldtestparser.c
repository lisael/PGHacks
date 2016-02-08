#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "ldtestparser.h"
#include "errors.h"
#include "utils.h"

int pghx_ld_test_parser_init(pghx_ld_test_parser *p){
    p->txid = 0;
    return 1;
}

int pghx_ld_test_parser_reinit(pghx_ld_test_parser *p){
    p->verb = PGHX_LD_TP_VERB_NONE;
    p->input = NULL;
    p->start = 0;
    p->pos = 0;
    p->schema = NULL;
    p->table = NULL;
    return 1;
}


int startswith(char *str, char *needle)
{
    int i = 0;
    char c;

    c = needle[i];
    while(c)
    {
        if (c != str[i])
            return -1;
        i++;
        c = needle[i];
    }
    // reached the end of needle
    return i;
}


int find_char(char *str, char needle)
{
    int i = 0;
    char c = str[0];

    while(c)
    {
        if (c == needle){
            return i;
        }
        i++;
        c = str[i];
    }
    // reached the end of string
    return 0 - i;
}


char *parser_emit(pghx_ld_test_parser *p)
{
    char *result;

    result = p->input + p->start;
    if(p->input[p->pos] != '\0')
    {
        p->input[p->pos] = '\0';
        p->pos++;
    }
    p->start = p->pos;
    return result;
}

char *parser_get_word(pghx_ld_test_parser *p)
{
    int length;
    length = find_char(p->input + p->start, ' ');
    if(length < 0)
    {
        p->pos -= length;
    }
    else
        p->pos += length;
    return parser_emit(p);
}

int parser_parse_txid(pghx_ld_test_parser *p)
{
    char *strid = parser_get_word(p);
    // TODO: check errors
    p->txid = atoi(strid);
    return 1;
}

int parser_parse_table(pghx_ld_test_parser *p)
{
    int length;

    length = find_char(p->input + p->start, '.');
    if (length <= -1)
        goto error;
    p->pos += length;
    p->schema = parser_emit(p);
    length = find_char(p->input + p->start, ':');
    if (length <= -1)
        goto error;
    p->pos += length;
    p->table = parser_emit(p);
    return 1;
error:
    Pghx_format_error(PGHX_LD_PARSE_ERROR,
            "Can't parse table name at char %i in line `%s`\n",
            p->start,
            p->input);
    return 0;
}

char *parser_parse_field_name(pghx_ld_test_parser *p)
{
    int length;

    length = find_char(p->input + p->start, '[');
    if (length <= -1)
    {
        Pghx_format_error(PGHX_LD_PARSE_ERROR,
                "Can't parse field name at char %i in line `%s`\n",
                p->start,
                p->input);
        return NULL;
    }
    p->pos += length;
    return parser_emit(p);
}

char *parser_parse_field_type(pghx_ld_test_parser *p)
{
    int length;

    length = find_char(p->input + p->start, ']');
    if (length <= -1)
    {
        Pghx_format_error(PGHX_LD_PARSE_ERROR,
                "Can't parse field type at char %i in line `%s`\n",
                p->start,
                p->input);
        return NULL;
    }
    p->pos += length;
    return parser_emit(p);
}

char *parser_parse_text_value(pghx_ld_test_parser *p)
{
    int length;

    p->pos++;
    length = find_char(p->input + p->pos, '\'');
    if (length <= -1)
        goto error;
    if (length == 1)
        goto emit;
    p->pos += length;
    for(;;)
    {
        if (p->input[p->pos + 1] == '\'')
        {
            p->pos += 2 ;
            length = find_char(p->input + p->pos, '\'');
            if (length <= -1)
                goto error;
            p->pos += length;
            continue;
        }
        if (length == -1)
            goto error;
        goto emit;
    }

emit:
    p->pos++;
    return parser_emit(p);
error:
    Pghx_format_error(PGHX_LD_PARSE_ERROR,
            "Can't parse field value at char %i in line `%s`\n",
            p->start,
            p->input);
    return NULL;
}

char *parser_parse_field_value(pghx_ld_test_parser *p)
{
    int length;

    if(p->input[p->start] == '\'')
        return parser_parse_text_value(p);

    length = find_char(p->input + p->start, ' ');
    if (length <= -1)
    {
        if(p->input[p->start] != '\0')
        {
            p->pos -= length;
            return parser_emit(p);
        }
        Pghx_format_error(PGHX_LD_PARSE_ERROR,
            "Can't parse field value at char %i in line `%s`\n",
            p->start,
            p->input);
        return NULL;
    }
    p->pos += length;
    return parser_emit(p);
}

int parser_parse_action(pghx_ld_test_parser *p)
{
    int length;

    length = startswith(p->input + p->start, "INSERT: ");
    if (length != -1)
    {
        p->verb = PGHX_LD_TP_VERB_INSERT;
        p->pos += length;
        p->start += length;
        return 1;
    }
    length = startswith(p->input + p->start, "UPDATE: ");
    if (length != -1)
    {
        p->verb = PGHX_LD_TP_VERB_UPDATE;
        p->pos += length;
        p->start += length;
        return 1;
    }
    length = startswith(p->input + p->start, "DELETE: ");
    if (length != -1)
    {
        p->verb = PGHX_LD_TP_VERB_DELETE;
        p->pos += length;
        p->start += length;
        return 1;
    }
    Pghx_format_error(PGHX_LD_PARSE_ERROR,
            "Can't parse action at char %i in line `%s`\n",
            p->start,
            p->input);
    return 0;
}

int pghx_ld_test_event_extend(pghx_ld_test_event *ev)
{
    char **new_keys;
    char **new_types;
    char **new_values;

    ev->size += 10;

    new_keys = realloc(ev->keys, sizeof(char *) * ev->size+2);
    if(new_keys == NULL)
        goto error;
    new_types = realloc(ev->types, sizeof(char *) * ev->size+2);
    if(new_types == NULL)
        goto error;
    new_values = realloc(ev->values, sizeof(char *) * ev->size+2);
    if(new_values == NULL)
        goto error;
    ev->keys = new_keys;
    ev->types = new_types;
    ev->values = new_values;

    return 1;
error:
    Pghx_set_error(PGHX_OUT_OF_MEMORY, "Could not extend field arrays\n");
    return 0;
}

pghx_ld_test_event *pghx_ld_test_event_new()
{
    pghx_ld_test_event *new;

    new = malloc(sizeof(pghx_ld_test_event));
    if(!new){
        Pghx_set_error(PGHX_OUT_OF_MEMORY, "Could not create test_event\n");
        return NULL;
    }
    new->size = 0;
    new->keys = new->types = new->values = NULL;
    if (!pghx_ld_test_event_extend(new))
    {
        return NULL;
    }
    return new;
}

pghx_ld_test_event *pghx_ld_test_parser_parse(
        pghx_ld_test_parser *p,
        char *input)
{
    int length;
    pghx_ld_test_event *result;

    result = pghx_ld_test_event_new();
    if(!result)
    {
        return NULL;
    }

    pghx_ld_test_parser_reinit(p);
    p->input = result->raw = input;

    // check the type of event (transaction or changes)
    length = startswith(p->input, "BEGIN ");
    if(length != -1)
    {
        p->verb = result->verb = PGHX_LD_TP_VERB_BEGIN;
        p->start = p->pos = length;
        goto tx_return;
    }
    length = startswith(p->input, "COMMIT ");
    if(length != -1)
    {
        p->verb = result->verb = PGHX_LD_TP_VERB_COMMIT;
        p->start = p->pos = length;
        goto tx_return;
    }

    // it's not a transaction action. Get the txid from the parser itself
    result->txid = p->txid;

    // .. so, this must be data change
    length = startswith(p->input, "table ");
    if(length != -1)
    {
        int i = 0;
        char expect_new = 0;
        char *txt;

        // consume "table "
        p->start = p->pos = length;

        // read table schema and name
        if (!parser_parse_table(p))
            return NULL;

        p->start++;
        p->pos++;

        // read the action
        if (!parser_parse_action(p))
            return NULL;
        result->verb = p->verb;
        result->table = p->table;
        result->schema = p->schema;

        // parse the fields
        while(p->input[p->pos])
        {
            // extend field arrays if needed
            if (i >= result->size)
            {
                if (!pghx_ld_test_event_extend(result))
                    return NULL;
            }

            // extract field name
            txt = parser_parse_field_name(p);
            if(!txt)
                return NULL;
            // UPDATE lines may be more complex in case of REPLICA IDENTITY FULL
            if (result->verb == PGHX_LD_TP_VERB_UPDATE)
            {
                length = startswith(txt, "old-key: ");
                if (length != -1)
                {
                    // strip "old-key: " from the fieldname
                    txt = txt + length;
                    expect_new = 1;
                    goto done;
                }
                if (expect_new)
                {
                    length = startswith(txt, "new_tuple: ");
                    if (length != -1)
                    {
                        txt = txt + length;
                        // no need to check anymore
                        expect_new = 0;
                        // add a NULL separator in the arrays
                        result->keys[i] = result->types[i] = result->values[i] = NULL;
                        i++;
                        if (i >= result->size)
                        {
                            if (!pghx_ld_test_event_extend(result))
                                return NULL;
                        }
                    }
                }
                done:
                    ;;
            }
            result->keys[i] = txt;

            // well... parse field type
            txt = parser_parse_field_type(p);
            if(!txt)
                return NULL;
            result->types[i] = txt;
            p->pos++;
            p->start++;

            // useless comment
            // parse field value
            txt = parser_parse_field_value(p);
            if(!txt)
                return NULL;
            result->values[i] = txt;
            i++;
        }

        // add two null values (one NULL => old/new separator)
        result->keys[i] = result->types[i] = result->values[i] = NULL;
        i++;
        result->keys[i] = result->types[i] = result->values[i] = NULL;
        return result;
    }
    else
    {
        Pghx_format_error(PGHX_LD_PARSE_ERROR,
                "Expected 'table' at char %i in line `%s`\n",
                p->start,
                p->input);
        return NULL;
    }
tx_return:
    if (!parser_parse_txid(p))
        return 0;
    result->txid = p->txid;
    return result;
}

void pghx_ld_test_event_free(pghx_ld_test_event *e)
{
     free(e->keys);
     free(e->types);
     free(e->values);
     free(e);
}
