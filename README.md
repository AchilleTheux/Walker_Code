# walker

Minimal RP2040 firmware for controlling Feetech servos on the half-duplex bus.

The Feetech data line uses the previous AX data pin, and the PIO TX program
drives the previous AX level-shifter direction pin during transmission.

## Commands

- `SCAN`: scan servo IDs.
- `READ <id> <reg>`: read a Feetech register.
- `WRITE <id> <reg> <value>`: write a Feetech register.
- `MOVE <id> <position>`: write `STS_GOAL_POSITION`.
- `POS <id>`: read `STS_PRESENT_POSITION`.
- `TORQUE <id> <0|1>`: disable or enable torque.
- `SETID <old_id> <new_id>`: unlock EEPROM, change ID, then lock EEPROM.
- `SETBAUD <id> <baud_code>`: change the servo baud register and switch the
  bus to the matching speed.
- `BUSBAUD <baud>`: change only the controller bus speed.
- `HELP`: print available commands.

Feetech baud codes:

- `0`: 1000000
- `1`: 500000
- `2`: 250000
- `3`: 128000
- `4`: 115200
- `5`: 76800
- `6`: 57600
- `7`: 38400
