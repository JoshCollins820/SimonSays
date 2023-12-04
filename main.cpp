#include "PinNames.h"
#include "mbed.h"
#include "platform/mbed_thread.h"
#include <stdlib.h>
#include <cstdlib>
#include <time.h>
#include "NHD_0216HZ.h"
#include <DebounceIn.h>
#include <stdio.h>


// Define Serial
Serial pc(D1,D0);
// Define Buttons
DebounceIn b1(D3); // green button b1
DebounceIn b2(D4); // yellow button b2
DebounceIn b3(D5); // red button b3
// Define LEDs
DigitalOut ledG(D6); // color 1
DigitalOut ledY(D7); // color 2
DigitalOut ledR(D8); // color 3
// Define the PWM output for the speaker
PwmOut speaker(D9); // speaker
// Define Potentiometer
AnalogIn volumeKnob(A0); // potentiometer


// highscore
int HIGHSCORE;
// temp score used for tallying points in score screen
int TEMP_SCORE;
// max size of array
int SIZE = 100;
// wait times
int SPEED_LED; // ms to wait for led to change
int SPEED_STEP_LED = 100; // ms to step down SPEED_LED every round
int SPEED_MIN_LED = 200; // minimum ms that SPEED_LED will get
int SPEED_MAX_LED_LOW = 800; // maximum ms that SPEED_LED will be for low
int SPEED_MAX_LED_MED = 500; // maximum ms that SPEED_LED will be for medium
int SPEED_MAX_LED_HI = 300; // maximum ms that SPEED_LED will be for high
int SPEED_ROUND_INTERM = 1000; // ms to wait for round to change
int SPEED_BUTTON_TICK = 50; // ms to wait before button polling refreshes
int SPEED_REPLAY_PATTERN = 100; // ms to wait when replaying pattern after player loses
// sound
volatile float VOLUME = 0.5; // volume for sounds


// LED FUNCTIONS
void lcd_main_menu() {
    // print to LCD
    set_cursor(0, 0);
    print_lcd("SIMON SAYS      ");
    set_cursor(0, 1);
    print_lcd("PRESS ANY BUTTON");
    set_cursor(16,1); // remove cursor
}

// LED FUNCTIONS
void lcd_level_select() {
    // print to LCD
    set_cursor(0, 0);
    print_lcd("LEVEL SELECT    ");
    set_cursor(0, 1);
    //print_lcd("HIGHSCORE:      ");
    print_lcd("G=LOW Y=MED R=HI");
    set_cursor(16,1); // remove cursor
}

void lcd_game_started(int difficulty) {
    // print to LCD
    set_cursor(0, 0);
    print_lcd("GAME STARTED    ");
    set_cursor(0, 1);
    print_lcd("DIFFICULTY:     ");
    set_cursor(12, 1);
    if(difficulty == 1)
    {
        print_lcd("LOW  ");
    }
    else if(difficulty == 2)
    {
        print_lcd("MED  ");
    }
    else if(difficulty == 3)
    {
        print_lcd("HIGH ");
    }
    ThisThread::sleep_for(2000);
    set_cursor(0, 0);
    print_lcd("ROUND: 1        ");
    set_cursor(0, 1);
    print_lcd("                ");
    set_cursor(16,1); // remove cursor
    //wait 1 second
    ThisThread::sleep_for(1000);
}

void lcd_game_playing(int score) {
    // convert ints to strings
    char charHighscore[50];
    sprintf(charHighscore, "%d", HIGHSCORE); 
    char charScore[50];
    sprintf(charScore, "%d", score); 

    // print to LCD
    set_cursor(0, 0);
    print_lcd("SCORE:          ");
    set_cursor(7, 0); // move cursor to after "Score: "
    print_lcd(charScore); // print string
    set_cursor(0, 1);
    print_lcd("HIGHSCORE:      ");
    set_cursor(11,1); // move cursor to after "Highscore: "
    print_lcd(charHighscore); // print string
    set_cursor(16,1); // remove cursor
}

void lcd_win_round(int round) {
    // convert int to string
    char charRound[50];
    sprintf(charRound, "%d", round); 

    // print to LCD
    set_cursor(0, 0);
    print_lcd("GREAT JOB!      ");
    set_cursor(0, 1);
    print_lcd("                ");
    set_cursor(16,1); // remove cursor
    ThisThread::sleep_for(1000);
    set_cursor(0, 0);
    print_lcd("ROUND:          ");
    set_cursor(7, 0); // move cursor to after "Round: "
    print_lcd(charRound); // print string
    set_cursor(16,1); // remove cursor
}

