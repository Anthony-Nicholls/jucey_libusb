
//==============================================================================
juce::String getDescriptorString (libusb_device_handle* handle,
                                  uint8_t index) noexcept
{
    if (index == 0)
        return {};

    unsigned char buffer[255];
    libusb_get_string_descriptor_ascii (handle, index, buffer, sizeof(buffer));
    return juce::String (juce::CharPointer_UTF8 (reinterpret_cast<char*>(buffer)));
}

int getPowerUnitsFromSpeed (libusb_speed speed)
{
    switch (speed)
    {
        case LIBUSB_SPEED_UNKNOWN:
            return 0;

        case LIBUSB_SPEED_LOW:
        case LIBUSB_SPEED_FULL:
            return 1;

        case LIBUSB_SPEED_HIGH:
            return 2;

        case LIBUSB_SPEED_SUPER:
        case LIBUSB_SPEED_SUPER_PLUS:
            return 8;

        default:
            jassertfalse;
            return 0;
    }
}

//==============================================================================
struct LibUsbConfig
{
    LibUsbConfig (libusb_device* device, int index) noexcept
    {
        libusb_get_config_descriptor (device, index, &descriptor);
    }

    LibUsbConfig (libusb_device* device) noexcept
    {
        libusb_get_active_config_descriptor (device, &descriptor);
    }

    ~LibUsbConfig() noexcept
    {
        if (descriptor != nullptr)
            libusb_free_config_descriptor (descriptor);
    }

    bool operator== (const LibUsbConfig& other) const noexcept
    {
        return descriptor != nullptr
            && other.descriptor != nullptr
            && descriptor->bConfigurationValue == other.descriptor->bConfigurationValue;
    }

    bool operator!= (const LibUsbConfig& other) const noexcept
    {
        return descriptor == nullptr
            || other.descriptor == nullptr
            || descriptor->bConfigurationValue != other.descriptor->bConfigurationValue;
    }

    libusb_config_descriptor* descriptor {nullptr};
};

//==============================================================================
struct LibUsbDevice
{
    LibUsbDevice (libusb_device* dev) noexcept
        : device (dev)
    {
        libusb_open (device, &handle);
    }

    ~LibUsbDevice() noexcept
    {
        libusb_close (handle);
    }

    libusb_device* device = nullptr;
    libusb_device_handle* handle = nullptr;
};

//==============================================================================
class USBDevice::Pimpl : public LibUsbDevice
{
public:
    Pimpl() = default;
    ~Pimpl() = default;

    Pimpl (libusb_device* device) noexcept
        : LibUsbDevice (device)
        , descriptor (getDeviceDescriptor (device))
        , speed ((libusb_speed) libusb_get_device_speed (device))
        , manufacturerName (getDescriptorString (handle, descriptor.iManufacturer))
        , productName (getDescriptorString (handle, descriptor.iProduct))
        , serialNumber (getDescriptorString (handle, descriptor.iSerialNumber))
    {

    }

    const libusb_device_descriptor descriptor {};

    const libusb_speed speed {LIBUSB_SPEED_UNKNOWN};

    const juce::String manufacturerName {};
    const juce::String productName {};
    const juce::String serialNumber {};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Pimpl)
};

//==============================================================================
class USBDevice::Configuration::Pimpl : public LibUsbConfig
{
public:
    Pimpl() = default;
    ~Pimpl() = default;

    Pimpl (const USBDevice& device, int index) noexcept
        : LibUsbConfig (device.pimpl->device, index)
        , numberOfInterfaces (descriptor->bNumInterfaces)
        , milliampsRequired (getPowerUnitsFromSpeed (device.pimpl->speed) * descriptor->MaxPower)
        , description (getDescriptorString (device.pimpl->handle, descriptor->iConfiguration))

    {

    }

    const int numberOfInterfaces = 0;
    const int milliampsRequired = 0;
    const juce::String description;
};

//==============================================================================
USBDevice::USBDevice (const std::shared_ptr<Pimpl>& pimpl) noexcept
    : pimpl (pimpl)
{
    jassert (pimpl != nullptr);
    jassert (pimpl->device != nullptr);

    for (auto index {0}; index < pimpl->descriptor.bNumConfigurations; ++index)
        configurations.add (Configuration (std::make_shared<USBDevice::Configuration::Pimpl>(*this, index)));
}

bool USBDevice::operator== (const USBDevice& other) const noexcept
{
    return pimpl == other.pimpl;
}

bool USBDevice::operator!= (const USBDevice& other) const noexcept
{
    return pimpl != other.pimpl;
}

int USBDevice::getVendorId() const noexcept
{
    jassert (pimpl != nullptr);
    return pimpl->descriptor.idVendor;
}

int USBDevice::getProductId() const noexcept
{
    jassert (pimpl != nullptr);
    return pimpl->descriptor.idProduct;
}

juce::String USBDevice::getManufacturerName() const noexcept
{
    jassert (pimpl != nullptr);
    return pimpl->manufacturerName;
}

