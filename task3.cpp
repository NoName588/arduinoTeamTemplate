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
        WAIT_CONFIG,
        APAGADO,
        ENCENDIDO,
        LENTO,
        MEDIO,
        RAPIDO,

    };
    static TaskStates taskState = TaskStates::INIT;
    const uint8_t led = 14;
    static uint32_t lasTime;
    static constexpr uint32_t LentoINTERVAL = 1000;
    static constexpr uint32_t MedioINTERVAL = 500;
    static constexpr uint32_t RapidoINTERVAL = 125;


    static BUTTONS SecretCodigoRapido[5] = {BUTTONS::B1, BUTTONS::B1,
                                                    BUTTONS::B2, BUTTONS::B2,
                                                    BUTTONS::B1};

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

        taskState = TaskStates::LENTO;

        break;
    }
    case TaskStates::LENTO:
    {
        uint32_t currentTime = millis();
        if ((currentTime - lasTime) >= LentoINTERVAL){

            lasTime = currentTime;
            digitalWrite(led, ledState);
            ledState = !ledState;
            if (buttonEvt.trigger == true)
            {
                buttonEvt.trigger = false;
                if (buttonEvt.whichButton == BUTTONS::B1)
                {
                    taskState = TaskStates::APAGADO;
                }
                else if (buttonEvt.whichButton == BUTTONS::B2)
                {

                    taskState = TaskStates::MEDIO;
                }

            }
        }
        break;
    }

    case TaskStates::MEDIO:
    {
        uint32_t currentTime = millis();
        if ((currentTime - lasTime) >= MedioINTERVAL){

            lasTime = currentTime;
            digitalWrite(led, ledState);
            ledState = !ledState;
            if (buttonEvt.trigger == true)
            {
                buttonEvt.trigger = false;
                if (buttonEvt.whichButton == BUTTONS::B1)
                {
                    taskState = TaskStates::ENCENDIDO;
                }
                else if (buttonEvt.whichButton == BUTTONS::B2)
                {
                    taskState = TaskStates::LENTO;
                }

            }
        }
        break;

    }

    case TaskStates::RAPIDO:
    {
        uint32_t currentTime = millis();
        if ((currentTime - lasTime) >= RapidoINTERVAL)
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
                            taskState = TaskStates::ENCENDIDO;
                        }
                        else if (lastStateOFF == true)
                        {
                            taskState = TaskStates::APAGADO;
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

    case TaskStates::APAGADO:
    {
        digitalWrite(led, ledState);

        ledState = false;

        if (buttonEvt.trigger == true)
        {
            buttonEvt.trigger = false;
            if (buttonEvt.whichButton == BUTTONS::B1)
            {
                taskState = TaskStates::LENTO;
            }
            else if (buttonEvt.whichButton == BUTTONS::B2)
            {
                taskState = TaskStates::RAPIDO;
                lastStateOFF = true;
                lastStateON = false;
            }
        }
        break;

    }

    case TaskStates::ENCENDIDO:
    {
        digitalWrite(led, ledState);
        ledState = true;

        if (buttonEvt.trigger == true)
        {
            buttonEvt.trigger = false;
            if (buttonEvt.whichButton == BUTTONS::B1)
            {
                taskState = TaskStates::MEDIO;
            }
            else if (buttonEvt.whichButton == BUTTONS::B2)
            {
                taskState = TaskStates::RAPIDO;
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
