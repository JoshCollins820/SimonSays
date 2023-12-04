# SimonSays
For our embedded systems project we built a Simon Says game that is playable on a NUCLEO board.

![game_image]([https://github.com/JoshCollins820/SimonSays/blob/main/game.jpg])

---

### Objective
* Create our own Simon Says memory game:
  * Random sequence of colored LEDs will light up.
  * Player will have to memorize and mimic the color pattern by pressing the colored buttons in the correct order.
  * After every successful round, the sequence of colors gets longer and the LEDs light up at a faster rate.
* Our goal was to replicate the game with the hardware and knowledge gained from this course:
  * Input/Output: buttons, LEDs, potentiometer, LCD, speaker
  * Classes: Serial, PwmOut, AnalogIn
  * Hardware connections
  * Documentation: making a block diagram
 
### Related Work
* Our project’s differences: 
  * Pattern is consistent across rounds, whereas the other project generates a random sequence after every round.
  * LEDs blink progressively faster as the rounds increase.
  * Has sounds and jingles for every LED, button, winning round, game over, and highscore event.
  * Has a volume dial for the player to change to their preference.

### Physical Components

* List of components:
  * STM32F401RET6 Nucleo Board
  * Breadboard
  * LCD
  * Integrated Circuit 74HC595 (Shift Register)
  * LED: 1 Green, 1 Yellow, 1 Red
  * Piezo Speaker
  * Potentiometer
  * Resistors: Four 200 ohms and three 100 ohms
  * Gikfun Push Button: 1 Green, 1 Yellow, 1 Red
 
### Software
* Mbed OS 5
  * Included LCD & DebounceIn libraries
  * Usage of many of the classes learned in this course:
  * Serial, DebounceIn, DigitalOut, PwmOut, AnalogIn
* The program is modeled like a finite state machine
  * There are several states that dictate how the game flows
  * Main Menu state: 
    * updates LCD with PLAY text and HIGHSCORE, waits for player to press a button before starting match.
  * Match state: 
    * Sets up match variables (score, round number, empty pattern array)
    * While loop that updates/displays the LED sequence, has user input their pattern, and handles the round system. Ends when the player enters an incorrect pattern.
* Through the states and functions we :
  * We have full control over LEDs: We can enable them when the user presses a button or when displaying light patterns
  * We use polling for detecting button presses. Although not the most efficient, we found that polling provided us with more control over interrupts when it came to when we wanted a button input to have an effect.
  * We are able to play sounds & tunes by changing the period and duty cycle of the speaker in between wait.
 
### Obstacles
* Bouncing
  * The first obstacle that we knew was inevitable was the bouncing problem
  * Bouncing is when one button registers as multiple “presses”.
  * To resolve this issue, we use the DebounceIn library that eliminates the problem, allowing the player to freely input the pattern without worrying about mis-inputs.
* Confusion with button components
  * We had some confusion figuring out how to hook up the new colorful buttons to the breadboard as it did not have a labeled “1 2 3 4” under them.
  * After some tests we found that it needed to be hooked up differently.

### Contributions
* Vanessa:
  * LCD screens
  * LED pattern display
  * Sound/tune effects
  * Hardware setup
* Josh:
  * Pattern logic
  * State machine
  * Input handling
  * LCD effects
  * Hardware setup
* Gregorio:
  * Component research
  * Block diagram
  * Hardware setup






