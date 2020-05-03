
#pragma once

class USBDevice
{
public:
    /** Default constructor. */
    USBDevice() = default;

    /** Destructor. */
    ~USBDevice() = default;

    /** Copy constructor. */
    USBDevice (const USBDevice&) = default;

    /** Assignment operator. */
    USBDevice& operator= (const USBDevice&) = default;

    /** Returns the Vendor ID. */
    int getVendorId() const noexcept;

    /** Returns the Product ID. */
    int getProductId() const noexcept;

    /** Returns the manufacturer name. */
    juce::String getManufacturerName() const noexcept;

    /** Returns the product name. */
    juce::String getProductName() const noexcept;

    /** Returns the serial number. */
    juce::String getSerialNumber() const noexcept;

    /** Retruns the USB specification version number as a binary-coded decimal

        A value of 0x0200 indicates USB 2.0, 0x0110 indicates USB 1.1, etc
     */
    int getUSBSpecificationVersion() const noexcept;

    /** Returns the major version of the USB specification version number. */
    int getUSBSpecificationVersionMajor() const noexcept;

    /** Returns the minor version of the USB specification version number. */
    int getUSBSpecificationVersionMinor() const noexcept;

    /** Returns the USB specification version number as a string. */
    juce::String getUSBSpecificationVersionString() const noexcept;

    /** Retruns the device version number in binary-coded decimal. */
    int getVersion() const noexcept;

    /** Retruns the major version of the Device version number. */
    int getVersionMajor() const noexcept;

    /** Retruns the minor version of the Device version number. */
    int getVersionMinor() const noexcept;

    /** Retruns the Device version number as a string. */
    juce::String getVersionString() const noexcept;

    /** Returns the negotiated connection speed in MBit/s. */
    float getSpeedMbps() const noexcept;

    /** Returns the negotiated connection speed as a string. */
    juce::String getSpeedString() const noexcept;

    /** Returns a unique identifier for this device for the bus it is on. */
    int getAddress() const noexcept;

    /** Returns the bus this device is sonnected to. */
    int getBusNumber() const noexcept;

    /** Returns the port number the device is connected to on the bus. */
    int getPortNumber() const noexcept;

    class Configuration
    {
    public:
        /** Default constructor. */
        Configuration() = default;

        /** Destructor. */
        ~Configuration() = default;

        /** Returns the maximum power consumption in milliamps based on this
            configuration.
         */
        int getMilliampsRequired() const noexcept;

    private:
        friend class USBDevice;

        class Pimpl;
        std::shared_ptr<Pimpl> pimpl;

        /** Internal constructor. */
        Configuration (const std::shared_ptr<Pimpl>& pimpl) noexcept;
    };

    /** Returns the configuration currently in use. */
    Configuration getActiveConfiguration() const noexcept;

    /** Returns all possible configurations. */
    juce::Array<Configuration> getConfigurations() const noexcept;

    /** Returns the maximum power consumption in milliamps based on the active
        configuration.
     */
    int getCurrentMilliampsRequired() const noexcept;

    /** Returns the maximum power consumption in milliamps based on all the
        avaliable configurations.
     */
    int getMaximumMilliampsRequired() const noexcept;

    /** Comparison operators */
    bool operator== (const USBDevice& other) const noexcept;
    bool operator!= (const USBDevice& other) const noexcept;

private:
    friend class USBDeviceManager;
    
    class Pimpl;
    std::shared_ptr<Pimpl> pimpl;

    /** Internal constructor. */
    USBDevice (const std::shared_ptr<Pimpl>& pimpl) noexcept;

    juce::Array<Configuration> configurations;
    
    JUCE_LEAK_DETECTOR (USBDevice)
};
