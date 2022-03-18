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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ultraleap/haptics/streaming.hpp"

#ifndef M_PI
    #define M_PI 3.14159265358979323
#endif

#define EXPORT_DECORATION 

using namespace Ultraleap::Haptics;

//It's important to use double to prevent precision issues with long run times
using Seconds = std::chrono::duration<double>;

int current_sample = 0;
int sample_size;


// Structure for passing data to the callback function
struct CircleData
{
    double radius;
    double control_point_speed;
    float control_point_intensity;
    LocalTimePoint start_time;
    double *x_samplesl;
    std::atomic<double> *x_samples; //pointer for allocation dynamic of X positions
    std::atomic<double> *y_samples; //pointer for allocation dynamic of Y positions
    std::atomic<double> *z_samples; //pointer for allocation dynamic of Z positions
    std::atomic<double> *i_samples; //pointer for allocation dynamic of Intensities
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
    int i = 0;
    float tstart;
    //std::cout << "Output Interval: " << interval. << std::endl;
    //std::cout << "Submission Deadline: " << submission_deadline << std::endl;

    for (auto& sample : interval) {
        Seconds t = sample - circle_data->start_time;
        double angle = t.count() * angular_frequency;

        if (i==0){
            tstart = t.count();
        }
        //std::cout << "Test i: " << i << std::endl;
        //std::cout << "T count i: " << t.count() << std::endl;
        i++;

        if (i==20){
            //std::cout << "Time elapsed: " << t.count() - tstart << std::endl;
        }

        Vector3 p;
        p.x = static_cast<float>(circle_data->x_samples[current_sample]);
        p.y = static_cast<float>(circle_data->y_samples[current_sample]);
        p.z = 0.2f;
        
        current_sample +=1;
		if(current_sample > (sample_size-1))
		{
			current_sample = 0;
			//emit_on = false;
		}
        sample.controlPoint(0).setPosition(p);
        sample.controlPoint(0).setIntensity(circle_data->control_point_intensity);
    }
}

// some functions accessible through the dll interface
extern "C"{

    std::string filename = "/Users/karthik/Documents/GenHap/UH_Multithread/build/lineseven4_8_100hz_0005ss.csv";
    int restartflag = 0; 

    void alter_me(char* param, size_t length) {
        // truncates if additional info exceeds length
        std::cout << "Fname found"<<std::endl;
        filename = param;
        std::cout <<filename<<std::endl;
        //strncat(param, length, " adding some info", length - 1);
    }

    

	void csvfile()
	{   
        std::cout << "Filename check: " <<filename<<std::endl;
	}

    // int restartUH() 
    // {

    // }

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
        // File pointer
        //std::string filename = "lineseven001_all"; //Covers in 0.01s
        //std::string filename = "lineseven0_01_old.csv";
        
        //std::string filename = "lineseven4_8_100hz.csv";
        //std::string filename = "lineseven4_8_100hz_005s.csv";
        //std::string filename = "lineseven0_01.csv"; //Covers second part in 0.005s
        //std::string filename = "lineseven001_50hz.csv"; //Covers second part in 0.005s
        //std::string filename = "lineseven001_75hz.csv"; //Covers second part in 0.005s

        std::ifstream fin(filename);
        if (!fin) printf("file not found");
        //std::fstream file;
        // Open an existing file
        // /file.open("square.csv", std::ios::in);
    
        // Get the roll number
        // of which the data is required
        int rowcount = 0;
        // Read the Data from the file
        // as String Vector
        std::vector<std::string> row;
        std::string line, word, temp;
        
        while (fin >> temp) {
            rowcount+=1;
        }
        sample_size = rowcount;
        printf("Row count %d", rowcount);

        int size = sample_size;
        circle_data.i_samples = (std::atomic<double> *) malloc(size*sizeof(std::atomic<double>));
        circle_data.x_samples = (std::atomic<double> *) malloc(size*sizeof(std::atomic<double>));
        circle_data.y_samples = (std::atomic<double> *) malloc(size*sizeof(std::atomic<double>));
        circle_data.z_samples = (std::atomic<double> *) malloc(size*sizeof(std::atomic<double>));

        std::ifstream file(filename);
        float data[sample_size][3];
        for(int row = 0; row < sample_size; row++)
        {
            std::string line;
            std::getline(file, line);
            if ( !file.good() )
                break;

            std::stringstream iss(line);

            for (int col = 0; col < 4; col++)
            {
                std::string val;
                std::getline(iss, val, ',');

                std::stringstream convertor(val);
                convertor >> data[row][col];

                printf("Row: %d  ",row);
                printf("Col: %d  ",col);
                printf("%f\n",data[row][col]);
                if (col==0)
                    circle_data.i_samples[row] = data[row][col];
                else if (col==1)
                    circle_data.x_samples[row] = data[row][col];
                else if (col==2)
                    circle_data.y_samples[row] = data[row][col];
                else 
                    circle_data.z_samples[row] = data[row][col];

                if ( !iss.good() )
                    break;
            }
        }

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

        // Wait for user input to quit.
        std::cout << "Hit ENTER to quit..." << std::endl;
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
}
