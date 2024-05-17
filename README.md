# squadra_corse_task

## First Implementation with Arduino Mega

![image](https://github.com/giuuuug/squadra_corse_task/assets/150455039/1da5185a-ee0d-4557-90da-1c38cf7f50c0)

Arduino Mega has 6 timers:
* Timer0 and Timer1 are 8-bit
* Timer1, Timer3, Timer4 and Timer5 are 16-bit

About how timers for Arduno work [here](https://deepbluembedded.com/arduino-timers/#arduino-timers-timer-mode). For the autocalculate timer preloader [here](https://deepbluembedded.com/arduino-timer-calculator-code-generator/).

### Photoresistor part
![image](https://github.com/giuuuug/squadra_corse_task/assets/150455039/bd4d14b5-17cc-43e3-881d-e17b118d7dcd)

Here, using a photosestor as sensor, we will read every *200ms* the value and print on the LCD screen what type of brigthness is.
The table below shows for each value read from the ADC what is the corrisponding brightness level.
| Condition | Illumination (lux) | LDR Resistance | Voltage* | analogRead() value |
|---|---|---|---|---|
| Full moon | 0.1 | 1.25ΜΩ | 4.96 | 1016 |
| Deep twilight | 1 | 250ΚΩ | 4.81 | 985 |
| Twilight | 10 | 50ΚΩ | 4.17 | 853 |
| Computer monitor** | 50 | 16.2ΚΩ | 3.09 | 633 |
| Stairway lighting | 100 | 9.98ΚΩ | 2.50 | 511 |
| Office lighting | 400 | 3.78ΚΩ | 1.37 | 281 |
| Overcast day | 1,000 | 1.99ΚΩ | 0.83 | 170 |
| Full daylight | 10,000 | 397Ω | 0.19 | 39 |
| Direct sunlight | 100,000 | 79Ω | 0.04 | 8 |

### System voltage
![image](https://github.com/giuuuug/squadra_corse_task/assets/150455039/68473fa1-697c-4116-a325-6ebc6b52e299).

In that part we use *analogRead()* function to read the value from Arduino ADC an then convert it into voltage (from 0V to 5V). By rotating the potentiometer knob we can adjust the voltage value as we like.
the LEDs light up only in certain events:
+ system is in *undervoltage* state (<1.8V) -> Left LED will be ON
+ system is in *overvoltage* state (>2.7V) -> Right LED will be ON
+ nominal system operation -> LEDs OFF

### Emergency button
![image](https://github.com/giuuuug/squadra_corse_task/assets/150455039/7c563d55-d5b6-4fbf-9c9d-56d9c3a45a5f).

If the button is pressed, the system will enter in the *waiting state* so the loop algorithm will stop its operations. Re-pressing the button reset the system state into normal algorithm flow (*running state*).
>[!IMPORTANT]
>The solution provided uses the *busy waiting* timer dictated from the keywork *delay()*.
>The alternative solution (non-blocking one or *non-busy waiting*) is to use a 16-bit timer like this:
 >```
>TCCR4A = 0;           // Init Timer4A
>TCCR4B = 0;           // Init Timer4B
>TCCR4B |= B00000100;  // Prescaler = 256
>TCNT4 = 34285;        // Timer Preloading
>TIMSK4 |= B00000001;  // Enable Timer Overflow Interrupt
>```
