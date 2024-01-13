# e-trials_throttle
This project is the development of a Teensy 4.0 based microcontroller to go between the throttle and main motor controller on an e-trials bike.

The Teensy manages throttle smoothing and provides a 'virtual flywheel' effect by applying torque to force the motor to follow a pre-determined RPM decay rate when the throttle is closed.

Teensy was chosen for this project because:
* multiple interrupt pins for future expansion
* CANBus to enable easy porting to CAN enabled controllers instead of relying on signals from hall effect devices for RPM measurement
  *fast processor for plenty of capacity for more complex management if it goes that way
  *inbuilt USB for easy reconfiguring of parameters or software updates
  *cost competitive with other much more limited hardware such as Arduino UNO.

The process as envisaged is:
*Read throttle input. If it is reducing then switch to routine to maintain the pre-determined RPM decay rate.
*Read motor RPM and calculate the inertia in the 'virtual flywheel'.
*From the calculated inertia set a maximum throttle signal limit that can be sent to the main controller.
*Calculate the current RPM decay rate and compare to the set rate.
*If RPM is decaying faster than the set rate, then apply throttle proportional to the difference between the two rates.
