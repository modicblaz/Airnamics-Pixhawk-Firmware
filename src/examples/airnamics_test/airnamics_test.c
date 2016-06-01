/**********************************************************************************************************************

  Airnamics Pixhawk test module

  Description:
  - simple testing program
  
  Usage notes:
  - run in terminal

***********************************************************************************************************************/

/************* INCLUDES ***********************************************************************************************/

// system includes
#include <px4_config.h>
#include <px4_tasks.h>
#include <px4_posix.h>
#include <unistd.h>
#include <stdio.h>
#include <poll.h>
#include <string.h>

// uORB includes
#include <uORB/uORB.h>
#include <uORB/topics/sensor_combined.h>
#include <uORB/topics/vehicle_attitude.h>

/************* DEFINITIONS ********************************************************************************************/

/************* DECLARATIONS *******************************************************************************************/

/************* FUNCTION PROTOTYPES ************************************************************************************/

__EXPORT int airnamics_test_main(int argc, char *argv[]);

/************* FUNCTIONS **********************************************************************************************/

/**********************************************************************************************************************
Function name : airnamics_test_main
Description   : Test function
***********************************************************************************************************************/
int airnamics_test_main(int argc, char *argv[])
{
	// local variables
	int                      sensor_sub_fd;
	int                      poll_ret;
	struct sensor_combined_s raw;
	int                      i;

	// print to terminal
	PX4_INFO("Running airnamics_test");

	// subscribe to sensor_combined topic and initialize polling data
	sensor_sub_fd = orb_subscribe(ORB_ID(sensor_combined));
	px4_pollfd_struct_t      fds[] = {{.fd = sensor_sub_fd, .events = POLLIN}, };
	
	for(i = 0; i < 10; i++)
	{
    // wait for sensor update of 1 file descriptor for 1000 ms (1 second)
    poll_ret = px4_poll(fds, 1, 1000);

    if(poll_ret == 0)
    {
      // this means none of our providers is giving us data
      PX4_ERR("[airnamics_test] Got no data within a second");
    }
    else if(poll_ret < 0)
    {
    // this is seriously bad - should be an emergency
      PX4_ERR("[airnamics_test] ERROR return value from poll(): %d", poll_ret);
    }
    else
    {
	    if(fds[0].revents & POLLIN)
	    {      
	      // copy sensors raw data into local buffer
	      orb_copy(ORB_ID(sensor_combined), sensor_sub_fd, &raw);

	      // print to terminal
	      printf("[airnamics_test] Accelerometer:\t%lu\t%8.4f\t%8.4f\t%8.4f\n", raw.timestamp,
                                                                             (double)raw.accelerometer_m_s2[0],
	                  																										     (double)raw.accelerometer_m_s2[1],
	                  																										     (double)raw.accelerometer_m_s2[2]);
		  }
		}
	}

  PX4_INFO("airnamics_test finished");
  return OK;
}