void lcd_game_over() {
    // print to LCD
    set_cursor(0, 0);
    print_lcd("GAME OVER...    ");
    set_cursor(0, 1);
    print_lcd("                ");
    set_cursor(16,1); // remove cursor
}

void lcd_show_score(int score) {
    // convert ints to strings 
    char charScore[50];
    sprintf(charScore, "%d", score); 
    char charTempScore[50];
    sprintf(charTempScore, "%d", TEMP_SCORE); 

    // print to LCD
    set_cursor(0, 0);
    print_lcd("SCORE:          ");
    set_cursor(7, 0); // move cursor to after "Score: "
    print_lcd(charTempScore); // print string
    set_cursor(0, 1); // move cursor to after "Score: "
    // display message about their performance
    if(score < 10)
    {
        print_lcd("???...          ");
    }
    else if(score < 30)
    {
        print_lcd("NOOB..          ");
    }
    else if(score < 50)
    {
        print_lcd("NICE...         ");
    }
    else if(score < 700)
    {
        print_lcd("GREAT!          ");
    }
    else if(score < 100)
    {
        print_lcd("AMAZING!        ");
    }
    else if(score < 150)
    {
        print_lcd("HOLY CACA!      ");
    }
    else if(score < 200)
    {
        print_lcd("INSANE!         ");
    }
    else if(score < 250)
    {
        print_lcd("LEGEND!         ");
    }
    else if(score < 250)
    {
        print_lcd("SKINNY LEGEND!  ");
    }
    else if(score < 300)
    {
        print_lcd("PRESTIGIOUS!    ");
    }
    else if(score < 350)
    {
        print_lcd("HOW!? CHEATER?  ");
    }
    else if(score < 400)
    {
        print_lcd("UNBELIEVEABLE!  ");
    }
    else if(score <= 500)
    {
        print_lcd("MULTIDIMENSIONAL");
    }
    set_cursor(16,1); // remove cursor
}

void lcd_highscore_on() {
    // convert ints to strings
    char charHighscore[50];
    sprintf(charHighscore, "%d", HIGHSCORE); 

    // print to LCD
    set_cursor(0, 0);
    print_lcd("NEW HIGHSCORE   ");
    set_cursor(0, 1);
    print_lcd("HIGHSCORE:      ");
    set_cursor(11,1); // move cursor to after "Highscore: "
    print_lcd(charHighscore); // print string
    set_cursor(16,1); // remove cursor
}

void lcd_highscore_off() {
    // convert ints to strings
    char charHighscore[50];
    sprintf(charHighscore, "%d", HIGHSCORE); 

    // print to LCD
    set_cursor(0, 0);
    print_lcd("                ");
    set_cursor(0, 1);
    print_lcd("HIGHSCORE:      ");
    set_cursor(11,1); // move cursor to after "Highscore: "
    print_lcd(charHighscore); // print string
    set_cursor(16,1); // remove cursor
}


// SOUND FUNCTIONS
void sound_green()
{
    float note1 = 0.3;
    VOLUME = volumeKnob.read(); // check for volume change
    speaker.period((0.003*note1)+0.000125); // frequency
}

void sound_yellow()
{
    float note1 = 0.5;
    VOLUME = volumeKnob.read(); // check for volume change
    speaker.period((0.003*note1)+0.000125); // frequency
}

void sound_red()
{
    float note1 = 0.7;
    VOLUME = volumeKnob.read(); // check for volume change
    speaker.period((0.003*note1)+0.000125); // frequency
}

void sound_play()
{
    float note1 = 0.15;
    VOLUME = volumeKnob.read(); // check for volume change

    speaker = VOLUME; // unmute
    speaker.period((0.003*note1)+0.000125); // frequency
    ThisThread::sleep_for(100); // duration
    speaker = 0; // mute
}

void sound_win()
{
    float note1 = 0.3;
    float note2 = 0.1;

    VOLUME = volumeKnob.read(); // check for volume change
    speaker = VOLUME; // unmute
    speaker.period((0.003*note1)+0.000125); // frequency
    ThisThread::sleep_for(100); // duration
    speaker.period((0.003*note2)+0.000125); // frequency
    ThisThread::sleep_for(100); // duration
    speaker = 0; // mute
}

