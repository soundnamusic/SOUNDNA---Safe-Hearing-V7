cmake_minimum_required(VERSION 3.22)
project(SoundnaListen VERSION 1.5.1 LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
option(SOUNDNA_CORE_ONLY "Build model tests without JUCE" OFF)
enable_testing()
add_executable(timer_tests tests/timer_tests.cpp)
add_test(NAME timer_model COMMAND timer_tests)
add_executable(report_tests tests/report_tests.cpp)
add_test(NAME report_writer COMMAND report_tests)
if(SOUNDNA_CORE_ONLY)
    return()
endif()
set(JUCE_SOURCE_DIR "" CACHE PATH "Optional local JUCE 8.0.12 checkout")
if(JUCE_SOURCE_DIR)
    add_subdirectory("${JUCE_SOURCE_DIR}" JUCE)
else()
    include(FetchContent)
    FetchContent_Declare(JUCE GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
        GIT_TAG 29396c22c93392d6738e021b83196283d6e4d850)
    FetchContent_MakeAvailable(JUCE)
endif()
set(formats VST3)
if(APPLE)
    list(APPEND formats AU)
endif()
option(SOUNDNA_ENABLE_AAX "Requires separately obtained Avid SDK and signing" OFF)
set(AAX_SDK_PATH "" CACHE PATH "Avid AAX SDK root")
if(SOUNDNA_ENABLE_AAX)
    if(NOT EXISTS "${AAX_SDK_PATH}/Interfaces/AAX.h")
        message(FATAL_ERROR "Set AAX_SDK_PATH to a valid Avid SDK root")
    endif()
    juce_set_aax_sdk_path("${AAX_SDK_PATH}")
    list(APPEND formats AAX)
endif()
juce_add_plugin(SoundnaListen
    COMPANY_NAME SOUNDNA
    BUNDLE_ID com.soundna.listen
    PLUGIN_MANUFACTURER_CODE Snda
    PLUGIN_CODE Slsn
    PRODUCT_NAME "SOUNDNA - Safe Hearing"
    FORMATS ${formats}
    IS_SYNTH FALSE NEEDS_MIDI_INPUT FALSE NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD FALSE
    VST3_CATEGORIES Fx Tools
    AU_MAIN_TYPE kAudioUnitType_Effect
    AAX_IDENTIFIER com.soundna.listen)
target_sources(SoundnaListen PRIVATE Source/PluginProcessor.cpp Source/PluginEditor.cpp)
target_compile_definitions(SoundnaListen PUBLIC JUCE_WEB_BROWSER=0 JUCE_USE_CURL=0 JUCE_VST3_CAN_REPLACE_VST2=0)
target_link_libraries(SoundnaListen PRIVATE juce::juce_audio_utils
    PUBLIC juce::juce_recommended_config_flags juce::juce_recommended_warning_flags)

add_executable(state_tests tests/state_tests.cpp)
target_compile_definitions(state_tests PRIVATE JUCE_USE_CURL=0 JUCE_WEB_BROWSER=0)
target_link_libraries(state_tests PRIVATE juce::juce_core)
add_test(NAME state_roundtrip COMMAND state_tests)

add_executable(preferences_tests tests/preferences_tests.cpp)
target_compile_definitions(preferences_tests PRIVATE JUCE_USE_CURL=0 JUCE_WEB_BROWSER=0)
target_link_libraries(preferences_tests PRIVATE juce::juce_data_structures)
add_test(NAME global_preferences COMMAND preferences_tests)
