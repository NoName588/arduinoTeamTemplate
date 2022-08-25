#include <Arduino.h>
#include "task2.h"
#include "events.h"

void task2(){
enum class TaskState
    {
        INIT,
        WAIT_PRESS,
        WAIT_STABLE,
        WAIT_RELEASE
    };
    static TaskState taskState = TaskState::INIT;
    static uint8_t lastButtonPressed;
    static uint32_t initStableTime;

    const uint8_t U1_BTN = 32;
    const uint8_t U2_BTN = 33;

    const uint32_t STABLE_TIME = 100;

    switch (taskState)
    {
    case TaskState::INIT:
    {
        pinMode(U1_BTN, INPUT_PULLUP);
        pinMode(U2_BTN, INPUT_PULLUP);

        taskState = TaskState::WAIT_PRESS;
        break;
    }
    case TaskState::WAIT_PRESS:
    {
        if(digitalRead(U1_BTN) == LOW){
            lastButtonPressed =U1_BTN;
            initStableTime = millis();
            taskState = TaskState::WAIT_STABLE;
        }
        if(digitalRead(U2_BTN) == LOW){
            lastButtonPressed = U2_BTN;
            initStableTime = millis();
            taskState = TaskState::WAIT_STABLE;
        }

        break;
    }
    case TaskState::WAIT_STABLE:
    {
        if(digitalRead(lastButtonPressed) != LOW){
            taskState = TaskState::WAIT_PRESS;
        }
        else if ( (millis() - initStableTime) > STABLE_TIME){
            if(lastButtonPressed ==U1_BTN) buttonEvt.whichButton = BUTTONS::U1_BTN;
            else if(lastButtonPressed == U2_BTN) buttonEvt.whichButton = BUTTONS::U2_BTN;

            buttonEvt.trigger = true;
            printf("Button pressed: %d\n", lastButtonPressed);
            taskState = TaskState::WAIT_RELEASE;
        }
        break;
    }
    case TaskState::WAIT_RELEASE:{
        if(digitalRead(lastButtonPressed) == HIGH){
            taskState = TaskState::WAIT_PRESS;
        }
        break;
    }

    default:
    {
    }
    }
}