void sound_lose()
{
    float note1 = 0.1;
    float note2 = 0.3;
    float note3 = 0.15;
    float note4 = 0.4;
    float note5 = 0.2;
    float note6 = 0.5;
    float note7 = 0.25;
    float note8 = 0.6;
    float note9 = 0.3;
    float note10 = 0.7;
    float note11 = 0.35;
    float note12 = 0.8;

    VOLUME = volumeKnob.read(); // check for volume change
    speaker = VOLUME; // unmute
    speaker.period((0.003*note1)+0.000125); // frequency
    ThisThread::sleep_for(150); // duration
    speaker.period((0.003*note2)+0.000125); // frequency
    ThisThread::sleep_for(200); // duration
    speaker.period((0.003*note3)+0.000125); // frequency
    ThisThread::sleep_for(150); // duration
    speaker.period((0.003*note4)+0.000125); // frequency
    ThisThread::sleep_for(200); // duration
    speaker.period((0.003*note5)+0.000125); // frequency
    ThisThread::sleep_for(150); // duration
    speaker.period((0.003*note6)+0.000125); // frequency
    ThisThread::sleep_for(200); // duration
    speaker.period((0.003*note7)+0.000125); // frequency
    ThisThread::sleep_for(150); // duration
    speaker.period((0.003*note8)+0.000125); // frequency
    ThisThread::sleep_for(200); // duration
    speaker.period((0.003*note9)+0.000125); // frequency
    ThisThread::sleep_for(150); // duration
    speaker.period((0.003*note10)+0.000125); // frequency
    ThisThread::sleep_for(200); // duration
    speaker.period((0.003*note11)+0.000125); // frequency
    ThisThread::sleep_for(250); // duration
    speaker.period((0.003*note12)+0.000125); // frequency
    ThisThread::sleep_for(300); // duration
    speaker = 0; // mute
}

void sound_highscore()
{
    float note1 = 0.4;
    float note2 = 0.3;
    float note3 = 0.2;
    float note4 = 0.15;
    float note6 = 0.1;

    VOLUME = volumeKnob.read(); // check for volume change
    lcd_highscore_on(); // flash on NEW HIGHSCORE
    speaker = VOLUME; // unmute
    speaker.period((0.003*note1)+0.000125); // frequency
    ThisThread::sleep_for(150); // duration
    speaker = 0; // mute
    ThisThread::sleep_for(50); // duration
    lcd_highscore_off(); // flash off NEW HIGHSCORE
    speaker = VOLUME; // unmute
    speaker.period((0.003*note1)+0.000125); // frequency
    ThisThread::sleep_for(150); // duration
    speaker = 0; // mute
    ThisThread::sleep_for(50); // duration
    lcd_highscore_on(); // flash on NEW HIGHSCORE
    speaker = VOLUME; // unmute
    speaker.period((0.003*note1)+0.000125); // frequency
    ThisThread::sleep_for(150); // duration
    speaker = 0; // frequency
    ThisThread::sleep_for(50); // duration
    lcd_highscore_off(); // flash on NEW HIGHSCORE
    speaker = VOLUME; // unmute
    speaker.period((0.003*note4)+0.000125); // frequency
    ThisThread::sleep_for(250); // duration
    lcd_highscore_on(); // flash off NEW HIGHSCORE
    speaker.period((0.003*note6)+0.000125); // frequency
    ThisThread::sleep_for(350); // duration
    lcd_highscore_off(); // flash on NEW HIGHSCORE
    speaker.period((0.003*note2)+0.000125); // frequency
    ThisThread::sleep_for(200); // duration
    lcd_highscore_on(); // flash off NEW HIGHSCORE
    speaker.period((0.003*note3)+0.000125); // frequency
    ThisThread::sleep_for(150); // duration
    lcd_highscore_off(); // flash on NEW HIGHSCORE
    speaker.period((0.003*note4)+0.000125); // frequency
    ThisThread::sleep_for(100); // duration
    lcd_highscore_on(); // flash off NEW HIGHSCORE
    speaker.period((0.003*note6)+0.000125); // frequency
    ThisThread::sleep_for(300); // duration

    speaker = 0;
}


