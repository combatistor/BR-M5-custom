#ifndef MAIN_PROGRAMS_H_
#define MAIN_PROGRAMS_H_

enum class MainPrograms : uint8_t
{
    REMOTE = 0,
    TIMELAPSE = 1,
    PREFERENCES = 2
};

constexpr int NUM_MODES = 3;

// Convert uint8_t to MainPrograms (unsafe, assumes valid input)
constexpr MainPrograms toMainPrograms(uint8_t value)
{
    return static_cast<MainPrograms>(value);
}

// Convert MainPrograms to uint8_t (direct cast)
constexpr uint8_t toUint8(MainPrograms program)
{
    return static_cast<uint8_t>(program);
}

inline MainPrograms operator++(MainPrograms &program)
{
    program = toMainPrograms((toUint8(program) + 1) % NUM_MODES);
    return program;
}

inline MainPrograms operator++(MainPrograms &mode, int) // postfix operator
{
    MainPrograms result = mode;
    ++mode;
    return result;
}

inline MainPrograms operator--(MainPrograms &program)
{
    program = toMainPrograms((toUint8(program) - 1) % NUM_MODES);
    return program;
}

inline MainPrograms operator--(MainPrograms &mode, int) // postfix operator
{
    MainPrograms result = mode;
    --mode;
    return result;
}

#endif // MAIN_PROGRAMS_H_
