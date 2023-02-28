#include <emulation.h>

void setUp(void) {}

void tearDown(void) {}

void runTests() {
	UNITY_BEGIN();
	// TODO add tests
	UNITY_END();
}

#if defined(ARDUINO)
#include <Arduino.h>

void setup() {
	runTests();
}

void loop() {}

#else

int main(int argc, char **argv) {
	runTests();
	return 0;
}

#endif
