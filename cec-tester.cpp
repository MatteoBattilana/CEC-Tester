#include <libcec/cec.h>
#include <iostream>
#include <libcec/cecloader.h>
using std::cout;
using std::cin;
using std::endl;
#include <algorithm>
#include <array>
#include <chrono>
#include <thread>

int key_pressed;
struct key
{
	int key_val;
	std::string name;
};

key key_test[] = {
	{ CEC::CEC_USER_CONTROL_CODE_UP, "UP" },
	{ CEC::CEC_USER_CONTROL_CODE_DOWN, "DOWN" },
	{ CEC::CEC_USER_CONTROL_CODE_LEFT, "LEFT" },
	{ CEC::CEC_USER_CONTROL_CODE_RIGHT, "RIGHT" }
};

void on_keypress(void* not_used, const CEC::cec_keypress* msg)
{
	key_pressed = msg->keycode;
}

void wait_key_pressed(key key)
{
	std::cout << "\tKey [" << key.name << "] pressed\t\t" << std::flush;
	while (key_pressed != key.key_val)
	{
		std::this_thread::sleep_for( std::chrono::seconds(1) );
	}
	std::cout << "\033[1;32m\u2713\033[0m" << std::endl;
}

int main(int argc, char* argv[])
{
	std::cout << "\n" << std::endl;
	std::cout << "################################################################################" << std::endl;
	std::cout << "######                            CEC-Tester                              ######" << std::endl;
	std::cout << "######                       by Matteo Battilana                          ######" << std::endl;
	std::cout << "################################################################################" << std::endl << std::endl;
	std::cout << "Please connect to the CEC-Tester device on your television and press the shown "<<std::endl;
	std::cout << "key during the test.\n" << std::endl;

	CEC::ICECCallbacks        cec_callbacks;
	CEC::libcec_configuration cec_config;
	cec_config.Clear();
	cec_callbacks.Clear();

	const std::string devicename("CEC-Tester");
	devicename.copy(cec_config.strDeviceName, std::min((int)devicename.size(), 13));

	// Setup
	cec_config.clientVersion       = CEC::LIBCEC_VERSION_CURRENT;
	cec_config.bActivateSource	   = 0;
	cec_config.callbacks           = &cec_callbacks;
	cec_config.deviceTypes.Add(CEC::CEC_DEVICE_TYPE_RECORDING_DEVICE);
	cec_callbacks.keyPress    		 = &on_keypress;

	CEC::ICECAdapter* cec_adapter = LibCecInitialise(&cec_config);
	if( !cec_adapter )
	{
		std::cerr << "\033[1;31mFailed loading libcec.so.\nExiting.\033[0m" << std::endl;
		return 1;
	}

	std::cout << "Test list:" << std::endl;

	// Try to automatically determine the CEC devices
	std::array<CEC::cec_adapter_descriptor,10> devices;
	int8_t devices_found = cec_adapter->DetectAdapters(devices.data(), devices.size(), NULL);
	if( devices_found <= 0)
	{
		std::cout << "\tCEC supported\t\t\t\033[1;31m\u2715\033[0m" <<  std::endl;
		std::cerr << "\t\033[1;31mCould not automatically determine the cec adapter devices." << std::endl;
		std::cerr << std::endl << "Exiting.\033[0m" << std::endl;
		UnloadLibCec(cec_adapter);
		return 0;
	}

	// Open a connection to the zeroth CEC device
	if( !cec_adapter->Open(devices[0].strComName) )
	{
		std::cout << "\tLocal CEC supported\t\t\033[1;31m\u2715\033[0m" <<  std::endl;
		std::cerr << "\t\033[1;31mFailed to open the CEC device on port " << devices[0].strComName  << std::endl;
		std::cerr << std::endl << "Exiting.\033[0m" << std::endl;
		UnloadLibCec(cec_adapter);
		return 0;
	}
	std::cout << "\tLocal CEC supported\t\t\033[1;32m\u2713\033[0m" <<  std::endl;

	// Ping the device
	if(!cec_adapter->PowerOnDevices())
	{
			std::cout << "\tRemote CEC supported\t\t\033[1;31m\u2715\033[0m" <<  std::endl;
			UnloadLibCec(cec_adapter);
			return 0;
	}
			std::cout << "\tRemote CEC supported\t\t\033[1;32m\u2713\033[0m" <<  std::endl;

	for(const key &key : key_test)
	{
		wait_key_pressed(key);
	}

	std::cout << "\033[1;32m\nAll tests done!\n\033[0m" <<  std::endl;

	// Close CEC
	cec_adapter->Close();
	UnloadLibCec(cec_adapter);

	return 0;
}
