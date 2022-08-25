#include <Arduino.h>
#include "task3.h"
#include "events.h"

static bool compareKeys(BUTTONS *pKey, BUTTONS *pSecret)
{
    bool correct = true;
    for (uint8_t i = 0; i < 5; i++)
    {
        if (pSecret[i] != pKey[i]){
            correct = false;
            break;
        }
    }
    return correct;
}

void task3()
{
    enum class TaskStates
    {
        INIT,
        //MOF States
        SLOW,
        WAIT_OFF,
        MEDIUM,
        WAIT_ON,
        FAST,
        PERMA_OFF,
        PERMA_ON
    };

    static TaskStates taskState = TaskStates::INIT;

    const uint8_t ledBlink = 25;

    static uint32_t lasTime;

    static bool ledStatus = false;

    const uint32_t SlowInterval = 1000;

    const uint32_t MediumInterval = 500;

    const uint32_t FastInterval = 250;
    static bool switchState;

    //Password Var
    static BUTTONS secret[5] = {BUTTONS::B1, BUTTONS::B1, BUTTONS::B2, BUTTONS::B2, BUTTONS::B1};

    static BUTTONS disarmKey[5] = {BUTTONS::NONE};
    static uint8_t keyCounter;

    switch (taskState)
    {
        //Initialization of Vars
        case TaskStates::INIT:
        {
            pinMode(ledBlink, OUTPUT);
            digitalWrite(ledBlink, LOW);
            lasTime = millis();

            keyCounter = 0;
            taskState = TaskStates::SLOW;
            break;
        }

        case TaskStates::SLOW:
        {
            uint32_t currentTime = millis();

            if( (currentTime - lasTime) >= SlowInterval )
            {
                lasTime = currentTime;
                digitalWrite(ledBlink,ledStatus);
                ledStatus = !ledStatus;
            }

            if(buttonEvt.trigger == true)
            {
                buttonEvt.trigger = false;
                if(buttonEvt.whichButton == BUTTONS::B1)
                {
                    taskState = TaskStates::WAIT_OFF;
                    printf("Change to Wait: %d\n");
                }
                if (buttonEvt.whichButton == BUTTONS::B2)
                {
                    keyCounter = 0;
                    taskState = TaskStates::MEDIUM;
                    printf("Change to Medium Mode: %d\n");
                }
            }
            break;
        }

        case TaskStates::WAIT_OFF:
        {
            uint32_t currentTime = millis();
            if( (currentTime - lasTime) >= SlowInterval )
            {
                digitalWrite(ledBlink, LOW);
                taskState = TaskStates::PERMA_OFF;
                printf("Change to Perma_OFF: %d\n");
            }
            break;
        }

        case TaskStates::MEDIUM:
        {
            uint32_t currentTime = millis();
            if( (currentTime - lasTime) >= MediumInterval )
            {
                lasTime = currentTime;
                digitalWrite(ledBlink,ledStatus);
                ledStatus = !ledStatus;
            }

            if(buttonEvt.trigger == true)
            {
                buttonEvt.trigger = false;
                if(buttonEvt.whichButton == BUTTONS::B1)
                {
                    taskState = TaskStates::WAIT_ON;
                    printf("Change to Wait: %d\n");
                }
                if (buttonEvt.whichButton == BUTTONS::B2)
                {
                    keyCounter = 0;
                    taskState = TaskStates::SLOW;
                    printf("Change to Slow Mode: %d\n");
                }
            }
            break;
        }

        case TaskStates::WAIT_ON:
        {
            uint32_t currentTime = millis();
            if( (currentTime - lasTime) >= MediumInterval )
            {
                digitalWrite(ledBlink, HIGH);
                taskState = TaskStates::PERMA_ON;
                printf("Change to Perma_ON: %d\n");
            }
            break;
        }

        //Fast Mode (Difficult task)
        case TaskStates::FAST:
        {
            uint32_t currentTime = millis();
            if( (currentTime - lasTime) >= FastInterval )
            {
                lasTime = currentTime;
                digitalWrite(ledBlink,ledStatus);
                ledStatus = !ledStatus;
            }

            if(buttonEvt.trigger == true)
            {
                buttonEvt.trigger = false;
                disarmKey[keyCounter] = buttonEvt.whichButton;
                keyCounter++;
                if (keyCounter == 5)
                {
                    keyCounter = 0;
                    if (compareKeys(secret, disarmKey) == true)
                    {
                        if(switchState == true)
                        {
                            digitalWrite(ledBlink, HIGH);
                            printf("Back to ON\n");
                            taskState = TaskStates::PERMA_ON;
                        }
                        else if(switchState == false)
                        {
                            digitalWrite(ledBlink, LOW);
                            printf("Back to OFF\n");
                            taskState = TaskStates::PERMA_OFF;
                        }
                    }
                    else
                    {
                        printf("Failed Password\n");
                    }
                }
            }
            break;
        }

        //LED PERMA States
        case TaskStates::PERMA_ON:
        {
            if(buttonEvt.trigger == true)
            {
                buttonEvt.trigger = false;
                if(buttonEvt.whichButton == BUTTONS::B1)
                {
                    taskState = TaskStates::MEDIUM;
                    printf("Change to Medium Mode: %d\n");
                }
                else if (buttonEvt.whichButton == BUTTONS::B2)
                {
                    switchState = true;
                    keyCounter = 0;
                    taskState = TaskStates::FAST;
                    printf("Change to Fast Mode: %d\n");
                    printf("Boolean: %d\n", switchState);
                }
            }
            break;
        }

        case TaskStates::PERMA_OFF:
        {
            if(buttonEvt.trigger == true)
            {
                buttonEvt.trigger = false;
                if(buttonEvt.whichButton == BUTTONS::B1)
                {
                    taskState = TaskStates::SLOW;
                    printf("Change to Slow Mode: %d\n");
                }
                else if (buttonEvt.whichButton == BUTTONS::B2)
                {
                    switchState = false;
                    keyCounter = 0;
                    taskState = TaskStates::FAST;
                    printf("Change to Fast Mode: %d\n");
                    printf("Boolean: %d\n", switchState);
                }
            }
            break;
        }

        default:
        {
            break;
        }
    }
}
