#include <stdlib.h>
#include <stdio.h>
#include "ldtestparser.h"
#include "errors.h"

int pghx_ld_test_parser_init(pghx_ld_test_parser *p){
    p->txid = 0;
}

int pghx_ld_test_parser_reinit(pghx_ld_test_parser *p){
    p->verb = PGHX_LD_TP_VERB_NONE;
    p->input = NULL;
    p->start = 0;
    p->pos = 0;
    p->schema = NULL;
    p->table = NULL;
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
    return -1;
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
    p->pos += length;
    if(length == -1)
        return p->input + p->start;
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
    if (length == -1)
        goto error;
    p->pos += length;
    p->schema = parser_emit(p);
    length = find_char(p->input + p->start, ':');
    if (length == -1)
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
    if (length == -1)
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
    if (length == -1)
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
    if (length == -1)
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
    if (length == -1)
    {
        if(p->input[p->start] != '\0')
            return p->input + p->start;
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


int pghx_ld_test_parser_parse(
        pghx_ld_test_parser *p,
        char *input,
        char *keys[],
        char *types[],
        char *values[])
{
    int length;

    pghx_ld_test_parser_reinit(p);
    p->input = input;
    length = startswith(p->input, "BEGIN ");
    if(length != -1)
    {
        p->verb = PGHX_LD_TP_VERB_BEGIN;
        p->start = p->pos = length;
        return parser_parse_txid(p);
    }
    length = startswith(p->input, "COMMIT ");
    if(length != -1)
    {
        p->verb = PGHX_LD_TP_VERB_COMMIT;
        p->start = p->pos = length;
        return parser_parse_txid(p);
    }
    length = startswith(p->input, "table ");
    if(length != -1)
    {
        int i = 0;
        char *txt;
        p->start = p->pos = length;
        if (!parser_parse_table(p))
            return 0;
        p->start++;
        p->pos++;
        if (!parser_parse_action(p))
            return 0;
        while(p->pos)
        {
            txt = parser_parse_field_name(p);
            if(!txt)
                return 0;
            keys[i] = txt;
            txt = parser_parse_field_type(p);
            if(!txt)
                return 0;
            types[i] = txt;
            p->pos++;
            p->start++;
            txt = parser_parse_field_value(p);
            if(!txt)
                return 0;
            values[i] = txt;
            i++;
        }
        keys[i] = NULL;
        types[i] = NULL;
        values[i] = NULL;
        return 1;
    }
    else
    {
        Pghx_format_error(PGHX_LD_PARSE_ERROR,
                "Expected 'table' at char %i in line `%s`\n",
                p->start,
                p->input);
        return 0;
    }
}

