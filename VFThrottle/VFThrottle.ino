// Pseudocode development.
//
// includes

// Declare variables
// Mass - Flywheel mass
// Radius - Flywheel radius
// Ip 
// Idiv
// Ioffset - Minimum motor current
// Tx - Energy to throttle value (non scaled)
// ThrFactor - convert current to throttle V signal
// MaxT - Maximum Throttle signal correction
// ThrottleOut - value to send to controller throttle
// MomentI - Static Flywheel value
// RPM = motor RPM

// Initialise
// MomentI = 0.5 x Mass x Radius^2

// If throttle is reducing
// Then goto flywheel routine
// Else goto throttle smoothing routine

// Throttle Smoothing
// 

//Flywheel Routine
// Read RPM
// Inertia = MomentI*(RPM/9.55)^2
// 
// MaxT = 
// ThrottleOut = 
// Send ThrottleOut

// RPM1 = RPM
// Wait for 100ms
// Read RPM
// 