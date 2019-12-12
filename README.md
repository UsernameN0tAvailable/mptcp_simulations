# MPTCP Simulations

This repository makes use of the NS3 MPTCP implementation found [here](https://github.com/mkheirkhah/amp), which is based on ns3-19.
All the documentation about the original implementation is also valid for this respository.

## Addition: MpTcpOnOffApplication
The aforementioned implementation does not have an On/Off application for the MpTcp protocol. Therefore, in this repository an we implement an On/Off application, which is located in src/applications/model/mp-tcp-on-off-application.cc and the header file with the respective documentation is found in src/applications/model/mp-tcp-on-off-application.h . The On/Off application is the only addition to the original implementation.

## Simulation Scenarios
The implemented simulation scenarios are found in the scratch folder.

### Network 
The implemented scenarios all use the same Network:

![alt text](https://github.com/UsernameN0tAvailable/mptcp_simulations/blob/master/sim_scenario_2.jpg "Network:")

### Simulation Scripts
In folder scratch/MixedBulkSend, the mixed scenario is implemented, which make use of 2 MPTCP and 2 TCP connections which transfer data generated from a Bulk Send application.

In folder scratch/MixedOnOff, the mixed scenario is implemented, which make use of 2 MPTCP and 2 TCP connections which transfer data generated from an On/Off application.
 
In folder scratch/MpTcpBulkSend, 5 MPTCP connections transfer data generated from a Bulk Send application.

In folder scratch/MpTcpOnOff, 5 MPTCP connections transfer data generated from an On/Off application.

### Execution
Setup and Configuration:

````./waf configure````

Execution:
````./waf --run "<SimulationScript> --<parameter>=<value>"````

The simulation scripts are: MixedBulkSend, MixedOnOff, MpTcpBulkSend, MpTcpOnOff

The valid parameters are:

````ber````: Bit Error Ratio during the OFF phase

````per````: Packet Error Ratio during the ON phase

````duration````: simulation duration in seconds

````nSF````: number of subflows

````meanONDuration````: mean On phase duration in seconds

````mode````: Error Model: 1 := BER Error Model, 2 := ON/OFF Error Model, 3 := ON/OFF plus BER Error Model

#### Output
The output of mixed scenario is the throughput through the whole network of MpTcp and of Tcp:

````<MpTcp throughput> <TCP throughput> ````

The output of the MPTCP-only scenarios is the throughput through the whole network of MpTcp:

````<MpTcp throughput> ````
