# walker

Minimal RP2040 firmware for controlling Feetech servos on the half-duplex bus.

The Feetech data line uses the previous AX data pin, and the PIO TX program
drives the previous AX level-shifter direction pin during transmission.

## Commands

- `SCAN`: scan servo IDs.
- `SCANBAUD`: scan servo IDs at each common Feetech baud rate.
- `READ <id> <reg>`: read a Feetech register.
- `WRITE <id> <reg> <value>`: write a Feetech register.
- `MOVE <id> <position>`: write `STS_GOAL_POSITION`.
- `POS <id>`: read `STS_PRESENT_POSITION`.
- `TORQUE <id> <0|1>`: disable or enable torque.
- `SETID <old_id> <new_id>`: unlock EEPROM, change ID, then lock EEPROM.
- `SETBAUD <id> <baud_code>`: change the servo baud register and switch the
  bus to the matching speed.
- `BUSBAUD <baud>`: change only the controller bus speed.
- `EXTSIDE [1|2]`: get or set which motor-side link carries the 38 mm foot
  extension. `1` means `theta1`, `2` means `theta2`; default is `2`.
- `IK <fx_mm> <fy_mm> [branch1] [branch2]`: solve the five-bar inverse
  kinematics and print `ik,<ext_side>,<t1_mrad>,<t2_mrad>,<pos11>,<pos12>`.
- `FOOT <fx_mm> <fy_mm> [branch1] [branch2]`: solve inverse kinematics and
  command Feetech ID `11` and ID `12`.
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

## Five-Bar Notes

Feetech ID `11` is `theta1`, ID `12` is `theta2`, and `theta = 0` maps to
servo position `2048`. Branch arguments are optional and default to `0`.
With the extension on the `theta2` link, the straight-ahead zero-angle point is
approximately:

```text
EXTSIDE 2
IK 230.83 7.88 1 0
```

which should return positions close to `2048,2048`.