// function that gets pattern from user and compares it to the actual pattern
bool input_pattern(int ptrn[], int rnd) {
    pc.printf("Entered input pattern\n");
    int correct_color;
    int button_pressed;
    // user input
    for (int i = 0; i < rnd; i++) // loops until pattern is iterated through
    {
        correct_color = ptrn[i]; // get the correct color from the pattern array
        pc.printf("Correct color: %d\n", correct_color);
        button_pressed = -1; // reset button_pressed to -1 (not pressed)
        // wait for user to press a button
        while(button_pressed == -1) // while the user hasn't pressed a button (-1)
        {
            // disable leds
            ledG = 0;
            ledY = 0;
            ledR = 0;
            // if user presses button 1
            if(b1.read() == 0)      
            {
                pc.printf("User pressed b1 (GREEN)!\n");
                button_pressed = 1; // store button press
                sound_green(); // prepare b1 sound
                speaker = VOLUME; 
                ledG = 1; // turn on green led

                while(b1.read() == 0); // wait until user unpresses button

                ThisThread::sleep_for(100); // minimum amount of beep
                ledG = 0;
                speaker = 0;
            }
            // if user pressed button 2 else if(b2.read() == 1) 
            else if(b2.read() == 0) 
            {
                pc.printf("User pressed b2 (YELLOW)!\n");
                button_pressed = 2; // store button press
                sound_yellow(); // prepare b2 sound
                speaker = VOLUME; // unmute
                ledY = 1; // turn on yellow led

                while(b2.read() == 0); // wait until user unpresses button

                ThisThread::sleep_for(100); // minimum amount of beep
                ledY = 0;
                speaker = 0;
            } 
            // if user presses button 3
            else if(b3.read() == 0) 
            {
                pc.printf("User pressed b3 (RED)!\n");
                button_pressed = 3; // store button press
                sound_red(); // prepare b3 sound
                speaker = VOLUME; // unmute
                ledR = 1; // turn on red led

                while(b3.read() == 0); // wait until user unpresses button

                ThisThread::sleep_for(100); // minimum amount of beep
                ledR = 0;
                speaker = 0;
            }
            ThisThread::sleep_for(SPEED_BUTTON_TICK); // how often you poll for new button press
        }
        // if the entered color does not match array[i]
        if(button_pressed != correct_color) // wrong pattern
        {
            pc.printf("Wrong color!\n");
            return false; // user loses
        }
        // else, keep looping
    }
    // user entered the correct pattern
    return true;
}

// function that randomly adds a new color to the pattern
void update_pattern(int ptrn[], int rnd) {
    pc.printf("Update pattern entered\n");
    // round has just been incremented, so enter a new color(1-3), at position rnd-1
    int random_color = (rand() % 3) + 1;
    ptrn[rnd - 1] = random_color;
}

// function display led pattern
void display_pattern(int ptrn[], int rnd, float speed, int score, bool replay) {
    pc.printf("Display pattern entered\n");
    TEMP_SCORE = 0; // reset temp score
    for (int i = 0; i < rnd; i++) {
        ThisThread::sleep_for(speed/2); // wait for a bit so that the light/sound turns off inbetween beeps
        if (ptrn[i] == 1) {
            sound_green(); // prepare green sound
            ledG = 1; // turn on ledG
            speaker = VOLUME; // unmute speaker
            ThisThread::sleep_for(speed); // wait for a bit
            ledG = 0; // turn off ledG
            speaker = 0; // mute speaker
        }
        else if (ptrn[i] == 2) {
            sound_yellow(); // prepare yellow sound
            ledY = 1; // turn on ledG
            speaker = VOLUME; // unmute speaker
            ThisThread::sleep_for(speed); // wait for a bit
            ledY = 0; // turn off ledY
            speaker = 0; // mute speaker
        }
        else if (ptrn[i] == 3) {
            sound_red(); // prepare red sound
            ledR = 1; // turn on ledR
            speaker = VOLUME; // unmute speaker
            ThisThread::sleep_for(speed); // wait for a bit
            ledR = 0; // turn off ledR
            speaker = 0; // mute speaker
        }
        if(replay == true) // if this is being called to display the score at the end of the game
        {
            TEMP_SCORE += 10; // add to temp score for score screen
            lcd_show_score(score); 
        }
        
    }
}


