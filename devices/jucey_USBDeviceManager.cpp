
class LibUsbDevices : private LibUsbUser
{
public:
    LibUsbDevices() noexcept
    {
        const auto numDevices {(int) libusb_get_device_list (getContext(), &deviceList)};
        devices.addArray (deviceList, numDevices);
    }

    ~LibUsbDevices() noexcept
    {
        libusb_free_device_list (deviceList, true);
    }

    libusb_device* const* begin() const noexcept
    {
        return devices.begin();
    }

    libusb_device* const* end() const noexcept
    {
        return devices.end();
    }

    bool contains (libusb_device* device) const
    {
        return devices.contains (device);
    }

private:
    libusb_device** deviceList;
    juce::Array<libusb_device*> devices;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LibUsbDevices)
};

//==============================================================================
class USBDeviceManager::Pimpl   : private LibUsbUser
                                , public juce::HighResolutionTimer
{
public:
    Pimpl (USBDeviceManager& manager) noexcept
        : manager (manager)
    {
        hiResTimerCallback();
    }
    
    ~Pimpl() noexcept
    {
    }
    
    juce::Array<USBDevice> getDevices() const noexcept
    {
        std::unique_lock<std::recursive_mutex> lock (mutex);
        return devices.get();
    }
    
    void addListener (Listener& listenerToAdd, bool shouldCallBackWithCurrentDevices) noexcept
    {
        std::unique_lock<std::recursive_mutex> lock (mutex);
        
        if (shouldCallBackWithCurrentDevices)
        {
            for (auto& device : devices.get())
                listenerToAdd.deviceArrived (device);
        }
        
        listeners.add (&listenerToAdd);
    }
    
    void removeListener (Listener& listenerToRemove) noexcept
    {
        std::unique_lock<std::recursive_mutex> lock (mutex);
        listeners.remove (&listenerToRemove);
    }
    
private:
    class Devices
    {
    public:
        USBDevice addAndReturn (libusb_device* deviceToAdd) noexcept
        {
            // you shouldn't add a device twice!
            jassert ( ! contains (deviceToAdd));

            libusbDevices.add (deviceToAdd);
            devices.add (USBDevice (std::make_shared<USBDevice::Pimpl>(deviceToAdd)));
            return devices.getLast();
        }

        USBDevice removeAndReturn (libusb_device* deviceToRemove) noexcept
        {
            // you cant remove a device that hasn't been added!
            jassert (contains (deviceToRemove));

            auto device = devices.removeAndReturn (indexOf (deviceToRemove));
            libusbDevices.remove (&deviceToRemove);
            return device;
        }

        int indexOf (libusb_device* deviceToFind) const noexcept
        {
            jassert (contains (deviceToFind));
            return libusbDevices.indexOf (deviceToFind);
        }

        bool contains (libusb_device* deviceToFind) const noexcept
        {
            return libusbDevices.contains (deviceToFind);
        }

        libusb_device* const* begin() const noexcept
        {
            return libusbDevices.begin();
        }

        libusb_device* const* end() const noexcept
        {
            return libusbDevices.end();
        }

        juce::Array<USBDevice> get() const noexcept
        {
            return devices;
        }

    private:
        juce::Array<USBDevice> devices;
        juce::Array<libusb_device*> libusbDevices;
    };
    
    void hiResTimerCallback() override
    {
        std::unique_lock<std::recursive_mutex> lock (mutex);
        LibUsbDevices connectedDevices {};

        // any devices already added will be ignored
        for (const auto& connectedDevice : connectedDevices)
        {
            if ( ! devices.contains (connectedDevice))
            {
                listeners.call (&USBDeviceManager::Listener::deviceArrived,
                                devices.addAndReturn (connectedDevice));
            }
        }

        juce::Array<libusb_device*> devicesToRemove {};

        // find devices to remove, any device that isn't currently connected
        // should be marked for removal
        for (auto& device : devices)
        {
            if ( ! connectedDevices.contains (device))
                devicesToRemove.add (device);
        }

        // remove the devices
        for (const auto& deviceToRemove : devicesToRemove)
        {
            listeners.call (&USBDeviceManager::Listener::deviceRemoved,
                            devices.removeAndReturn (deviceToRemove));
        }
    }
    
    USBDeviceManager& manager;
    Devices devices;
    juce::ListenerList<Listener> listeners;
    mutable std::recursive_mutex mutex;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Pimpl)
};

//==============================================================================
USBDeviceManager& USBDeviceManager::getInstance()
{
    static USBDeviceManager instance;
    return instance;
}

USBDeviceManager::USBDeviceManager() noexcept
    : pimpl (std::make_unique<USBDeviceManager::Pimpl>(*this))
{
    pimpl->startTimer (pollingIntervalMs);
}

USBDeviceManager::~USBDeviceManager() noexcept
{

}

int USBDeviceManager::getPollingIntervalMs() const noexcept
{
    return pollingIntervalMs;
}

void USBDeviceManager::setPollingIntervalMs (int newPollingIntervalMs) noexcept
{
    pollingIntervalMs = newPollingIntervalMs;
}

void USBDeviceManager::pausePolling() noexcept
{
    pimpl->stopTimer();
}

void USBDeviceManager::resumePolling() noexcept
{
    pimpl->startTimer (pollingIntervalMs);
}
    
void USBDeviceManager::addListener (USBDeviceManager::Listener& listenerToAdd,
                                    bool shouldCallBackWithCurrentDevices) noexcept
{
    pimpl->addListener (listenerToAdd, shouldCallBackWithCurrentDevices);
}
    
void USBDeviceManager::removeListener (USBDeviceManager::Listener& listenerToRemove) noexcept
{
    pimpl->removeListener (listenerToRemove);
}

juce::Array<USBDevice> USBDeviceManager::getDevices() const noexcept
{
    return pimpl->getDevices();
}
