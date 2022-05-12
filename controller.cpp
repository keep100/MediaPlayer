#include "controller.h"

Controller* Controller::_controller = nullptr;
Controller::Controller(QObject *parent)
    : QObject{parent}
{

}