// function for when player is in game state
void game_state(int difficulty) {
    pc.printf("Game entered!\n");
    // initial setup
    bool alive = true; // if player is still playing the game and hasn't messed up
    int pattern[SIZE]; // array that will hold the simon says pattern, green = 1, yellow = 2, red = 3
    int score = 0; // reset score to 0
    int round = 1; // number round we are on, start at 1
    bool won;      // if the player won the current round

    // START GAME ------------------------------------------------------------------------------
    pc.printf("Showing Game Started to LCD\n");
    lcd_game_started(difficulty); // display lcd message for starting a game
    //ThisThread::sleep_for(1500); // wait some time for lcd to finish

    pc.printf("Entering match while loop...\n");
    while (alive) // loop until player loses
    {
        pc.printf("Start of match while loop\n");
        pc.printf("Showing Playing to LCD\n");
        lcd_game_playing(score); // display lcd message for playing game
        // start of round
        pc.printf("Entering update pattern\n");
        update_pattern(pattern, round); // a random color (1-3) is added to the pattern array
        pc.printf("Returned from update pattern\n");
        pc.printf("Entering display pattern\n");
        display_pattern(pattern, round, SPEED_LED, score, false); // display updated pattern to user with SPEED_LED (false since this is not for replay)
        pc.printf("Returned from update pattern\n");
        // player guesses pattern
        pc.printf("Entering input pattern\n");
        won = input_pattern(pattern, round); // check if player gets the pattern right
        pc.printf("Returned from input pattern\n");
        if (won == true) // if player won the round
        {
            pc.printf("Player won round!\n");
            round += 1;  // increment to next round
            score += 10; // reward player with score
            if(SPEED_LED > SPEED_MIN_LED) // if SPEED_LED is not at minimum
            {
                SPEED_LED -= SPEED_STEP_LED; // decrease SPEED_LED to make LEDs flash quicker
            }
            lcd_win_round(round); // display lcd message for round win
            sound_win(); // play sound to signal a win
        } 
        else 
        {
            pc.printf("Player lost round!\n");
            alive = false; // player has lost
            lcd_game_over();
            sound_lose(); // play sound to signal a loss
            lcd_show_score(score);
            display_pattern(pattern, round-1, SPEED_REPLAY_PATTERN, score, true); // display pattern that the user was able to get to successfully (round-1)
            ThisThread::sleep_for(500);
        }
        ThisThread::sleep_for(SPEED_ROUND_INTERM); // wait for some time
    }
    // GAME OVER ------------------------------------------------------------------------------
    pc.printf("Game over!\n");
    if (score > HIGHSCORE) // if score is higher than highscore
    {
        pc.printf("Updating score\n");
        HIGHSCORE = score; // update highscore
        //lcd_highscore_game(); // display LCD message for highscore change
        sound_highscore(); // play highscore sound
    }
    // return to menu
}

/*----------------------------------------------------------------------------
 MAIN function
 *----------------------------------------------------------------------------*/

int main() {
    //Initialise the SPI and the LCD
    pc.printf("Starting Main!");
    
    pc.printf("Initializing SPI and LCD\n");
    init_spi();
    init_lcd();

    pc.printf("Setting HIGHSCORE to 0\n");
    // initial setup
    srand(time(0)); // generate a random seed
    // set buttons to pull up
    b1.mode(PullUp);
    b2.mode(PullUp);
    b3.mode(PullUp);
    HIGHSCORE = 0;  // reset highscore every time the program starts up
    int b_pressed; // indicates if player has selected to play, if not it will be -1
    int difficulty; // users chosen difficulty

    pc.printf("Entering while true loop...\n");
    // loop
    while (true) {
        pc.printf("Start of while loop\n");
        b_pressed = -1; // reset b_pressed to -1
        difficulty = -1; // reset difficulty to -1
        pc.printf("Showing Main Menu to LCD\n");
        // inside main menu
        lcd_main_menu();

        pc.printf("Entering awaiting game start loop...\n");
        pc.printf("Press Button to Start Game!\n");
        // wait for button press
        while(b_pressed == -1) // loop as long as button isn't pressed
        {
            if(b1.read() == 0 || b2.read() == 0 || b3.read() == 0) // if button is pressed
            {
                pc.printf("Button Pressed!\n");
                b_pressed = 0; // set to something not -1
                sound_play();
            }
            ThisThread::sleep_for(SPEED_BUTTON_TICK); // wait a little bit
        }
        // inside level select
        lcd_level_select();
        while(difficulty == -1) // loop as long as button isn't pressed
        {
            if(b1.read() == 0) // if green button is pressed
            {
                pc.printf("Button Pressed! (LOW)\n");
                difficulty = 1; // set to 1, will act as difficulty
                sound_play();
                SPEED_LED = SPEED_MAX_LED_LOW; // user selected LOW
            }
            else if(b2.read() == 0) // if yellow button is pressed
            {
                pc.printf("Button Pressed! (MED)\n");
                difficulty = 2; // set to 2, will act as difficulty
                sound_play();
                SPEED_LED = SPEED_MAX_LED_MED; // user selected MED
            }
            else if(b3.read() == 0) // if red button is pressed
            {
                pc.printf("Button Pressed! (HIGH)\n"); 
                difficulty = 3; // set to 3, will act as difficulty
                sound_play();
                SPEED_LED = SPEED_MAX_LED_HI; // user selected HIGH
            }
            ThisThread::sleep_for(SPEED_BUTTON_TICK); // wait a little bit
        }
        // start game
        pc.printf("Starting game!\n");
        game_state(difficulty);
        pc.printf("Returned from game\n");
    }
}
