//   Copyright 2015 Silicon Laboratories, Inc.                              *80*

#include PLATFORM_HEADER
#include "stack/include/ember-types.h"
#include "stack/include/event.h"

EmberEventControl emberAfPluginSb1GestureSensorMessageReadyEventControl;

uint8_t halSb1GestureSensorMsgReady(void)
{
  return 0;
}

uint8_t halSb1GestureSensorCheckForMsg(void)
{
  return 0;
}

//void emberAfPluginSb1GestureSensorGestureReceivedCallback(uint8_t gesture,
//                                                           uint8_t buttonNum)
//{
//}

void emAfPluginSb1MessageReady(void)
{
}

void emAfPluginSb1ReadMessage(void)
{
}

void emAfPluginSb1SendGesture(void)
{
}

void emberAfPluginSb1GestureSensorMessageReadyEventHandler(void)
{
}

void emberAfPluginSb1GestureSensorInitCallback(void)
{
}
