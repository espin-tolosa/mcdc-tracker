/*
 * MC/DC library
 *
 * 1. mcdc_alloc_table  : allocates an mc/dc table
 * 2. mcdc_register_row : stablish the conditions of one row of a mc/dc table
 * 3. mcdc_check_row    : evaluate the conditions to set a row
 *
 * USAGE:
 *
 * define an mcdc table like this:
 *
 * | ROW | cond1 | cond2 |
 * |=====|=======|=======|
 * |  0  | FALSE | FALSE |
 * |  1  | TRUE  | FALSE |
 * |  2  | FALSE | TRUE  |
 * |=====|=======|=======|
 *
 * Inside strncmp, try to add the result of a test of MC/DC
 *
 * mcdc_set_row( table, {TRUE,  TRUE } ); <- this case won't match any row so it will be discarded
 * mcdc_set_row( table, {FALSE, FALSE} ); <- this case will set to checked the row 1 of the table
 *
 * ------------------------------------------------------------------------------------------------
 * Challenge: mcdc table has to be accessed in two places:
 *
 * - the function under test, example: strncmp
 * - the mcdc function to check table is completed
 *
 */

#include <malloc.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <stdbool.h>

#include "mcdc.h"

static mcdc_table_size_t m_nrows = 0;

mcdc_context_t m_mcdc_context = { 0 };

static void    try_mcdc_alloc_table(void);
static void    try_mcdc_alloc_checks(void);

/*
 * PUBLIC MEMBERS
 */

extern void mcdc_register_row ( mcdc_cond_t * conds )
{
    mcdc_table_t this = m_mcdc_context.list[ m_mcdc_context.len - 1 ];

    memcpy( &this.table[ m_nrows*this.cols ], conds, this.cols );

    m_nrows++;
}

extern void mcdc_check_row ( mcdc_table_size_t at_table, mcdc_row_t row )
{
    mcdc_table_t this = m_mcdc_context.list[ at_table ];

    for( mcdc_table_size_t i = 0; i < this.rows; i++ )
    {
        if ( memcmp( &this.table[ i*this.cols ], row, this.cols ) == 0 )
        {
            this.checks[ i ] = true;
            break;
        }
    }
}

extern void mcdc_check_mcdc( void )
{
    for( mcdc_table_size_t at_table = 0; at_table < m_mcdc_context.len; at_table++ )
    {
        mcdc_table_t this = m_mcdc_context.list[ at_table ];

        mcdc_cond_t all_passed = true;

        mcdc_table_size_t checks_passed = 0;

        for(mcdc_table_size_t row = 0; row < this.rows; row++ )
        {
            if ( this.checks[ row ] == true )
            {
                checks_passed++;
            }
        }

        all_passed = checks_passed == this.rows;

        printf("====================================================\n MC/DC %u x %u %39s\n- - - - - - - - - - - - - - - - - - - - - - - - - -\n %s\n====================================================\n", checks_passed, this.rows, all_passed?"PASS":"FAIL", this.name);

        for(mcdc_table_size_t row = 0; row < this.rows; row++ )
        {
            printf(" Test No. %u | ", 1 + row);
            for(mcdc_table_size_t col = 0; col < this.cols; col++ )
            {
                printf("%s ", this.table[row*this.cols+col] ? "T":"F");
            }

            printf("| %s\n", this.checks[row]?"Checked":"Unchecked");
        }
    }
}

/*
 * PRIVATE MEMBERS
 */

static void try( void (*func) (void), const char * msg )
{
    int saved_err = errno;
    errno = 0;

    func();

    if( errno != 0 )
    {
        fprintf(stderr, "[ERROR] %s\n", msg);
    }

    errno = saved_err;
}

void * m_ret_malloc = NULL;
unsigned int m_rows = 0;
unsigned int m_cols = 0;

static void try_mcdc_alloc_table(void)
{
    m_ret_malloc = calloc( m_rows * m_cols , sizeof(mcdc_cond_t) );
    m_nrows = 0;
}

static void try_mcdc_alloc_checks(void)
{
    m_ret_malloc = calloc( m_rows , sizeof(mcdc_cond_t) );
}

extern void mcdc_alloc_table( mcdc_table_size_t rows, mcdc_table_size_t cols, const char * name )
{
    mcdc_row_t checks = NULL;
    mcdc_table_handle_t table = NULL;

    m_rows = rows;
    m_cols = cols;

    try( try_mcdc_alloc_checks, "MC/DC checks allocation" );

    if( m_ret_malloc != NULL )
    {
        checks = (mcdc_row_t) m_ret_malloc;
    }

    try( try_mcdc_alloc_table, "MC/DC table allocation" );

    if( m_ret_malloc != NULL )
    {
        table = (mcdc_table_handle_t) m_ret_malloc;
    }

    else
    {
        /* NOP: table is already set to NULL, and this case will be checked in the next step */
    }

    /* regrow context */
    if( checks != NULL && table != NULL )
    {
        if( m_mcdc_context.list == NULL )
        {
            m_mcdc_context.list = calloc( 1, sizeof(mcdc_table_t) );
        }
        else
        {
            m_mcdc_context.list = realloc( m_mcdc_context.list, (1 + m_mcdc_context.len) * sizeof(mcdc_table_t) );
            memset( &m_mcdc_context.list[ m_mcdc_context.len + 1 ], 0, sizeof(mcdc_table_size_t) );
        }

        m_mcdc_context.list [ m_mcdc_context.len ].table    = table;
        m_mcdc_context.list [ m_mcdc_context.len ].checks   = checks;
        m_mcdc_context.list [ m_mcdc_context.len ].rows     = rows;
        m_mcdc_context.list [ m_mcdc_context.len ].cols     = cols;
        m_mcdc_context.list [ m_mcdc_context.len ].name     = name;

        m_mcdc_context.len++;
    }

    else
    {
        /* NOP: not planned to do nothing yet in the error condition of `checks`, `table` or both are NULL */
    }
}
