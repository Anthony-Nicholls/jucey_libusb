
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
class USBDevice::Pimpl : private LibUsbDevice
{
public:
    Pimpl() = default;
    ~Pimpl() = default;

    Pimpl (libusb_device* device) noexcept
        : LibUsbDevice (device)
        , descriptor (getDeviceDescriptor (device))
        , vendorId (descriptor.idVendor)
        , productId (descriptor.idProduct)
        , busNumber (libusb_get_bus_number (device))
        , portNumber (libusb_get_port_number (device))
        , address (libusb_get_device_address (device))
        , speed ((libusb_speed) libusb_get_device_speed (device))
        , usbSpecification (descriptor.bcdUSB)
        , version (descriptor.bcdDevice)
        , manufacturerName (getDescriptorString (handle, descriptor.iManufacturer))
        , productName (getDescriptorString (handle, descriptor.iProduct))
        , serialNumber (getDescriptorString (handle, descriptor.iSerialNumber))
    {

    }

    const libusb_device_descriptor descriptor {};

    const int vendorId {0};
    const int productId {0};
    const int busNumber {0};
    const int portNumber {0};
    const int address {0};
    const libusb_speed speed {LIBUSB_SPEED_UNKNOWN};
    const int usbSpecification {0};
    const int version {0};

    const juce::String manufacturerName {};
    const juce::String productName {};
    const juce::String serialNumber {};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Pimpl)
};

//==============================================================================
USBDevice::USBDevice (const std::shared_ptr<Pimpl>& pimpl) noexcept
    : pimpl (pimpl)
//    , configurations (USBDeviceConfiguration::createConfigurationsFromDevice (*this))
{

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
    return pimpl->vendorId;
}

int USBDevice::getProductId() const noexcept
{
    return pimpl->productId;
}

juce::String USBDevice::getManufacturerName() const noexcept
{
    return pimpl->manufacturerName;
}

juce::String USBDevice::getProductName() const noexcept
{
    return pimpl->productName;
}

juce::String USBDevice::getSerialNumber() const noexcept
{
    return pimpl->serialNumber;
}

int USBDevice::getBusNumber() const noexcept
{
    return pimpl->busNumber;
}

int USBDevice::getPortNumber() const noexcept
{
    return pimpl->portNumber;
}

int USBDevice::getAddress() const noexcept
{
    return pimpl->address;
}

juce::String USBDevice::getUSBSpecificationVersionString() const noexcept
{
    return "USB " + getVersionStringFromBinaryCodedDecimal (pimpl->usbSpecification);
}

int USBDevice::getUSBSpecificationVersion() const noexcept
{
    return pimpl->usbSpecification;
}

int USBDevice::getUSBSpecificationVersionMajor() const noexcept
{
    return getMajorVersionFromBinaryCodedDecimal (pimpl->usbSpecification);
}

int USBDevice::getUSBSpecificationVersionMinor() const noexcept
{
    return getMinorVersionFromBinaryCodedDecimal (pimpl->usbSpecification);
}

juce::String USBDevice::getVersionString() const noexcept
{
    return getVersionStringFromBinaryCodedDecimal (pimpl->version);
}

int USBDevice::getVersion() const noexcept
{
    return pimpl->version;
}

int USBDevice::getVersionMajor() const noexcept
{
    return getMajorVersionFromBinaryCodedDecimal (pimpl->version);
}

int USBDevice::getVersionMinor() const noexcept
{
    return getMinorVersionFromBinaryCodedDecimal (pimpl->version);
}

juce::String USBDevice::getSpeedString() const noexcept
{
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
