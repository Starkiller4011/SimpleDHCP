#include <SimpleDHCP.h>

DHCP_TESTER *dhcp_tester;

void setup() {
    Serial.begin(9600);
    dhcp_tester = new DHCP_TESTER();
    if (dhcp_tester->runTests()) {
        Serial.println("All tests passed");
    } else {
        Serial.println("Test failed");
    }
    delete dhcp_tester;
}

void loop() {
}