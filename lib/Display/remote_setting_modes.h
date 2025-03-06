#ifndef REMOTE_SETTING_MODES_H_
#define REMOTE_SETTING_MODES_H_

enum class RemoteSettingModes : uint8_t
{
    DELAY = 0,
    SHOTS = 1,
    INTERVAL = 2
};

constexpr int NUM_SETTING_MODES = 3;

// Convert uint8_t to RemoteSettingModes (unsafe, assumes valid input)
constexpr RemoteSettingModes toRemoteSettingMode(uint8_t value)
{
    return static_cast<RemoteSettingModes>(value);
}

// Convert RemoteSettingModes to uint8_t (direct cast)
constexpr uint8_t toUint8(RemoteSettingModes mode)
{
    return static_cast<uint8_t>(mode);
}

inline RemoteSettingModes operator++(RemoteSettingModes &mode)
{
    mode = toRemoteSettingMode((toUint8(mode) + 1) % NUM_SETTING_MODES);
    return mode;
}

inline RemoteSettingModes operator++(RemoteSettingModes &mode, int) // postfix operator
{
    RemoteSettingModes result = mode;
    ++mode;
    return result;
}

inline RemoteSettingModes operator--(RemoteSettingModes &mode)
{
    mode = toRemoteSettingMode((toUint8(mode) - 1) % NUM_SETTING_MODES);
    return mode;
}

inline RemoteSettingModes operator--(RemoteSettingModes &mode, int) // postfix operator
{
    RemoteSettingModes result = mode;
    --mode;
    return result;
}

#endif // REMOTE_SETTING_MODES_H_
