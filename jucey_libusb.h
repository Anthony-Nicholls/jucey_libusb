
/*******************************************************************************
 The block below describes the properties of this module, and is read by
 the Projucer to automatically generate project code that uses it.

 BEGIN_JUCE_MODULE_DECLARATION

 ID:                jucey_libusb
 vendor:            jucey
 version:           0.0.0
 name:              JUCEY libusb classes
 description:       A collection of friendly classes that wrap up the libusb library

 dependencies:      juce_core
 searchpaths:       libusb/libusb config
 OSXFrameworks:     IOKit CoreFoundation

 END_JUCE_MODULE_DECLARATION

 *******************************************************************************/

#pragma once

#include "juce_core/juce_core.h"

#include "devices/jucey_USBDevice.h"
#include "devices/jucey_USBDeviceManager.h"
