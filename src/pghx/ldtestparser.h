#ifndef LDTESTPARSER_H
#define LDTESTPARSER_H

typedef enum pghx_ld_test_parser_verb
{
    PGHX_LD_TP_VERB_NONE,
    PGHX_LD_TP_VERB_BEGIN,
    PGHX_LD_TP_VERB_INSERT,
    PGHX_LD_TP_VERB_UPDATE,
    PGHX_LD_TP_VERB_DELETE,
    PGHX_LD_TP_VERB_COMMIT,
} pghx_ld_test_parser_verb;

typedef struct pghx_ld_test_event
{
    char *raw;
    pghx_ld_test_parser_verb verb;
    int txid;
    char *table;
    char *schema;
    int size;
    char **keys;
    char **types;
    char **values;
} pghx_ld_test_event;

typedef struct pghx_ld_test_parser
{
    pghx_ld_test_parser_verb verb;
    char *input;
    int start;
    int pos;
    int txid;
    char *schema;
    char *table;
} pghx_ld_test_parser;

int pghx_ld_test_parser_init(pghx_ld_test_parser *p);
pghx_ld_test_event *pghx_ld_test_parser_parse(
        pghx_ld_test_parser *p,
        char *input);
#endif
