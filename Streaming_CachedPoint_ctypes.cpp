// This example uses the Streaming emitter to project a static control
// point 20cm above the centre of the array. The point is modulated using a
// cosine

#include <cmath>
#include <iostream>
#include <vector>
#include <cstddef>
#include <string>
#include <thread>
#include <atomic>
#include <chrono>
#include <thread>

#include "ultraleap/haptics/streaming.hpp"
#include <ultraleap/haptics/driver_device.hpp>
#include <ultraleap/haptics/research.hpp>
#include <ultraleap/haptics/state_emitters.hpp>


#ifndef M_PI
#define M_PI 3.14159265358979323
#endif

// Export decoration are needed for windows but not for mac
#ifdef _WIN32
	#define EXPORT_DECORATION __declspec(dllexport)
#else
	#define EXPORT_DECORATION 
#endif

using namespace Ultraleap::Haptics;
using Seconds = std::chrono::duration<double>;
//Some global variables to be used within this code:
static auto start_time = LocalTimeClock::now(); //time for trigger
static int sent_trigger = 0; 

//some global variables to be used in CTYPES:
extern "C"{
	EXPORT_DECORATION std::atomic<double> *x_samples; //pointer for allocation dynamic of X positions
	EXPORT_DECORATION std::atomic<double> *y_samples; //pointer for allocation dynamic of Y positions
	EXPORT_DECORATION std::atomic<double> *z_samples; //pointer for allocation dynamic of Z positions
	EXPORT_DECORATION std::atomic<double> *i_samples; //pointer for allocation dynamic of Intensities
	//std::atomic<double> i_samples[40000]; //pointer for allocation dynamic of Intensities

	EXPORT_DECORATION std::atomic<bool> is_on = false; //Will be set to true when device is initiated. Once set to false, turn off arrays
	EXPORT_DECORATION std::atomic<bool> emit_on = false; //Will be set to true, to start callback. Will be set to false, when run out of data

	EXPORT_DECORATION std::atomic<int> samples_size;
	EXPORT_DECORATION std::atomic<int> current_sample;
	
	EXPORT_DECORATION std::atomic<double> cap_ave = 1800;
	EXPORT_DECORATION std::atomic<double> cap_peak = 2000;
	EXPORT_DECORATION std::atomic<double> cap_range = 2000;
	
	EXPORT_DECORATION std::atomic<double> update_rate = 40000;
	EXPORT_DECORATION std::atomic<double> device_update_rate = -1;

	EXPORT_DECORATION std::atomic<int> emit_mode = 0; //0 STAM, 1 AM, 2 STM
	
	EXPORT_DECORATION std::atomic<int> *sensors;
	EXPORT_DECORATION std::atomic<int> n_sensor = 0;
	
	EXPORT_DECORATION std::atomic<int> trig_period;
	EXPORT_DECORATION std::atomic<int> trig_rise;
	EXPORT_DECORATION std::atomic<int> trig_fall;
	EXPORT_DECORATION std::atomic< bool> trigger_on = false;
	
}

// Structure for passing information over time for our control point
struct UserData
{
    explicit UserData(unsigned int timepointCount)
        : timepoints(timepointCount)
    {}

    std::size_t next_index = 0;
    std::vector<Ultrahaptics::ControlPoint> timepoints;
};


// Callback function for filling out complete device output states through time
typedef StreamingStateEmitter<ControlPointState> CSEmitter;
typedef StateInterval<ControlPointState> CSInterval;

