
#if defined (_WIN32) || defined (_WIN64)
 #include "../libusb/msvc/config.h"
#elif defined (JUCE_ANDROID)
 #include "../libusb/android/config.h"
#elif defined (__APPLE_CPP__) || defined (__APPLE_CC__)
 #include "../libusb/Xcode/config.h"
#endif
