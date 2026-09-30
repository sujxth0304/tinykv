#include <string.h>

#include "unity.h"

#include "tinykv/tinykv.h"

void setUp(void)
{
}
void tearDown(void)
{
}

static void test_version_is_reported(void)
{
    const char *version = tkv_version();
    TEST_ASSERT_NOT_NULL(version);
    TEST_ASSERT_EQUAL_STRING("0.1.0-dev", version);
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_version_is_reported);
    return UNITY_END();
}
