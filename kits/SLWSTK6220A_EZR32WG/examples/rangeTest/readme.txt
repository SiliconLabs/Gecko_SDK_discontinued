Range Test example.

The Range Test application can be used to evaluate Silicon Laboratories radio
chips including performace, sensitivity or blocking measurements.

Range Test creates a radio link between two evaluation kits and sends 
predefined amount of packets from the transmitter side to the receiver.
The receiver calculates Packet Error Rate (PER) and Moving-Average (MA) of PER
based on the packets received. It also displays the current RSSI level in dBm
units and draws a chart of the RSSI historical data.
For long tests the transmitter can be set up to transmit continuously. It is not
necessary to start both sides synchronously.

*PLEASE NOTE* This example uses the radio extensively and could transmit at high
RF power levels that may result in higher peak current consumption. Due to the
peak current demand this example is not suited to run on a coin cell battery.
If standalone operation is required please consider the use of an external USB
connected battery or other power source.


Board:  Silicon Labs SLWSTK6220A_EZR32WG Starter Kit
Device: EZR32WG330F256R60