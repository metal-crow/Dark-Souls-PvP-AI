#include "vjoyhelper.h"

extern JOYSTICK_POSITION iReport;

void ResetVJoyController(){
    // reset struct info
    iReport.wAxisX = MIDDLE;
    iReport.wAxisY = MIDDLE;
    iReport.wAxisZ = MIDDLE;//this is l2 and r2
    iReport.wAxisYRot = MIDDLE;
    iReport.wAxisXRot = MIDDLE;
    iReport.lButtons = 0x0;
    iReport.bHats = dcenter;//d-pad center
}