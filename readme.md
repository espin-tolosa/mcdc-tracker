# MC/DC Tracker

A tool to track the MC/DC (Modified Condition/Decision Coverage) achieved in your tests.
The tool is currently very lightweight and does not yet include a compilation configuration.
This version is considered a proof of concept, which has proven to be very useful, though the MC/DC cases must be manually introduced. The roadmap for this tool includes automating this process by lexing and parsing a boolean expression to extract the required conditions.

The demo provides a report in the form of a table, listing the coverage of all defined cases.

## Demo Example

Inside a possible `strncmp` implementation (see Figure 1), there is a loop that is tested using the `MCDC_CHECK` tool.

```c
/* Figure 1: MC/DC test of stop conditions in the loop of a strncmp implementation */

unsigned int i = 0;

while ( ( i < n ) && ( str1[i] != '\0' ) && ( str1[i] == str2[i] ) )
{
    MCDC_CHECK( 0, i < n, str1[i] != '\0', str1[i] == str2[i] );
    i++;
}

MCDC_CHECK( 0, i < n, str1[i] != '\0', str1[i] == str2[i] );
```

The stop condition of the loop consists of three concatenated `AND` conditions. A previous manual analysis determined that there are four test cases to consider (see Figure 2). Here is a summary of the manual analysis:

- Tests 1 and 4 demonstrate that condition 1 can independently affect the outcome (`i < n`).
- Tests 2 and 4 demonstrate that condition 2 can independently affect the outcome (`str1[i] != '\0'`).
- Tests 3 and 4 demonstrate that condition 3 can independently affect the outcome (`str1[i] == str2[i]`).

These cases are manually registered within the test, and the tool keeps track of them.

In this demo version, the developer must manually instrument the code by placing calls to `MCDC_CHECK` in each possible branch to allow the tool to track each branch decision.

This process is cumbersome, so the goal is to parse and instrument the code automatically. The tool is designed with portability in mind, avoiding compiler-specific extensions such as `.gcno` and `.gcda` from GCC.

```c
/* Figure 2: Registering four test cases */

mcdc_alloc_table( 4, 3, "i < n && str1[i] != \0 && str1[i] == str2[i]" );
MCDC_REGISTER( 0, 1, 1 ); /* Test 1 */
MCDC_REGISTER( 1, 0, 1 ); /* Test 2 */
MCDC_REGISTER( 1, 1, 0 ); /* Test 3 */
MCDC_REGISTER( 1, 1, 1 ); /* Test 4 */
```

The test results are presented in a report:

```bash
====================================================
 MC/DC 4 x 4                                    PASS
- - - - - - - - - - - - - - - - - - - - - - - - - -
 i < n && str1[i] != \0 && str1[i] == str2[i]
====================================================
 Test No. 1 | F T T | Checked
 Test No. 2 | T F T | Checked
 Test No. 3 | T T F | Checked
 Test No. 4 | T T T | Checked
====================================================
 MC/DC 2 x 2                                    PASS
- - - - - - - - - - - - - - - - - - - - - - - - - -
 i == n
====================================================
 Test No. 1 | F | Checked
 Test No. 2 | T | Checked
```

# Future Features

## 1. Integrate Testing Directly into the Code Under Test

This feature will enable the tool to lex and parse MC/DC conditions automatically,
allowing to remove calls to `MCDC_CHECK` in each branch like in demo version (see Figure 1.)

```c
/* Figure 3: MCDC_CHECK is able to track all the decisions from a single point */

while ( MCDC_CHECK( i < n && str1[i]!='\0' && str1[i] == str2[i] ) )
{
    i++;
}
```
