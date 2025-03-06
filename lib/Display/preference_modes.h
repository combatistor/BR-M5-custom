#ifndef PREFERENCE_MODES_H_
#define PREFERENCE_MODES_H_

enum class PreferenceModes : uint8_t
{
    DISPLAY_NEG = 0,
    LED = 1,
    BUZZER = 2,
};

constexpr uint8_t NUM_PREFERENCES_MODES = 3;

// Convert uint8_t to PreferenceModes (unsafe, assumes valid input)
constexpr PreferenceModes toPreferenceMode(uint8_t value)
{
    return static_cast<PreferenceModes>(value);
}

// Convert PreferenceModes to uint8_t (direct cast)
constexpr uint8_t toUint8(PreferenceModes mode)
{
    return static_cast<uint8_t>(mode);
}

inline PreferenceModes operator++(PreferenceModes &mode)
{
    mode = toPreferenceMode((toUint8(mode) + 1) % NUM_PREFERENCES_MODES);
    return mode;
}

inline PreferenceModes operator++(PreferenceModes &mode, int) // postfix operator
{
    PreferenceModes result = mode;
    ++mode;
    return result;
}

inline PreferenceModes operator--(PreferenceModes &mode)
{
    mode = toPreferenceMode((toUint8(mode) - 1) % NUM_PREFERENCES_MODES);
    return mode;
}

inline PreferenceModes operator--(PreferenceModes &mode, int) // postfix operator
{
    PreferenceModes result = mode;
    --mode;
    return result;
}

#endif // PREFERENCE_MODES_H_
