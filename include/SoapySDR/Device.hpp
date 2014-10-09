///
/// \file SoapySDR/Device.hpp
///
/// Interface definition for Soapy SDR devices.
///
/// \copyright
/// Copyright (c) 2014-2014 Josh Blum
/// SPDX-License-Identifier: BSL-1.0
///

#pragma once
#include <SoapySDR/Config.hpp>
#include <SoapySDR/Types.hpp>
#include <SoapySDR/Constants.h>
#include <vector>
#include <string>
#include <complex>
#include <cstddef> //size_t

namespace SoapySDR
{

//! Forward declaration of stream handle for type safety
class Stream;

/*!
 * Abstraction for an SDR tranceiver device - configuration and streaming.
 */
class SOAPY_SDR_API Device
{
public:

    //! virtual destructor for inheritance
    virtual ~Device(void);

    /*!
     * Enumerate a list of available devices on the system.
     * \param args device construction key/value argument filters
     * \return a list of argument maps, each unique to a device
     */
    static std::vector<Kwargs> enumerate(const Kwargs &args);

    /*!
     * Make a new Device object given device construction args.
     * The device pointer will be stored in a table so subsequent calls
     * with the same arguments will produce the same device.
     * For every call to make, there should be a matched call to unmake.
     * \param args device construction key/value argument map
     * \return a pointer to a new Device object
     */
    static Device *make(const Kwargs &args);

    /*!
     * Unmake or release a device object handle.
     * \param device a pointer to a device object
     */
    static void unmake(Device *device);

    /*******************************************************************
     * Channels API
     ******************************************************************/

    /*!
     * Set the frontend mapping of available DSP units to RF frontends.
     * This mapping controls channel mapping and channel availability.
     * \param direction the channel direction RX or TX
     * \param mapping a vendor-specific mapping string
     */
    virtual void setFrontendMapping(const int direction, const std::string &mapping);

    /*!
     * Get the mapping configuration string.
     * \param direction the channel direction RX or TX
     * \param the vendor-specific mapping string
     */
    virtual std::string getFrontendMapping(const int direction) const;

    /*!
     * Get a number of channels given the streaming direction
     */
    virtual size_t getNumChannels(const int direction) const;

    /*******************************************************************
     * Stream API
     ******************************************************************/

    /*!
     * Initialize a stream given a list of channels and stream arguments.
     * The implementation may change switches or power-up components.
     *
     * Format string markup guidelines:
     *  - C means complex
     *  - F means floating point
     *  - U means signed integer
     *  - S means unsigned integer
     *  - number float/int size in bytes (complex is 2x this size)
     *
     * Example format strings:
     *  - CF32 complex float32 (8 bytes per element)
     *  - CS16 complex int16 (4 bytes per element)
     *  - CS12 complex int12 (3 bytes per element)
     *  - CS4 complex int4 (1 byte per element)
     *  - S32 int32 (4 bytes per element)
     *  - U8 uint8 (1 byte per element)
     *
     * Recommended keys to use in the args dictionary:
     *  - "WIRE" - format of the samples between device and host
     *
     * \param direction the channel direction RX or TX
     * \param format the desired buffer format in read/writeStream()
     * \param channels a list of channels for empty for automatic
     * \param args stream args or empty for defaults
     * \return an opaque pointer to a stream handle
     */
    virtual Stream *setupStream(
        const int direction,
        const std::string &format,
        const std::vector<size_t> &channels = std::vector<size_t>(),
        const Kwargs &args = Kwargs());

    /*!
     * Close an open stream created by setupStream
     * The implementation may change switches or power-down components.
     * \param stream the opaque pointer to a stream handle
     */
    virtual void closeStream(Stream *stream);

    /*!
     * Read elements from a stream for reception.
     * This is a multi-channel call, and buffs should be an array of void *,
     * where each pointer will be filled with data from a different channel.
     *
     * \param stream the opaque pointer to a stream handle
     * \param buffs an array of void* buffers num chans in size
     * \param numElems the number of elements in each buffer
     * \param flags optional flag indicators about the result
     * \param timeNs the buffer's timestamp in nanoseconds
     * \param timeoutUs the timeout in microseconds
     * \return the number of elements read per buffer or error code
     */
    virtual int readStream(
        Stream *stream,
        void * const *buffs,
        const size_t numElems,
        int &flags,
        long long &timeNs,
        const long timeoutUs = 100000);

    /*!
     * Write elements to a stream for transmission.
     * This is a multi-channel call, and buffs should be an array of void *,
     * where each pointer will be filled with data for a different channel.
     *
     * \param stream the opaque pointer to a stream handle
     * \param buffs an array of void* buffers num chans in size
     * \param numElems the number of elements in each buffer
     * \param flags optional input flags and output flags
     * \param timeNs the buffer's timestamp in nanoseconds
     * \param timeoutUs the timeout in microseconds
     * \return the number of elements written per buffer or error
     */
    virtual int writeStream(
        Stream *stream,
        const void * const *buffs,
        const size_t numElems,
        int &flags,
        const long long timeNs,
        const long timeoutUs = 100000);