void my_emitter_callback(
    CSEmitter &emitter,
    CSInterval &interval,
    const LocalTimePoint &interval_begin,
    const LocalTimePoint &interval_end,
    const LocalTimePoint &deadline,
    void *user_data_ptr)
{

    // The user pointer allows us to store data we can re-use in our callback.
    // In this case, we've used a custom struct that describes the precomputed control point
    // data for the number of samples required to represent a complete period of our waveform.
    UserData* user_data = static_cast<UserData*>(user_data_ptr);

    //Loop through the samples in this interval
    for (auto &sample : interval) {
        // Update the control point at this time point with our precomputed value
		
		if(trigger_on)
		{
			Seconds t = sample.time() - start_time;
			if( t.count() < trig_rise && emit_on == true)
			{
				emit_on = false;
			}	
			else if(t.count() >= trig_rise && t.count() < trig_fall && sent_trigger == 0)
			{
				//set the trigger to high and switch the trigger flag to 1
				const_cast<DriverDevice *>(emitter.getDeviceAt(0).value().getDriverDevice().value().get())->sendV4TriggerSignalRise(sample.time());
				sent_trigger = 1;
				
				//reset sample index to 0 and start emission
				current_sample = 0;
				emit_on = true;
			}
			else if(t.count() >= trig_fall && t.count() <trig_period && sent_trigger == 1)
			{
				//set the trigger to low and switch the trigger flag to 0
				const_cast<DriverDevice *>(emitter.getDeviceAt(0).value().getDriverDevice().value().get())->sendV4TriggerSignalFall(sample.time());
				sent_trigger = 0;
				
				//stop emission
				emit_on = false;
			}
			else if(t.count() >= trig_period)
			{
				//reset timer for next trigger period
				start_time = LocalTimeClock::now();
			}
		}


		float tmp_i, tmp_x, tmp_y, tmp_z; 
		if(emit_on)
		{
			if(emit_mode == 0)
			{
				tmp_i = i_samples[current_sample];
				tmp_x = x_samples[current_sample];
				tmp_y = y_samples[current_sample];
				tmp_z = z_samples[current_sample];
			}
			else if(emit_mode == 1)
			{
				tmp_i = i_samples[current_sample];
				tmp_x = 0;
				tmp_y = 0;
				tmp_z = 0.2;
			}
			else if(emit_mode == 2)
			{
				tmp_i = 1;
				tmp_x = x_samples[current_sample];
				tmp_y = y_samples[current_sample];
				tmp_z = z_samples[current_sample];
			}
		}
		else
		{
        	//sample.controlPoint(0).setIntensity(0);
			tmp_i = 0;
		}
		
		uint32_t group = 0;
        *sample.state(0) = {&tmp_x, &tmp_y, &tmp_z, &tmp_i, &group, 1};

		current_sample +=1;
		if(current_sample > samples_size)
		{
			current_sample = 0;
			emit_on = false;
		}
		
    }
}

using namespace Ultrahaptics;


