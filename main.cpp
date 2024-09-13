#include "mbed.h"
#include "controller.h"
int main()
{
    // device.attach(&EnemyDetect3, SerialBase::RxIrq);
    Timer timer;
 while (true) {
        controller.CheckStartTime();
        if(StartFlag) {
            if(timer.read() == 0) timer.start();
            if(timer.read() < 5) {
                if(controller.GetWallSafetyState()) controller.SetSpeed(+0.5);
                else controller.SetSpeed(-0.5);
            }
            if(timer.read() > 5 && timer.read() < 10) {
                controller.SetSpeed(1.0,1.0);
            }
            if(timer.read() > 10) controller.SetSpeed(0,0);
            controller.Move(controller.GetSpeedL(),controller.GetSpeedR());
        } 
        ThisThread::sleep_until(controller.GetStartTime() + 10); //절대 시간으로 10ms 만큼 쉬기
    }
}