    /*******************************************************************
     * Antenna API
     ******************************************************************/

    /*!
     * Get a list of available antennas to select on a given chain.
     * \param direction the channel direction RX or TX
     * \param channel an available channel on the device
     * \return a list of available antenna names
     */
    virtual std::vector<std::string> listAntennas(const int direction, const size_t channel) const;

    /*!
     * Set the selected antenna on a chain.
     * \param direction the channel direction RX or TX
     * \param channel an available channel on the device
     * \param name the name of an available antenna
     */
    virtual void setAntenna(const int direction, const size_t channel, const std::string &name);

    /*!
     * Get the selected antenna on a chain.
     * \param direction the channel direction RX or TX
     * \param channel an available channel on the device
     * \return the name of an available antenna
     */
    virtual std::string getAntenna(const int direction, const size_t channel) const;

    /*******************************************************************
     * Frontend corrections API
     ******************************************************************/

    /*!
     * Set the frontend DC offset correction.
     * \param direction the channel direction RX or TX
     * \param channel an available channel on the device
     * \param offset the relative correction (1.0 max)
     */
    virtual void setDCOffset(const int direction, const size_t channel, const std::complex<double> &offset);

    /*!
     * Get the frontend DC offset correction.
     * \param direction the channel direction RX or TX
     * \param channel an available channel on the device
     * \return the relative correction (1.0 max)
     */
    virtual std::complex<double> getDCOffset(const int direction, const size_t channel) const;

    /*!
     * Set the frontend IQ balance correction.
     * \param direction the channel direction RX or TX
     * \param channel an available channel on the device
     * \param balance the relative correction (1.0 max)
     */
    virtual void setIQBalance(const int direction, const size_t channel, const std::complex<double> &balance);

    /*!
     * Get the frontend IQ balance correction.
     * \param direction the channel direction RX or TX
     * \param channel an available channel on the device
     * \return the relative correction (1.0 max)
     */
    virtual std::complex<double> getIQBalance(const int direction, const size_t channel) const;

    /*******************************************************************
     * Gain API
     ******************************************************************/

    /*!
     * List available amplification elements.
     * \param direction the channel direction RX or TX
     * \param channel an available channel
     * \return a list of gain string names
     */
    virtual std::vector<std::string> listGains(const int direction, const size_t channel) const;

    /*!
     * Set the automatic gain mode on the chain.
     * \param direction the channel direction RX or TX
     * \param channel an available channel on the device
     * \param mode true for automatic gain setting
     */
    virtual void setGainMode(const int direction, const size_t channel, const bool mode);

     /*!
     * Get the automatic gain mode on the chain.
     * \param direction the channel direction RX or TX
     * \param channel an available channel on the device
     * \return true for automatic gain setting
     */
    virtual bool getGainMode(const int direction, const size_t channel) const;

    /*!
     * Set the overall amplification in a chain.
     * The gain will be distributed automatically across available element.
     * \param direction the channel direction RX or TX
     * \param channel an available channel on the device
     * \param name the name of an amplification element
     * \param value the new amplification value in dB
     */
    virtual void setGain(const int direction, const size_t channel, const double value);

    /*!
     * Set the value of a amplification element in a chain.
     * \param direction the channel direction RX or TX
     * \param channel an available channel on the device
     * \param name the name of an amplification element
     * \param value the new amplification value in dB
     */
    virtual void setGain(const int direction, const size_t channel, const std::string &name, const double value);

    /*!
     * Get the overall value of the gain elements in a chain.
     * \param direction the channel direction RX or TX
     * \param channel an available channel on the device
     * \return the value of the gain in dB
     */
    virtual double getGain(const int direction, const size_t channel) const;

    /*!
     * Get the value of an individual amplification element in a chain.
     * \param direction the channel direction RX or TX
     * \param channel an available channel on the device
     * \param name the name of an amplification element
     * \return the value of the gain in dB
     */
    virtual double getGain(const int direction, const size_t channel, const std::string &name) const;

    /*!
     * Get the overall range of possible gain values.
     * \param direction the channel direction RX or TX
     * \param channel an available channel on the device
     * \return a list of gain ranges in dB
     */
    virtual Range getGainRange(const int direction, const size_t channel) const;

    /*!
     * Get the range of possible gain values for a specific element.
     * \param direction the channel direction RX or TX
     * \param channel an available channel on the device
     * \param name the name of an amplification element
     * \return a list of gain ranges in dB
     */
    virtual Range getGainRange(const int direction, const size_t channel, const std::string &name) const;

    /*******************************************************************
     * Frequency API
     ******************************************************************/

    /*!
     * Set the center frequency of the chain.
     * Recommended keys to use in the args dictionary:
     *  - "CORR" - freq error correction in PPM
     *  - "OFFSET" - offset for the RF frontend
     *  - "RF" - frequency of the RF frontend
     *  - "BB" - frequency of the baseband DSP
     * \param direction the channel direction RX or TX
     * \param channel an available channel on the device
     * \param frequency the center frequency in Hz
     * \param args optional tuner arguments
     */
    virtual void setFrequency(const int direction, const size_t channel, const double frequency, const Kwargs &args = Kwargs());

