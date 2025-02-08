#ifndef MCDC_H
#define MCDC_H

typedef _Bool           mcdc_cond_t;
typedef _Bool *         mcdc_row_t;
typedef _Bool *         mcdc_table_handle_t;
typedef unsigned int    mcdc_table_size_t;

typedef struct
{
    const char *            name;
    mcdc_table_handle_t     table;
    mcdc_row_t              checks;
    mcdc_table_size_t       rows;
    mcdc_table_size_t       cols;
} mcdc_table_t;

typedef struct
{
    mcdc_table_t * list;
    unsigned int   len;
} mcdc_context_t;

extern void mcdc_alloc_table    ( mcdc_table_size_t rows, mcdc_table_size_t cols, const char * name );
extern void mcdc_register_row   ( mcdc_cond_t * conds );
extern void mcdc_check_row      ( mcdc_table_size_t at_table, mcdc_row_t row );
extern void mcdc_check_mcdc     ( void );

#define MCDC_CHECK(at_table,...)    mcdc_check_row( at_table, (mcdc_cond_t[]){ __VA_ARGS__ } )
#define MCDC_REGISTER(...)          mcdc_register_row( (mcdc_cond_t[]){ __VA_ARGS__ } )

#endif
