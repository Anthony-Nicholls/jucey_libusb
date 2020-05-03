
#pragma once

class USBDeviceManager
{
public:
    /** Returns the one and only instance of this object */
    static USBDeviceManager& getInstance();

    /** Desstructor. */
    ~USBDeviceManager() noexcept;

    /** Retruns the number of milliseconds between polling events. */
    int getPollingIntervalMs() const noexcept;

    /** Sets the interval between polling events in milliseconds. */
    void setPollingIntervalMs (int newPollingIntervalMs) noexcept;

    /** Pauses polling for devices. */
    void pausePolling() noexcept;

    /** Resumes polling for devices. */
    void resumePolling() noexcept;

    /** Returns an array of the currently connected devices. */
    juce::Array<USBDevice> getDevices() const noexcept;
    
    class Listener
    {
    public:
        /** Destructor. */
        virtual ~Listener() {}

        /** Called back when a new USB device is attached. */
        virtual void deviceArrived (const USBDevice& device) = 0;

        /** Called back when a USB device is removed. */
        virtual void deviceRemoved (const USBDevice& device) = 0;
    };

    /** Add a listener to be called back when USB devices are added or removed.

        @param listenerToAdd                        The listener that will recieve callbacks.

        @param shouldCallBackWithCurrentDevices     If true this will trigger `deviceArrived`
                                                    callbacks for all currently connected
                                                    devices to the listener being added.
     */
    void addListener (Listener& listenerToAdd, bool shouldCallBackWithCurrentDevices) noexcept;

    /** Remove a listener to prevent it recieving further callabcks. */
    void removeListener (Listener& listenerToRemove) noexcept;
    
private:
    /** Default constructor. */
    USBDeviceManager() noexcept;

    class Pimpl;
    std::unique_ptr<Pimpl> pimpl;
    int pollingIntervalMs {250};
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (USBDeviceManager)
};
