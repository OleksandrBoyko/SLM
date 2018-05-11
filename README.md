

According to project needs used custom profile as any of default profiles do not cover all required needs

SLM BLE device configured as server to provide to clients (Android or iOS devices) information about tuner
BLE Profile contains default services required to make device connection and custom services.

Default services:
 - Generic Access
 - Generic Attribute
 - Battery

Custom services hierarchy:
TunerInformation
|
|--- (c) Manufacturer Name String (UUID: 2A29) 				# Provide information about service name as custom service do not provide such information directly
|--- (c) SignalStrength 						# Provide signal strength information in tenth of dBm TBD
|        |--- (d) Client User Description (UUID:2901)			# Provide characteristic's name because custom service do not provide such information similar to service 
|        \--- (d) Client Characteristic Configuration (UUID:2902)	# Required for client to make configuration if user require notifications when status changes
\--- (c) LockStatus							# Provide tuner LOCK status
         |--- (d) Client User Description (UUID:2901)
         \--- (d) Client Characteristic Configuration (UUID:2902)

TunerConfiguration
|
|--- (c) Manufacturer Name String (UUID: 2A29)
|--- (c) TunerType							# Client provides tuner type configuration: 0 - ATSC, 1 - QAM-B
|        |--- (d) Client User Description (UUID:2901)
|        \--- (d) Valid range (UUID:2906)
|--- (c) SlaveAddr							# Client provides tuner slave I2C address ToBeDiscussed with customer is that required or not. Default is 96d (0x60h)
|        \--- (d) Client User Description (UUID:2901)
|--- (c) Frequency							# Client provides frequency to lock on in range 50-698 MHz
|        |--- (d) Client User Description (UUID:2901)
|        \--- (d) Valid range (UUID:2906)
|--- (c) DemodulatorBW							# Client provides demodulator bandwidth: 0 - 6MHz, 1 - 7 MHz, 2 - 8MHz
|        \--- (d) Client User Description (UUID:2901)
\--- (c) UpdateInterval							# Client provides update interval in ms 
         \--- (d) Client User Description (UUID:2901)
