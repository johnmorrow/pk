#include <unistd.h>
#include <stdlib.h>

#include "command_line.h"

#define CTEST_MAIN
#include "ctest.h"

CTEST(suite1, create_object)
{
    COMMAND_LINE *command_line = command_line_new();
    ASSERT_NOT_NULL(command_line);
    command_line_delete(command_line);
}

CTEST(suite1, create_object_add_flag)
{
    COMMAND_LINE *command_line = command_line_new();
    ASSERT_NOT_NULL(command_line);
    command_line_add_flag(command_line,
        't', "test", "Test flag");
    ASSERT_NOT_NULL(command_line);
    command_line_delete(command_line);
}

CTEST(suite1, create_object_parse_empty)
{
    COMMAND_LINE *command_line = command_line_new();
    ASSERT_NOT_NULL(command_line);
    int argc = 0;
    const char *argv[] = {0};
    command_line_parse(command_line, argc, argv);
    command_line_delete(command_line);
}

CTEST_DATA(suite2)
{
    COMMAND_LINE *command_line;
};

CTEST_SETUP(suite2) {
    data->command_line = command_line_new();
    command_line_add_flag(data->command_line,
        't', "test", "Test flag");
    int argc = 1;
    const char *argv[] = {"-t"};
    command_line_parse(data->command_line, argc, argv);
}

CTEST2(suite2, test1)
{
    ASSERT_TRUE(command_line_check_flag(data->command_line, 't'));
}

CTEST2(suite2, test2)
{
    ASSERT_FALSE(command_line_check_flag(data->command_line, 'f'));
}

CTEST_TEARDOWN(suite2) {
    command_line_delete(data->command_line);
}

int main(int argc, const char *argv[])
{
    return ctest_main(argc, argv); 
}
