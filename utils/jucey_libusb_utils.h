
#pragma once

void throwOnLibUsbError (int result)
{
    if (result == libusb_error::LIBUSB_SUCCESS)
        return;

    std::stringstream errorMessage;
    errorMessage << libusb_error_name (result);
    errorMessage << ": ";
    errorMessage << libusb_strerror ((libusb_error)result);
    throw std::runtime_error (errorMessage.str());
}

libusb_device_descriptor getDeviceDescriptor (libusb_device* device) noexcept
{
    libusb_device_descriptor descriptor {};
    libusb_get_device_descriptor (device, &descriptor);

    return descriptor;
}

int getMajorVersionFromBinaryCodedDecimal (int versionNumber) noexcept
{
    // remove the bottom 8 bits (minor version) by shifting the major version to
    // the bottom
    return versionNumber >> 8;
}

int getMinorVersionFromBinaryCodedDecimal (int versionNumber) noexcept
{
    // keep only the bottom 8 bits (minor version)
    return versionNumber & 0xff;
}

juce::String getVersionStringFromBinaryCodedDecimal (int binaryCodedDecimal) noexcept
{
    juce::StringArray versions;
    versions.add (juce::String::toHexString (getMajorVersionFromBinaryCodedDecimal (binaryCodedDecimal)));
    versions.add (juce::String::toHexString (getMinorVersionFromBinaryCodedDecimal (binaryCodedDecimal)).paddedLeft ('0', 2));

    return versions.joinIntoString (".");
}

//==============================================================================
class LibUsbUser
{
public:
    libusb_context* getContext() const noexcept
    {
        return contextManager->context;
    }

private:
    struct ContextManager
    {
        ContextManager() noexcept
        {
            libusb_init (&context);
        }

        ~ContextManager() noexcept
        {
            libusb_exit (context);
        }

        libusb_context* context = nullptr;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ContextManager)
    };

    juce::SharedResourcePointer<ContextManager> contextManager;

    JUCE_LEAK_DETECTOR (LibUsbUser)
};
