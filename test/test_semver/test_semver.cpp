#include <gtest/gtest.h>

// TEST(...)
// TEST_F(...)

#if defined(ARDUINO)
#include <Arduino.h>
#endif
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

TEST(SemverTests, version_2_greater_than_version_1)
{
    EXPECT_EQ(-1, Semver::versionCompare(version1, version2));
}

TEST(SemverTests, version_3_greater_than_version_2)
{
    EXPECT_EQ(-1, Semver::versionCompare(version2, version3));
}

TEST(SemverTests, version_4_greater_than_version_3)
{
    EXPECT_EQ(-1, Semver::versionCompare(version3, version4));
}

TEST(SemverTests, version_5_greater_than_version_4)
{
    EXPECT_EQ(-1, Semver::versionCompare(version4, version5));
}

TEST(SemverTests, version_6_greater_than_version_5)
{
    EXPECT_EQ(-1, Semver::versionCompare(version5, version6));
}

TEST(SemverTests, version_7_greater_than_version_6)
{
    EXPECT_EQ(-1, Semver::versionCompare(version6, version7));
}

TEST(SemverTests, version_8_greater_than_version_7)
{
    EXPECT_EQ(-1, Semver::versionCompare(version7, version8));
}

TEST(SemverTests, version_1_equals_version_1)
{
    EXPECT_EQ(0, Semver::versionCompare(version1, version1));
}

TEST(SemverTests, version_2_equals_version_2)
{
    EXPECT_EQ(0, Semver::versionCompare(version2, version2));
}

TEST(SemverTests, version_3_equals_version_3)
{
    EXPECT_EQ(0, Semver::versionCompare(version3, version3));
}

TEST(SemverTests, version_4_equals_version_4)
{
    EXPECT_EQ(0, Semver::versionCompare(version4, version4));
}

TEST(SemverTests, version_5_equals_version_5)
{
    EXPECT_EQ(0, Semver::versionCompare(version5, version5));
}

TEST(SemverTests, version_6_equals_version_6)
{
    EXPECT_EQ(0, Semver::versionCompare(version6, version6));
}

TEST(SemverTests, version_7_equals_version_7)
{
    EXPECT_EQ(0, Semver::versionCompare(version7, version7));
}

TEST(SemverTests, version_8_equals_version_8)
{
    EXPECT_EQ(0, Semver::versionCompare(version8, version8));
}

TEST(SemverTests, version_1_less_than_version_8)
{
    EXPECT_EQ(1, Semver::versionCompare(version8, version1));
}

TEST(SemverTests, version_2_less_than_version_7)
{
    EXPECT_EQ(1, Semver::versionCompare(version7, version2));
}

TEST(SemverTests, version_3_less_than_version_6)
{
    EXPECT_EQ(1, Semver::versionCompare(version6, version3));
}

TEST(SemverTests, version_4_less_than_version_5)
{
    EXPECT_EQ(1, Semver::versionCompare(version5, version4));
}

void setup()
{
    setUp();
    Serial.begin(115200);
    ::testing::InitGoogleTest();
}

void loop()
{
  // Run tests
  if (RUN_ALL_TESTS())
  ;

  // sleep for 1 sec
  delay(1000);
}