juce::String USBDevice::getProductName() const noexcept
{
    jassert (pimpl != nullptr);
    return pimpl->productName;
}

juce::String USBDevice::getSerialNumber() const noexcept
{
    jassert (pimpl != nullptr);
    return pimpl->serialNumber;
}

int USBDevice::getBusNumber() const noexcept
{
    jassert (pimpl != nullptr);
    jassert (pimpl->device != nullptr);
    return libusb_get_bus_number (pimpl->device);
}

int USBDevice::getPortNumber() const noexcept
{
    jassert (pimpl != nullptr);
    jassert (pimpl->device != nullptr);
    return libusb_get_port_number (pimpl->device);
}

int USBDevice::getAddress() const noexcept
{
    jassert (pimpl != nullptr);
    jassert (pimpl->device != nullptr);
    return libusb_get_device_address (pimpl->device);
}

juce::String USBDevice::getUSBSpecificationVersionString() const noexcept
{
    jassert (pimpl != nullptr);
    return "USB " + getVersionStringFromBinaryCodedDecimal (pimpl->descriptor.bcdUSB);
}

int USBDevice::getUSBSpecificationVersion() const noexcept
{
    jassert (pimpl != nullptr);
    return pimpl->descriptor.bcdUSB;
}

int USBDevice::getUSBSpecificationVersionMajor() const noexcept
{
    jassert (pimpl != nullptr);
    return getMajorVersionFromBinaryCodedDecimal (pimpl->descriptor.bcdUSB);
}

int USBDevice::getUSBSpecificationVersionMinor() const noexcept
{
    jassert (pimpl != nullptr);
    return getMinorVersionFromBinaryCodedDecimal (pimpl->descriptor.bcdUSB);
}

juce::String USBDevice::getVersionString() const noexcept
{
    jassert (pimpl != nullptr);
    return getVersionStringFromBinaryCodedDecimal (pimpl->descriptor.bcdDevice);
}

int USBDevice::getVersion() const noexcept
{
    jassert (pimpl != nullptr);
    return pimpl->descriptor.bcdDevice;
}

int USBDevice::getVersionMajor() const noexcept
{
    jassert (pimpl != nullptr);
    return getMajorVersionFromBinaryCodedDecimal (pimpl->descriptor.bcdDevice);
}

int USBDevice::getVersionMinor() const noexcept
{
    jassert (pimpl != nullptr);
    return getMinorVersionFromBinaryCodedDecimal (pimpl->descriptor.bcdDevice);
}

juce::String USBDevice::getSpeedString() const noexcept
{
    jassert (pimpl != nullptr);

    switch (pimpl->speed)
    {
        case LIBUSB_SPEED_UNKNOWN:
            return "Unknown";
            
        case LIBUSB_SPEED_LOW:
            return "1.5 MBit/s";
            
        case LIBUSB_SPEED_FULL:
            return "12 MBit/s";
            
        case LIBUSB_SPEED_HIGH:
            return "480 MBit/s";
            
        case LIBUSB_SPEED_SUPER:
            return "5 GBit/s";
            
        case LIBUSB_SPEED_SUPER_PLUS:
            return "10 GBit/s";
            
        default:
            jassertfalse;
            return "Unknown";
    }
}

float USBDevice::getSpeedMbps() const noexcept
{
    jassert (pimpl != nullptr);

    switch (pimpl->speed)
    {
        case LIBUSB_SPEED_UNKNOWN:
            return 0.f;
            
        case LIBUSB_SPEED_LOW:
            return 1.5f;
            
        case LIBUSB_SPEED_FULL:
            return 12.f;
            
        case LIBUSB_SPEED_HIGH:
            return 480.f;
            
        case LIBUSB_SPEED_SUPER:
            return 5000.f;
            
        case LIBUSB_SPEED_SUPER_PLUS:
            return 10000.f;
            
        default:
            jassertfalse;
            return 0.f;
    }
}

USBDevice::Configuration USBDevice::getActiveConfiguration() const noexcept
{
    jassert (pimpl != nullptr);
    jassert (pimpl->device != nullptr);

    const LibUsbConfig activeConfig {pimpl->device};

    for (const auto& config : configurations)
    {
        if (activeConfig == *config.pimpl)
            return config;
    }

    return {};
}

juce::Array<USBDevice::Configuration> USBDevice::getConfigurations() const noexcept
{
    return configurations;
}

int USBDevice::getCurrentMilliampsRequired() const noexcept
{
    return getActiveConfiguration().getMilliampsRequired();
}

int USBDevice::getMaximumMilliampsRequired() const noexcept
{
    auto milliamps {0};

    for (const auto& config : getConfigurations())
        milliamps = juce::jmax (milliamps, config.getMilliampsRequired());

    return milliamps;
}

//==============================================================================
USBDevice::Configuration::Configuration (const std::shared_ptr<Pimpl>& pimpl) noexcept
    : pimpl (pimpl)
{

}

int USBDevice::Configuration::getMilliampsRequired() const noexcept
{
    if (pimpl == nullptr)
        return 0;

    return pimpl->milliampsRequired;
}
