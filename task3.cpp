#include <Arduino.h>
#include "task3.h"
#include "events.h"

static bool compareKeys(BUTTONS *pSecret, BUTTONS *pKey)
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
        SLOW,
        MID,
        FAST,
        WAIT_CONFIG,
        OFF,
        ON,


    };
    static TaskStates taskState = TaskStates::INIT;
    const uint8_t led = 14;

    static constexpr uint32_t Lento_vel = 1000;

    static constexpr uint32_t Medio_vel = 500;

    static constexpr uint32_t Rapido_vel = 125;
    static uint32_t lasTime;


    static BUTTONS SecretCodigoRapido[5] = {BUTTONS::B1, BUTTONS::B1, BUTTONS::B2, BUTTONS::B2, BUTTONS::B1};

    static BUTTONS RapidoKey[5] = {BUTTONS::NONE};

    static bool ledState = false;
    static bool lastStateON = false;
    static bool lastStateOFF = false;
    static uint8_t key = 0;

    switch (taskState)
    {
    case TaskStates::INIT:
    {
        pinMode(led, OUTPUT);
        digitalWrite(led, LOW);

        taskState = TaskStates::SLOW;

        break;
    }
    case TaskStates::SLOW:
    {
        uint32_t currentTime = millis();
        if ((currentTime - lasTime) >= Lento_vel){

            lasTime = currentTime;
            digitalWrite(led, ledState);
            ledState = !ledState;
            if (buttonEvt.trigger == true)
            {
                buttonEvt.trigger = false;
                if (buttonEvt.whichButton == BUTTONS::B1)
                {
                    taskState = TaskStates::OFF;
                }
                else if (buttonEvt.whichButton == BUTTONS::B2)
                {

                    taskState = TaskStates::MID;
                }

            }
        }
        break;
    }

    case TaskStates::MID:
    {
        uint32_t currentTime = millis();
        if ((currentTime - lasTime) >= Medio_vel){

            lasTime = currentTime;
            digitalWrite(led, ledState);
            ledState = !ledState;
            if (buttonEvt.trigger == true)
            {
                buttonEvt.trigger = false;
                if (buttonEvt.whichButton == BUTTONS::B1)
                {
                    taskState = TaskStates::ON;
                }
                else if (buttonEvt.whichButton == BUTTONS::B2)
                {
                    taskState = TaskStates::SLOW;
                }

            }
        }
        break;

    }

    case TaskStates::FAST:
    {
        uint32_t currentTime = millis();
        if ((currentTime - lasTime) >= Rapido_vel)
        {

            lasTime = currentTime;
            digitalWrite(led, ledState);
            ledState = !ledState;

        }

        if (buttonEvt.trigger == true)
        {
                buttonEvt.trigger = false;
                RapidoKey[key] = buttonEvt.whichButton;
                key++;

                if (key == 5)
                {
                    key = 0;
                    if (compareKeys(SecretCodigoRapido, RapidoKey) == true)
                    {
                        if (lastStateON == true)
                        {
                            taskState = TaskStates::ON;
                        }
                        else if (lastStateOFF == true)
                        {
                            taskState = TaskStates::OFF;
                        }
                    }
                    else
                    {
                        Serial.print("TA MAL >:C");
                    }
                }

        }

            break;
    }

    case TaskStates::OFF:
    {
        digitalWrite(led, ledState);

        ledState = false;

        if (buttonEvt.trigger == true)
        {
            buttonEvt.trigger = false;
            if (buttonEvt.whichButton == BUTTONS::B1)
            {
                taskState = TaskStates::SLOW;
            }
            else if (buttonEvt.whichButton == BUTTONS::B2)
            {
                taskState = TaskStates::FAST;
                lastStateOFF = true;
                lastStateON = false;
            }
        }
        break;

    }

    case TaskStates::ON:
    {
        digitalWrite(led, ledState);
        ledState = true;

        if (buttonEvt.trigger == true)
        {
            buttonEvt.trigger = false;
            if (buttonEvt.whichButton == BUTTONS::B1)
            {
                taskState = TaskStates::MID;
            }
            else if (buttonEvt.whichButton == BUTTONS::B2)
            {
                taskState = TaskStates::FAST;
                lastStateON = true;
                lastStateOFF = false;
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
