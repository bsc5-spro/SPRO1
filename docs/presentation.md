# SPRO1 programming

# Overview

Language: C
Approach: custom libraries

# General

## Program flow

1. init systems
2. pre-run setup
3. control loop
4. post-run debrief

# Init

```c
// counter used by optocoupler
TCCR1A = 0x00; // pure ticks counter
TCCR1B = 0xC5; // 1024 prescaler

// counter used by PWM
TCCR0A = (1 << WGM10) | (1 << COM1A1); // Fast PWM, 8-bit
TCCR0B = (0 << CS11) | (1 << CS10) | (1 << CS12); // Prescaler: 1024 ~ 1Hz
```

Setting `0xx5` and `(0 << CS11) | (1 << CS10) | (1 << CS12)`
does the same thing, as only the last 3 bits are used for
prescaling.

<!-- stop -->

There are only two counters on the Nano.

They derive from the internal MCU clock (16MHz).

# Init

Remove

```c
io_redirect();
```

as UART communication is done between Nextion and MCU.

# Display

Uses UART Serial communication. -->

# Byte-wise communication

<!-- stop -->

Messages are sent one byte at a time, through `TX` line

Incoming bytes (from `RX`) are stored in a buffer, allowing
asynchronous communication.

To read the message, one must decode the buffer
appropriately. There is no standard for the packet
structure, so always refer to manufacturer documentation or
develop your own. :)

# Nextion communication protocoll

Question-and-response and notification style communication.

## Question-and-response

Whenever the _display_ receives a command, it must respond.

Responses include:

- execution success-failure response
- requested value

## Notification

The _display_ can send packets unprompted by the
microcontroller. This can be a result of a button pressed.
Tricky to receive.

# Nextion message syntax

```
xx aa aa (...) FF FF FF
```

- xx : message type identifier
- aa : message contents (in hex)

All (default) messages end in **FF**.

# Capabilites

- set value
- get value
- change page
- receive and handle unprompted data

# Motor

Standard DC motor

low voltage: brr
high voltage: brrrrrrrrrrrrr

# PWM

A square wave between `0V` and `5V`.

How 'HIGH' it is depends on the _duty cycle_

> a percentage of unit time, which depends on prescaler

<!-- stop -->

Signal is fed to _motor driver_, amplifying the PWM voltage.

# Optocoupler

Same basic principle as a light gate.

<!-- stop -->

(it **IS** a light gate)

Whenever there is an obstruction, the signal pin is pulled
HIGH

# Reading values

With `TCCR1B` set to `c5`, `TCNT1` can be read to see
current timer value.

`ICR1` copies the value of `TCNT1` when `PB0` is pulled up.

Comparing two consecutive values of `ICR1`, the time between
two rising edges can be determined.

# Measurement to real world data

$$
v = alpha * r_w
$$

$$
alpha = d_alpha / t
$$

| symbol    | meaning                         |
| --------- | ------------------------------- |
| $v$       | car velocity                    |
| $alpha$   | angular velocity                |
| $d_alpha$ | differenc of angle between gaps |
| $t$       | elapsed time                    |

# Circuits

# Step-down

Battery supplies **9V** while the Arduino can take **_at
most_** 6 (preferably 5)

<!-- stop -->

-> Solution: step-down converter