    /*!
     * Get the center frequency of the chain.
     * \param direction the channel direction RX or TX
     * \param channel an available channel on the device
     * \return the center frequency in Hz
     */
    virtual double getFrequency(const int direction, const size_t channel) const;

    /*!
     * Get the frequency of a tunable element in the chain.
     * Recommended names used to represent tunable components:
     *  - "CORR" - freq error correction in PPM
     *  - "RF" - frequency of the RF frontend
     *  - "BB" - frequency of the baseband DSP
     * \param direction the channel direction RX or TX
     * \param channel an available channel on the device
     * \param name the name of an frequency component
     * \return a dictionary of tunable elements to frequencies in Hz
     */
    virtual double getFrequency(const int direction, const size_t channel, const std::string &name) const;

    /*!
     * List available tunable elements in the chain.
     * \param direction the channel direction RX or TX
     * \param channel an available channel
     * \return a list of tunable elements by name
     */
    virtual std::vector<std::string> listFrequencies(const int direction, const size_t channel) const;

    /*!
     * Get the range of possible frequency values.
     * \param direction the channel direction RX or TX
     * \param channel an available channel on the device
     * \return a list of frequency ranges in Hz
     */
    virtual RangeList getFrequencyRange(const int direction, const size_t channel) const;

    /*******************************************************************
     * Sample Rate API
     ******************************************************************/

    /*!
     * Set the baseband sample rate of the chain.
     * \param direction the channel direction RX or TX
     * \param channel an available channel on the device
     * \param rate the sample rate in samples per second
     */
    virtual void setSampleRate(const int direction, const size_t channel, const double rate);

    /*!
     * Get the baseband sample rate of the chain.
     * \param direction the channel direction RX or TX
     * \param channel an available channel on the device
     * \return the sample rate in samples per second
     */
    virtual double getSampleRate(const int direction, const size_t channel) const;

    /*!
     * Get the range of possible baseband sample rates.
     * \param direction the channel direction RX or TX
     * \param channel an available channel on the device
     * \return a list of possible rates in samples per second
     */
    virtual std::vector<double> listSampleRates(const int direction, const size_t channel) const;

    /*!
     * Set the baseband filter width of the chain.
     * \param direction the channel direction RX or TX
     * \param channel an available channel on the device
     * \param bw the baseband filter width in Hz
     */
    virtual void setBandwidth(const int direction, const size_t channel, const double bw);

    /*!
     * Get the baseband filter width of the chain.
     * \param direction the channel direction RX or TX
     * \param channel an available channel on the device
     * \return the baseband filter width in Hz
     */
    virtual double getBandwidth(const int direction, const size_t channel) const;

    /*!
     * Get the range of possible baseband filter widths.
     * \param direction the channel direction RX or TX
     * \param channel an available channel on the device
     * \return a list of possible bandwidths in Hz
     */
    virtual std::vector<double> listBandwidths(const int direction, const size_t channel) const;

    /*******************************************************************
     * Clocking API
     ******************************************************************/

    /*!
     * Set the master clock rate of the device.
     * \param rate the clock rate in Hz
     */
    virtual void setMasterClockRate(const double rate);

    /*!
     * Get the master clock rate of the device.
     * \return the clock rate in Hz
     */
    virtual double getMasterClockRate(void) const;

    /*!
     * Get the list of available clock sources.
     * \return a list of clock source names
     */
    virtual std::vector<std::string> listClockSources(void) const;

    /*!
     * Set the clock source on the device
     * \param source the name of a clock source
     */
    virtual void setClockSource(const std::string &source);

    /*!
     * Get the clock source of the device
     * \return the name of a clock source
     */
    virtual std::string getClockSource(void) const;

    /*!
     * Get the list of available time sources.
     * \return a list of time source names
     */
    virtual std::vector<std::string> listTimeSources(void) const;

    /*!
     * Set the time source on the device
     * \param source the name of a time source
     */
    virtual void setTimeSource(const std::string &source);

    /*!
     * Get the time source of the device
     * \return the name of a time source
     */
    virtual std::string getTimeSource(void) const;

    /*******************************************************************
     * Time API
     ******************************************************************/

    /*!
     * Read the time from the hardware clock on the device.
     * The what argument can refer to a specific time counter.
     * \param what optional argument
     * \return the time in nanoseconds
     */
    virtual long long getHardwareTime(const std::string &what = "") const;

    /*!
     * Write the time to the hardware clock on the device.
     * The what argument can refer to a specific time counter.
     * \param timeNs time in nanoseconds
     * \param what optional argument
     */
    virtual void setHardwareTime(const long long timeNs, const std::string &what = "");

    /*!
     * Set the time of subsequent configuration calls.
     * The what argument can refer to a specific command queue.
     * Implementations may use a time of 0 to clear.
     * \param timeNs time in nanoseconds
     * \param what optional argument
     */
    virtual void setCommandTime(const long long timeNs, const std::string &what = "");

};

};
