#include <SimpleDHCP.h>

DHCP_TESTER *dhcp_tester;

void setup() {
    Serial.begin(9600);
    delay(500);
    dhcp_tester = new DHCP_TESTER();
    bool test_results = dhcp_tester->runTests();
    Serial.println(F("***************************************"));
    if (test_results) {
        Serial.println(F("All DHCP tests passed, library is ready for use"));
    } else {
        Serial.println(F("One or more DHCP tests failed, library is not ready for use"));
    }
}

void loop() {
}