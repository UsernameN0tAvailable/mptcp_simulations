# MPTCP And Mixed Scenarios Simulations

This repository makes use of the NS3 MPTCP implementation found [here](https://github.com/mkheirkhah/amp), which is based on ns3-19.
All the documentation about the original implementation is also valid for this repository.

## Addition: MpTcpOnOffApplication
The aforementioned implementation does not have an On/Off Application for the MpTcp protocol. Therefore, in this repository an we implement an On/Off Application, which is located in src/applications/model/mp-tcp-on-off-application.cc and the header file with the respective documentation is found in src/applications/model/mp-tcp-on-off-application.h . The On/Off Application is the only addition to the original implementation.

## Simulation Scenarios
The implemented simulation scenarios and the respective comments to the code are found in the scratch folder.

### Simulation Scripts
In folder [scratch/MixedBulkSend](https://github.com/UsernameN0tAvailable/mptcp_simulations/tree/master/scratch/MixedBulkSend), the mixed scenario is implemented, which make use of 2 MPTCP and 2 TCP connections which transfer data generated from a Bulk Send Application.

In folder [scratch/MixedOnOff](https://github.com/UsernameN0tAvailable/mptcp_simulations/tree/master/scratch/MixedOnOff), the mixed scenario is implemented, which make use of 2 MPTCP and 2 TCP connections which transfer data generated from an On/Off Application.
 
In folder [scratch/MpTcpBulkSend](https://github.com/UsernameN0tAvailable/mptcp_simulations/tree/master/scratch/MpTcpBulkSend), the following scenario is implemented: 5 MPTCP connections transfer data generated from a Bulk Send Application.

In folder [scratch/MpTcpOnOff](https://github.com/UsernameN0tAvailable/mptcp_simulations/tree/master/scratch/MpTcpOnOff), the following scenario is implemented: 5 MPTCP connections transfer data generated from an On/Off Application.

### Execution
Setup and Configuration:

````./waf configure````

Execution:
````./waf --run "<SimulationScript> --<parameter1>=<value> --<parameter2>=<value> ..."````

The simulation scripts are: MixedBulkSend, MixedOnOff, MpTcpBulkSend, MpTcpOnOff

Multiple parameters can be used at a time, the valid parameters are:

````ber````: Bit Error Ratio during the OFF phase

````per````: Packet Error Ratio during the ON phase

````duration````: simulation duration in seconds

````nSF````: number of subflows

````meanONDuration````: mean On phase duration in seconds

````mode````: Error Model: 1 := BER Error Model, 2 := ON/OFF Error Model, 3 := ON/OFF plus BER Error Model

#### Output
The output of mixed scenario is the throughput in bits per second through the whole network of MpTcp and of Tcp:

````<MpTcp throughput> <TCP throughput> ````

The output of the MPTCP-only scenarios is the throughput in bits per second through the whole network of MpTcp:

````<MpTcp throughput> ````

### Results
The results plotted in the thesis can be found in the [RESULTS](https://github.com/UsernameN0tAvailable/mptcp_simulations/tree/master/RESULTS) folder.
#### Throughput Results
The throughput results can be found in the folder [RESULTS/THROUGHPUT](https://github.com/UsernameN0tAvailable/mptcp_simulations/tree/master/RESULTS/THROUGHPUT). 
The [MPTCP_BULKSEND](https://github.com/UsernameN0tAvailable/mptcp_simulations/tree/master/RESULTS/THROUGHPUT/MPTCP_BULKSEND) folder contains the results obtained from the MPTCP-only scenarios with 
the Bulk Send Application. The [MPTCP_ON_OFF](https://github.com/UsernameN0tAvailable/mptcp_simulations/tree/master/RESULTS/THROUGHPUT/MPTCP_ON_OFF) folder contains the results obtained from the MPTCP-only scenarios with the On/Off Application. 
The [MIXED_BULKSEND](https://github.com/UsernameN0tAvailable/mptcp_simulations/tree/master/RESULTS/THROUGHPUT/MIXED_BULKSEND) folder contains the results obtained from the mixed scenarios with the Bulk Send Application. 
The [MIXED_ON_OFF](https://github.com/UsernameN0tAvailable/mptcp_simulations/tree/master/RESULTS/THROUGHPUT/MIXED_ON_OFF) folder contains the results obtained from the mixed scenarios with the On/Off Application. 
All four folders contain the results of the following four modes: MODE_1 contains the results obtained with the BER Error Model, MODE_2 the ON/OFF Error Model 
results with the OFF phase bit error ratio modulation, MODE_3 the results obtained with the ON/OFF plus BER Error Model and
MODE_4 the results obtained with the ON/OFF Error Model with the mean ON phase modulation. The text file names are descriptive, in that 
they represents the parameters used for execution (see above in the [Execution](#Execution) section for the description and the use of the parameters).

The content of the text files represents the following: ````<average throughput[bits]>   <standard deviation>````.
The average and the standard deviation of the throughput of four simulations with the same parameters but different execution seeds.
#### Congestion Window Results
The congestion window results can be found in the folder [RESULTS/CWND](https://github.com/UsernameN0tAvailable/mptcp_simulations/tree/master/RESULTS/CWND). 
The [CWND](https://github.com/UsernameN0tAvailable/mptcp_simulations/tree/master/RESULTS/CWND) folder contains the congestion window data plotted in the thesis, namely: 
the total congestion window of all fifteen subflows of one MPTCP connection in the Bulk Send Application scenario, the total congestion window of all fifteen
 MPTCP subflows of one connection in the On/Off Application scenario and the congestion window of one single subflow in the Bulk Send Application
scenario.

The content of the text files represents the following: ````<time stamp [seconds]> <congestion window size [bytes]>````




