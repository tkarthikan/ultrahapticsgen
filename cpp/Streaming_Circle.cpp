/**
 * Streaming_Circle.cpp
 *
 * This example uses a StreamingEmitter to project a ControlPoint moving
 * in a circle, 20cm above the centre of the array. The radius of the
 * circle can be changed during playback.
 */

#include <cmath>
#include <iostream>
#include <chrono>
#include <thread>
#include <string>
#include "ultraleap/haptics/streaming.hpp"

#ifndef M_PI
    #define M_PI 3.14159265358979323
#endif

using namespace Ultraleap::Haptics;

//It's important to use double to prevent precision issues with long run times
using Seconds = std::chrono::duration<double>;

// Structure for passing data to the callback function
struct CircleData
{
    double radius;
    double control_point_speed;
    float control_point_intensity;
    LocalTimePoint start_time;
};

/**
 * Callback function to calculate the position of the circle at each time point
 *
 * This uses the control_point_speed and the elapsed time to calculate an
 * angle around the circle.
 *
 * This uses the EmissionCallback signature.
 */
void circle_callback(const StreamingEmitter& emitter,
    OutputInterval& interval,
    const LocalTimePoint& submission_deadline,
    void* circle_data_ptr)
{
    auto circle_data = static_cast<CircleData*>(circle_data_ptr);
    double angular_frequency = circle_data->control_point_speed / circle_data->radius;

    for (auto& sample : interval) {
        Seconds t = sample - circle_data->start_time;
        double angle = t.count() * angular_frequency;

        Vector3 p;
        p.x = static_cast<float>(std::cos(angle) * circle_data->radius);
        p.y = static_cast<float>(std::sin(angle) * circle_data->radius);
        p.z = 0.2f;
        
        sample.controlPoint(0).setPosition(p);
        sample.controlPoint(0).setIntensity(circle_data->control_point_intensity);
    }
}

int main()
{
    // Create a Library object and connect it to a running service
    Library lib;
    auto connect_result = lib.connect();
    if (!connect_result) {
        std::cout << "Library failed to connect: " << connect_result.error().message() << std::endl;
        return 1;
    }

    // Create a streaming emitter and add a device to it
    StreamingEmitter emitter{ lib };
    auto device_result = lib.findDevice(DeviceFeatures::StreamingHaptics);
    if (!device_result) {
        std::cout << "Failed to find device: " << device_result.error().message() << std::endl;
        return 1;
    }

    auto add_result = emitter.addDevice(device_result.value());
    if (!add_result) {
        std::cout << "Failed to add device: " << add_result.error().message() << std::endl;
        return 1;
    }

    //Set the ControlPoint count on the emitter to be 1
    auto set_count_result = emitter.setControlPointCount(1, AdjustRate::All);
    if (!set_count_result) {
        std::cout << "Failed to set ControlPoint count: " << set_count_result.error().message() << std::endl;
        return 1;
    }

    CircleData circle_data{ 0.02, 8.0, 1.0, LocalTimeClock::now() };

    auto set_cb_result = emitter.setEmissionCallback(&circle_callback, &circle_data);
    if (!set_cb_result) {
        std::cout << "Failed to set callback on emitter: " << set_cb_result.error().message() << std::endl;
        return 1;
    }

    auto start_result = emitter.start();
    if (!start_result) {
        std::cout << "Failed to start emitter: " << start_result.error().message() << std::endl;
        return 1;
    }

    // Wait for enter key to be pressed.
    std::cout << "Hit ENTER to quit..." << std::endl;
    while (true) {
        std::cout << "Enter radius (mm): ";
        std::cout << std::cin.rdbuf();
        std::string line;
        std::getline(std::cin, line);
        if (line.empty())
            break;
        try {
            float mm = std::stof(line);
            circle_data.radius = mm / 1000.0f;

            
        } catch (std::invalid_argument e) {
            std::cout << "Unable to parse as number: " << line << std::endl;
        }
    }

    // Stop the callback
    auto stop_result = emitter.stop();
    if (!stop_result) {
        std::cout << "Failed to stop callback: " << stop_result.error().message() << std::endl;
        return 1;
    }

    return 0;
}