// some functions accessible through the dll interface
extern "C"{

	EXPORT_DECORATION int set_intensities(double* array, int size)
	{

		samples_size = size;
		i_samples = (std::atomic<double> *) malloc(size*sizeof(std::atomic<double>));


		for(int i =0; i<size; i++)
		{
			i_samples[i] = array[i];
		}
		return size;
	}

	EXPORT_DECORATION int set_positions(double* x, double* y, double* z, int size)
	{
		
		samples_size = size;
		x_samples = (std::atomic<double> *) malloc(size*sizeof(std::atomic<double>));
		y_samples = (std::atomic<double> *) malloc(size*sizeof(std::atomic<double>));
		z_samples = (std::atomic<double> *) malloc(size*sizeof(std::atomic<double>));

		for(int i =0; i<size; i++)
		{
			x_samples[i] = x[i];
			y_samples[i] = y[i];
			z_samples[i] = z[i];
		}
		return size;

	}

	EXPORT_DECORATION int set_samples(double* a, double* x, double* y, double* z, int size)
	{	
		samples_size = size;
		i_samples = (std::atomic<double> *) malloc(size*sizeof(std::atomic<double>));
		x_samples = (std::atomic<double> *) malloc(size*sizeof(std::atomic<double>));
		y_samples = (std::atomic<double> *) malloc(size*sizeof(std::atomic<double>));
		z_samples = (std::atomic<double> *) malloc(size*sizeof(std::atomic<double>));

		for(int i =0; i<size; i++)
		{
			i_samples[i] = a[i];
			x_samples[i] = x[i];
			y_samples[i] = y[i];
			z_samples[i] = z[i];
		}

		return size;
	}

	EXPORT_DECORATION void play_sensation(int mode)
	{
		emit_mode = mode;
		current_sample = 0;
		emit_on = true;
	}
	
	EXPORT_DECORATION void set_capping(double ave, double peak, double range)
	{
		cap_ave = ave;
		cap_peak = peak;
		cap_range = range;
		//std::cout<< ave << peak << range;
	}
	
	EXPORT_DECORATION void set_updateRate(double rate)
	{
		update_rate = rate;
	}
	
	EXPORT_DECORATION void set_trigger(double period, double t1 = 0.01, double t2 = 0.011)
	{
		trig_period = period;
		trig_rise = t1;
		trig_fall = t2;
	}
	
	EXPORT_DECORATION void start_trigger()
	{
		trigger_on = true;
	}
	
	EXPORT_DECORATION void stop_trigger()
	{
		trigger_on = false;
	}
	
	EXPORT_DECORATION int get_sensor(int i)
	{
		if(n_sensor > 0)
			return sensors[i];
		else
			return -1;
		
	}
	
	EXPORT_DECORATION double get_updateRate()
	{
		return device_update_rate;
	}

	//set is_on to false so main function ends
	EXPORT_DECORATION void stop_array()
	{
		is_on = false;
	}

	//main function, to initiate the array. This function should be run in a detached thread
	EXPORT_DECORATION int start_array()
	{
		// Create an Ultrahaptics Library object and connect it to a running service
		ResearchLibrary lib;
		auto found_library = lib.connect();
		if (!found_library) {
			std::cout << "Library failed to connect: " << found_library.error().message() << std::endl;
			return 1;
		}

		// Create a streaming emitter and add a suitable device to it
		CSEmitter emitter {lib, StreamingUpdateMode::DeviceLinked};
		auto device = lib.findDevice();
		if (!device) {
			std::cout << "Failed to findDevice: " << device.error().message() << std::endl;
			return 1;
		}

		std::cout << "Starting array...";
		
		DriverDevice* driver_device = NULL; 
		{
			auto modifiable_device_result = device.value().getModifiableDevice();
			auto modifiable_device = std::move(modifiable_device_result.value());
			auto driver_device_shared = modifiable_device.getDriverDevice().value();
			driver_device = driver_device_shared.get();

			//std::cout<< cap_ave << " "<< cap_peak << " "<< cap_range;

			driver_device->setCappingAverage(cap_ave, 0);
            driver_device->setCappingPeak(cap_peak, 0);
            driver_device->setCappingRange(cap_range * cap_range, 0);
			
			float capping_average = driver_device->getCappingAverage(0).value_or(-1);
			float capping_peak = driver_device->getCappingPeak(0).value_or(-1);
			float capping_range = driver_device->getCappingRange(0).value_or(-1);

			std::cout << std::fixed << "Capping Average\tCP" << 0 << ": " << capping_average << "Pa\n";
			std::cout << std::fixed << "Capping Peak\tCP" << 0 << ": " << capping_peak << "Pa\n";
			std::cout << std::fixed << "Capping Range\tCP" << 0 << ": " << capping_range << "Pa^2\n";
			
			driver_device->setStreamingUpdateRate(update_rate);
			device_update_rate = driver_device->getStreamingUpdateRate().value_or(-1);
			std::cout << std::fixed << "Update rate:" << device_update_rate <<"Hz\n"; 
			
			n_sensor = driver_device->getSensorCount().value();
			sensors = (std::atomic<int> *) malloc(n_sensor*sizeof(std::atomic<int>));
			
		}
		
		auto add_res = emitter.addDevice(device.value());
		if (!add_res) {
			std::cout << "Failed to addDevice: " << add_res.error().message() << std::endl;
			return 1;
		}

		// Set the maximum control point count and update rate
		auto cduc_res = emitter.setCommonDeviceUpdateConfig(1, 40000.f);
		if (!cduc_res) {
			std::cout << "Failed to setCommonDeviceUpdateConfig: " << cduc_res.error().message() << std::endl;
			return 1;
		}

		auto cr_res = emitter.setCallbackRate(1000.f);
		if (!cr_res) {
			std::cout << "Failed to setCallbackRate: " << cr_res.error().message() << std::endl;
			return 1;
		}
		
		//update buffer size to get less drop out on windows
		LocalDuration target_latency = duration_from_sec(0.005); // 5ms
		auto ctbd_res = emitter.setTargetBufferDuration(target_latency);
		if (!ctbd_res) {
			std::cout << "Failed to setTargetBufferDuration: " << ctbd_res.error().message() << std::endl;
			return 1;
		}
	
		auto ecm_res = emitter.setEmissionCallbackMode(StreamingCallbackMode::Normal);
		if (!ecm_res) {
			std::cout << "Failed to setEmissionCallbackMode: " << ecm_res.error().message() << std::endl;
			return 1;
		}
		
		// Create an object to inform the callback of user input once running
		UserData data(1);

		// Set the callback function to the callback written above
		auto ec_res = emitter.setEmissionCallback(my_emitter_callback, &data);
		if (!ec_res) {
			std::cout << "Failed to setEmissionCallback: " << ec_res.error().message() << std::endl;
			return 5;
		}
	
		// Start the array
		emitter.startCallback();

		is_on = true;
		emit_on = false;
		current_sample = 0;
		samples_size = 40000;

		//Main loop
		using namespace std::chrono_literals;
		while(is_on)
		{
			std::this_thread::sleep_for(2ms);//small break, not necessary I guess
			
			for(size_t i = 0; i < n_sensor; i++)
			{
				auto res = driver_device->getSensorValue(i+1); //sensors are indexed from 1 to n, not from 0 to n-1
				if(res)
				{
					sensors[i] = res.value().value();
				}		
			}
		}

		// Stop the array
    	emitter.stopCallback();

		// Emitter shuts down on exit.
		return 0;
	}
}