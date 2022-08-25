
#include <Arduino.h>
#include "task1.h"
#include "events.h"


void task1(){
 enum class TaskState
    {
        INIT,
        WAIT_COMMANDS
    };
    static TaskState taskState = TaskState::INIT;
    switch (taskState)
    {
    case TaskState::INIT:
    {
        Serial.begin(115200);
        taskState = TaskState::WAIT_COMMANDS;
        break;
    }
    case TaskState::WAIT_COMMANDS:
    {
        if (Serial.available() > 0)
        {
            int dataRx = Serial.read();
            if (dataRx == '1')
            {
                Serial.print("U1 BTN\n");
                buttonEvt.trigger = true;
                buttonEvt.whichButton = BUTTONS::U1_BTN;
            }
            if (dataRx == '2')
            {
                Serial.print("U2 BTN\n");
                buttonEvt.trigger = true;
                buttonEvt.whichButton = BUTTONS::U2_BTN;
            }
        }
        break;
    }
    default:
    {
    }
    }
}