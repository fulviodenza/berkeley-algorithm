# Berkeley Algorithm
![](docs/intro.png)

This is an implementation of the Berkeley Algorithm in the C programming language, with the aim to distribute correctly among the clients an internal timestamp defined by a server.
Raw TCP connections using sockets are used for our implementation.

## Slaves
Every slave sends its actual timestamp to the master and may receive a response with an adjustment to apply to its clock.

## Master
The master calculates the average of all the timestamps that it has received, including its own clock, and sends to each client the difference between the calculated average and the sent timestamp (the adjustment), if it's more than a prefixed delta-T.

## Future additional features
- RTT calculation: the Round Trip Time can be taken into account for the average of the timestamps
- UDP protocol: use UDP to reduce the overhead
- Master election: Fault Tolerance