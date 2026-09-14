#pragma once

#include <cstdint>
#include "Midi/MidiPort.hpp"
#include "Midi/MidiMessage.hpp"
#include "Interfaces/IMidiOutput.hpp"

/**
 * @brief A single entry in the MIDI routing table
 */
struct RouteEntry {
    MidiPort sourceMask;    // which input port(s) this rule matches
    MidiPort destMask;      // where to forward matching messages
    uint8_t  msgTypeMask;   // bitmask of MidiMessage::Type values (MidiTypeMask::*)
    uint8_t  channelMin;    // 1-16 inclusive, 0 = any
    uint8_t  channelMax;    // 1-16 inclusive, 0 = any
    bool     enabled;

    /** @brief Check whether a message matches this route entry */
    bool matches(const MidiMessage& msg) const {
        if (!enabled) return false;
        if (!hasPort(sourceMask, msg.source)) return false;
        if (!(msgTypeMask & (1 << msg.type))) return false;
        if (channelMin != 0 && msg.channel != 0) {
            if (msg.channel < channelMin || msg.channel > channelMax) return false;
        }
        return true;
    }
};

/**
 * @brief Central MIDI routing engine
 * 
 * Routes MidiMessage objects from input ports to output ports according
 * to a configurable routing table. Each output port is backed by an
 * IMidiOutput implementation.
 */
class MidiRouter {
public:
    static constexpr uint8_t MAX_ROUTES = 16;

    MidiRouter();

    // --- Port registration ---
    void setOutput(MidiPort port, IMidiOutput* output);
    IMidiOutput* getOutput(MidiPort port) const;

    // --- Routing table ---
    bool addRoute(MidiPort source, MidiPort dest,
                  uint8_t msgTypeMask = MidiTypeMask::ALL,
                  uint8_t channelMin = 0, uint8_t channelMax = 0);
    void removeRoute(uint8_t index);
    void clearRoutes();
    void setDefaultRoutes();
    uint8_t getRouteCount() const { return _routeCount; }
    const RouteEntry& getRoute(uint8_t index) const { return _routes[index]; }
    void enableRoute(uint8_t index, bool enabled);

    // --- Message dispatch ---
    void route(const MidiMessage& msg);

    // --- Clock routing configuration ---
    void setClockOutputPorts(MidiPort destMask);
    MidiPort getClockOutputPorts() const { return _clockOutputPorts; }
    void setClockInputSource(MidiPort sourceMask);
    MidiPort getClockInputSource() const { return _clockInputSource; }

private:
    MidiPort _clockOutputPorts = MidiPort::USB_DEVICE | MidiPort::DIN;
    MidiPort _clockInputSource = MidiPort::USB_DEVICE | MidiPort::DIN | MidiPort::USB_HOST;
    void sendToPort(MidiPort port, const MidiMessage& msg);
    void dispatchToOutput(IMidiOutput* output, const MidiMessage& msg);

    IMidiOutput* _outputs[MIDI_PORT_COUNT];
    RouteEntry   _routes[MAX_ROUTES];
    uint8_t      _routeCount;
};
