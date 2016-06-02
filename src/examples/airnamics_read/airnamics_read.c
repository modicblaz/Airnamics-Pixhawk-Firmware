/**********************************************************************************************************************

  Airnamics Pixhawk read test module

  Description:
  - simple testing program that subscribes to airnamics data that was published by airnamics_publish application and prints it to terminal
  
  Usage notes:
  - run in terminal or at boot time

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
#include <uORB/topics/airnamics_data.h>

/************* DEFINITIONS ********************************************************************************************/

/************* DECLARATIONS *******************************************************************************************/

/************* FUNCTION PROTOTYPES ************************************************************************************/

__EXPORT int airnamics_read_main(int argc, char *argv[]);

/************* FUNCTIONS **********************************************************************************************/

/**********************************************************************************************************************
Function name : airnamics_read_main
Description   : Test function
***********************************************************************************************************************/
int airnamics_read_main(int argc, char *argv[])
{
	// local variables
	int                      data_sub_fd;
	int                      poll_ret;
	struct airnamics_data_s  raw;
	
	// print to terminal
	PX4_INFO("Running airnamics_read");

	// subscribe to sensor_combined topic and initialize polling data
	data_sub_fd = orb_subscribe(ORB_ID(airnamics_data));
	px4_pollfd_struct_t      fds[] = {{.fd = data_sub_fd, .events = POLLIN}, };
	

	while(true)
	{
    // wait for sensor update of 1 file descriptor for 10000 ms (10 seconds)
    poll_ret = px4_poll(fds, 1, 10000);

    if(poll_ret == 0)
    {
      // this means none of our providers is giving us data
      PX4_ERR("[airnamics_read] Got no data within 10 seconds");
    }
    else if(poll_ret < 0)
    {
    // this is seriously bad - should be an emergency
      PX4_ERR("[airnamics_read] ERROR return value from poll(): %d", poll_ret);
    }
    else
    {
	    if(fds[0].revents & POLLIN)
	    {      
	      // copy sensors raw data into local buffer
	      orb_copy(ORB_ID(airnamics_data), data_sub_fd, &raw);

	      // print to terminal
	      printf("[airnamics_read] DATA:\t%d\t%d \n", raw.val1, raw.val2);
		  }
		}
	}

  PX4_INFO("airnamics_read finished");
  return OK;
}