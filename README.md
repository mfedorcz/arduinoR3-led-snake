# 🐍 Arduino LED Snake
 
Classic Snake game running on an 8x8 LED dot matrix, controlled with an analog joystick — built on Arduino Uno.
 
## Demo
 
https://github.com/user-attachments/assets/65ace0ee-e755-4c1e-8e4a-fbc47130b717

## Hardware
 
| Component | Model | Qty |
|---|---|---|
| Microcontroller | Arduino Uno Rev 3 | 1 |
| LED matrix | TOPLITE A-1088BS (8x8, 1.1") | 1 |
| Analog joystick | HW-504 | 1 |
| Resistors | 1000 Ohm | 8 |
| Breadboard | - | 1 |
| Jumper wires | - | a lot :D |
 
## Wiring
 
<img width="3000" height="2041" alt="circuit_image" src="https://github.com/user-attachments/assets/1cb8b7ef-0dfa-4be2-acc9-5341ec698ddd" />
 
The A-1088BS matrix is driven using multiplexing: 8 digital Arduino pins control the rows, and 8 pins (including some analog pins used digitally) control the columns through current-limiting resistors. The HW-504 joystick is connected to analog pins A0/A1 (X/Y axes) and a digital pin (push-button, used to restart the game).
 
Exact pin assignments are in the code (`rowPin[]`, `colPin[]`, `VRxPin`, `VRyPin`, `SWPin`).
 
> ⚠️ **Note:** the joystick button is wired to pin 1, which is the hardware RX line on Arduino Uno/Nano. If you add `Serial.begin()` for debugging, move the button to a different digital pin.
 
## How to play
 
1. Upload the sketch (`.ino`) to your Arduino via the Arduino IDE.
2. On startup, the snake (a single dot) and a fruit appear on the matrix.
3. Steer with the joystick — tilting it in a direction changes the snake's heading.
4. Eating fruit grows the snake.
5. Hitting a wall or the snake's own tail ends the game — a sad face is shown on the matrix for 4 seconds, then the game restarts automatically.
6. You can also restart manually by pressing the joystick button.
## How it works (technical overview)
 
- **Matrix multiplexing** — since the matrix only has 16 pins (8 rows + 8 columns) for 64 LEDs, it's not possible to light an arbitrary set of diodes at the same time. The program scans row by row fast enough that persistence of vision makes it look like a stable image.
- **Board state** is stored in a `bool diode[8][8]` array, independent of the physical pin control logic.
- **The snake** is represented as a circular buffer of positions (`snakeSegments[]`) with a head index (`headIndex`).
- **Timing** for snake movement and display refresh is decoupled using `millis()`, to avoid flickering.

## License
 
This project is available under the MIT License — see [LICENSE](LICENSE) for details.
 
## Author
~mfedorcz
 
