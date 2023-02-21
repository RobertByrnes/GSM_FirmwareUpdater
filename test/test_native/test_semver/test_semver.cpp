#include <unity.h>
#include <semver.hpp>

std::string version1;
std::string version2;
std::string version3;
std::string version4;
std::string version5;
std::string version6;
std::string version7;
std::string version8;

void setUp(void)
{
    version1 = "1.0.1";
    version2 = "1.0.2";
    version3 = "1.0.21";
    version4 = "1.1.21";
    version5 = "1.2.21";
    version6 = "2.0.0";
    version7 = "2.12.34";
    version8 = "2.13.34";
}

void tearDown(void) {}

void testVersion2GreaterThanVersion1()
{
    TEST_ASSERT_EQUAL(-1, Semver::versionCompare(version1, version2));
}

void testVersion3GreaterThanVersion2()
{
    TEST_ASSERT_EQUAL(-1, Semver::versionCompare(version2, version3));
}

void testVersion4GreaterThanVersion3()
{
    TEST_ASSERT_EQUAL(-1, Semver::versionCompare(version3, version4));
}

void testVersion5GreaterThanVersion4()
{
    TEST_ASSERT_EQUAL(-1, Semver::versionCompare(version4, version5));
}

void testVersion6GreaterThanVersion5()
{
    TEST_ASSERT_EQUAL(-1, Semver::versionCompare(version5, version6));
}

void testVersion7GreaterThanVersion6()
{
    TEST_ASSERT_EQUAL(-1, Semver::versionCompare(version6, version7));
}

void testVersion8GreaterThanVersion7()
{
    TEST_ASSERT_EQUAL(-1, Semver::versionCompare(version7, version8));
}

void testVersion1EqualsVersion1()
{
    TEST_ASSERT_EQUAL(0, Semver::versionCompare(version1, version1));
}

void testVersion2EqualsVersion2()
{
    TEST_ASSERT_EQUAL(0, Semver::versionCompare(version2, version2));
}

void testVersion3EqualsVersion3()
{
    TEST_ASSERT_EQUAL(0, Semver::versionCompare(version3, version3));
}

void testVersion4EqualsVersion4()
{
    TEST_ASSERT_EQUAL(0, Semver::versionCompare(version4, version4));
}

void testVersion5EqualsVersion5()
{
    TEST_ASSERT_EQUAL(0, Semver::versionCompare(version5, version5));
}

void testVersion6EqualsVersion6()
{
    TEST_ASSERT_EQUAL(0, Semver::versionCompare(version6, version6));
}

void testVersion7EqualsVersion7()
{
    TEST_ASSERT_EQUAL(0, Semver::versionCompare(version7, version7));
}

void testVersion8EqualsVersion8()
{
    TEST_ASSERT_EQUAL(0, Semver::versionCompare(version8, version8));
}

void testVersion1LessThanVersion8()
{
    TEST_ASSERT_EQUAL(1, Semver::versionCompare(version8, version1));
}

void testVersion2LessThanVersion7()
{
    TEST_ASSERT_EQUAL(1, Semver::versionCompare(version7, version2));
}

void testVersion3LessThanVersion6()
{
    TEST_ASSERT_EQUAL(1, Semver::versionCompare(version6, version3));
}

void testVersion4LessThanVersion5()
{
    TEST_ASSERT_EQUAL(1, Semver::versionCompare(version5, version4));
}

void runTests() {
    UNITY_BEGIN();
    RUN_TEST(testVersion2GreaterThanVersion1);
    RUN_TEST(testVersion3GreaterThanVersion2);
    RUN_TEST(testVersion4GreaterThanVersion3);
    RUN_TEST(testVersion5GreaterThanVersion4);
    RUN_TEST(testVersion6GreaterThanVersion5);
    RUN_TEST(testVersion7GreaterThanVersion6);
    RUN_TEST(testVersion8GreaterThanVersion7);
    RUN_TEST(testVersion1EqualsVersion1);
    RUN_TEST(testVersion2EqualsVersion2);
    RUN_TEST(testVersion3EqualsVersion3);
    RUN_TEST(testVersion4EqualsVersion4);
    RUN_TEST(testVersion5EqualsVersion5);
    RUN_TEST(testVersion6EqualsVersion6);
    RUN_TEST(testVersion7EqualsVersion7);
    RUN_TEST(testVersion8EqualsVersion8);
    RUN_TEST(testVersion1LessThanVersion8);
    RUN_TEST(testVersion2LessThanVersion7);
    RUN_TEST(testVersion3LessThanVersion6);
    RUN_TEST(testVersion4LessThanVersion5);
    UNITY_END();
}

#if defined(ARDUINO)
#include <Arduino.h>

void setup()
{
    runTests();
}

void loop() {}

#else
int main(int argc, char **argv)
{
    runTests();
}
#endif