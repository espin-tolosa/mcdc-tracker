#include <mcdc.h>

int spt_strncmp(const char *str1, const char *str2, unsigned int n)
{
    int result = 0;

    if (str1 == (void*)0 || str2 == (void*)0)
    {
        result = 1;
    }

    unsigned int i = 0;
    while ( i < n && str1[i] != '\0' && str1[i] == str2[i] )
    {
        MCDC_CHECK( 0, i < n, str1[i] != '\0', str1[i] == str2[i] );
        i++;
    }

    MCDC_CHECK( 0, i < n, str1[i] != '\0', str1[i] == str2[i] );

    if( i == n )
    {
        MCDC_CHECK( 1, i == n );
        result = 0;
    }
    else
    {
        MCDC_CHECK( 1, i == n );
        result = (int)str1[i] - (int)str2[i];
    }

    return result;
}

#define TEST_STRNCMP
#ifdef TEST_STRNCMP

#include <stdio.h>

void test_strncmp( void );

int main( void )
{
    test_strncmp();

    mcdc_check_mcdc( );

    return 0;
}

void test_strncmp( void )
{
    struct test
    {
        const char *str1;
        const char *str2;
        unsigned int n;
        int expected;
    };

    struct test tests[] =
    {
        {"apple", "applee", 5, 0 },
        {"apples", "applee", 4, 0 },
        {"ap\0les", "ap\0lee", 5, 0 },
        {"hellos", "hellx", 5, 'o'-'x'},
    };

    mcdc_alloc_table( 4, 3, "i < n && str1[i] != \\0 && str1[i] == str2[i]" );
    MCDC_REGISTER( 0, 1, 1 );
    MCDC_REGISTER( 1, 0, 1 );
    MCDC_REGISTER( 1, 1, 0 );
    MCDC_REGISTER( 1, 1, 1 );

    mcdc_alloc_table( 2, 1, "i == n" );
    MCDC_REGISTER( 0 );
    MCDC_REGISTER( 1 );

    int num_tests = sizeof(tests) / sizeof(tests[0]);
    int num_pass = 0;
    for (int i = 0; i < num_tests; i++)
    {
        int result = spt_strncmp(tests[i].str1, tests[i].str2, tests[i].n);
        if (result == tests[i].expected)
        {
            num_pass++;
        }

        else
        {
            printf("Test %d failed: strncmp(\"%s\", \"%s\", %u) = %d, expected %d\n",
            i + 1, tests[i].str1, tests[i].str2, tests[i].n, result, tests[i].expected);
        }
    }

    if(num_pass== num_tests){ printf("All test passed\n"); }
}

#endif
