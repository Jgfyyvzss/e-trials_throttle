# e-trials_throttle
This project is the development of an Arduino based microcontroller to go between the throttle and main motor controller on an e-trials bike.

The microcontroller manages throttle smoothing and provides a 'virtual flywheel' effect by applying torque to force the motor to follow a pre-determined RPM decay rate when the throttle is closed.

Hardware:
* multiple interrupt pins for future expansion
* CANBus to enable easy porting to CAN enabled controllers instead of relying on signals from hall effect devices for RPM measurement
* fast processor for plenty of capacity for more complex management if it goes that way
* inbuilt USB for easy reconfiguring of parameters or software updates

The process as envisaged is:
* Read throttle input. If it is reducing then switch to routine to maintain the pre-determined RPM decay rate.
* Read motor RPM and calculate the inertia in the 'virtual flywheel'.
* From the calculated inertia set a maximum throttle signal limit that can be sent to the main controller.
* Calculate the current RPM decay rate and compare to the target rate.
* If RPM is decaying faster than the set rate, then apply throttle proportional to the difference between the two rates.
