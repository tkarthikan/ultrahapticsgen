/**
 * Streaming_Line.cpp
 *
 * This example uses a StreamingEmitter to project a ControlPoint
 * moving in a line 20cm above the centre of the array.
 */

#include <cmath>
#include <iostream>
#include <chrono>
#include <thread>
#include <string>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#define PORT 12345 

#include "ultraleap/haptics/streaming.hpp"

#ifndef M_PI
    #define M_PI 3.14159265358979323
#endif

using namespace Ultraleap::Haptics;

//It's important to use double to prevent precision issues with long run times
using Seconds = std::chrono::duration<double>;

int i = 0;

// Structure for passing data to the callback function
struct LineData
{
    Vector3 central_position;
    Vector3 line_direction;
    float length;

    float control_point_speed;
    float control_point_intensity;

    LocalTimePoint start_time;
};

/**
 * Callback function for the Line
 *
 * This uses the EmissionCallback signature.
 */
void line_callback(const StreamingEmitter& emitter,
    OutputInterval& interval,
    const LocalTimePoint& submission_deadline,
    void* line_data_ptr)
{
    auto line_data = static_cast<LineData*>(line_data_ptr);
    double line_round_trip_period = 2.0 * static_cast<double>(line_data->length / line_data->control_point_speed);

    for (auto& sample : interval) {
        Seconds t = sample - line_data->start_time;

        // Make number between 0 and 1 which says how far through the round-trip we are
        double round_trip_progress = std::fmod(t.count(), line_round_trip_period) / line_round_trip_period;

        // Make a number between 0 and 1 which says how far along the line we are
        double line_progress = 2.0 * round_trip_progress;
        if (line_progress > 1.0)
            line_progress = 2.0 - line_progress;

        // Calculate the line position
        float direction_multiplier = static_cast<float>(line_progress - 0.5) * line_data->length;
        Vector3 position1 = line_data->central_position + direction_multiplier * line_data->line_direction;
        Vector3 position;
        float arr[7][3] = {{1, 0, 0.53119385},{2,2, 0.5769292},{3, 2, 0.64225787},{4, 0, 0.68382156}, {5, 0, 0.7131509}, {6, 0, 0.7462115}, {7, 0, 0.7881137}};
// 
        position = Vector3(arr[i][0]/150, arr[i][1]/150, 0.2f);
        i = i + 1;

        if (i == (sizeof(arr) / sizeof(arr[0]))){
            i=0;
        }

        //position = Vector3(0, 0, 0.2f);
        //std::cout << "Time: " << i << std::endl;
        sample.controlPoint(0).setPosition(position1);
        sample.controlPoint(0).setIntensity(1.0f);
    }
}

int main()
{
    // Create a Library object and connect it to a running service
    Library lib;
    auto connect_result = lib.connect();
    // if (!connect_result) {
    //     std::cout << "Library failed to connect: " << connect_result.error().message() << std::endl;
    //     return 1;
    // }

    // // Create a streaming emitter and add a device to it
    // StreamingEmitter emitter{ lib };
    // auto device_result = lib.findDevice(DeviceFeatures::StreamingHaptics);
    // if (!device_result) {
    //     std::cout << "Failed to find device: " << device_result.error().message() << std::endl;
    //     return 1;
    // }

    // auto add_result = emitter.addDevice(device_result.value());
    // if (!add_result) {
    //     std::cout << "Failed to add device: " << add_result.error().message() << std::endl;
    //     return 1;
    // }

    // // Set the maximum control point count
    // auto set_count_result = emitter.setControlPointCount(1, AdjustRate::All);
    // if (!set_count_result) {
    //     std::cout << "Failed to setControlPointCount: " << set_count_result.error().message() << std::endl;
    //     return 1;
    // }

    // LineData line_data;
    // line_data.central_position = Vector3(0, 0, 0.2f); // 20cm above the array
    // line_data.line_direction = Vector3(1, 0, 0);      // in the x-direction
    // line_data.length = 0.08f;                         // 8cm long
    // line_data.control_point_speed = 8.0f;             // moving at 8 metres per second;
    // line_data.control_point_intensity = 1.0f;         // maximum intensity;
    // line_data.start_time = LocalTimeClock::now();     // starting now;

    // auto set_cb_result = emitter.setEmissionCallback(&line_callback, &line_data);
    // if (!set_cb_result) {
    //     std::cout << "Failed to set callback on emitter: " << set_cb_result.error().message() << std::endl;
    //     return 1;
    // }

    // auto start_result = emitter.start();
    // if (!start_result) {
    //     std::cout << "Failed to start emitter: " << start_result.error().message() << std::endl;
    //     return 1;
    // }

    //Socket section
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    char *hello = "Hello from client";
    char buffer[1024] = {0};
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
       
    // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0) 
    {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }
   
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed \n");
        return -1;
    }
    
    send(sock , hello , strlen(hello) , 0 );
    printf("Hello message sent\n");
    valread = read( sock , buffer, 1024);
    //printf("%s\n",buffer );
    std::vector<float> v;

    // Build an istream that holds the input string
    std::istringstream iss(buffer);

    // Iterate over the istream, using >> to grab floats
    // and push_back to store them in the vector
    std::copy(std::istream_iterator<float>(iss),
            std::istream_iterator<float>(),
            std::back_inserter(v));

    // Put the result on standard out
    std::copy(v.begin(), v.end(),
            std::ostream_iterator<float>(std::cout, ", "));
    std::cout << "\n";
    //Socket end

    char buf;
    fread(&buf, 1, 1, stdin);
    // Wait for user input to quit.
    std::cout << "Hit ENTER to quit..."<<buf<< std::endl;
    std::string input;
    std::getline(std::cin, input);


    // Stop the callback
    auto stop_result = emitter.stop();
    if (!stop_result) {
        std::cout << "Failed to stop callback: " << stop_result.error().message() << std::endl;
        return 1;
    }

    return 0;
}

//[8, 0, 0.7224702],
// [9, 0, 0.6033514],
// [10, 0, 0.54148895],
// [8, 1, 0.66521686],
// [9, 1, 0.75878686]];