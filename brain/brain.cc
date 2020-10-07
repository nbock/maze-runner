          
#include <iostream>
#include <math.h>

#include "robot.hh"

using std::cout;
using std::endl;

// we start on the 0th file
int file = 0;
bool wallFound = false;
bool facingDoor = false;

// we know the goal state is always on the 11th file
int goalFile = 10;

void
callback(Robot* robot)
{
    
    // TODO: fix file system, not working at all
    // TODO: fix going through passages that are facing the wrong way, never an incentive for us unless on 12th file
    
    cout << "Range: " << robot->range << endl;
    cout << "Heading: " << robot->pos_t << endl;
    cout << "File: " << file << endl;

    if (robot->pos_t < 0.02 && robot->range > 998 && !wallFound) {
        robot->set_vel(5.0, 5.0);
        return;
    }
    
    if (robot->range < 1.0) {
       int heading = robot->pos_t;

        if (heading < 0) {
            robot->set_vel(5.0, -5.0);
        } else if (heading >= 0) {
            robot->set_vel(-5.0, 5.0);
        }
        wallFound = true;
        return;
 	}

    if (robot->range < 1.7 && !wallFound) {
        // now we know we can turn through a door
        
        if (robot->pos_t > 0) {
            robot->set_vel(5.0, -2.0);
        } else if (robot->pos_t < 0) {
            robot->set_vel(-2.0, 5.0);
        }

        return;
    }

 
    // range is 999 when we have no sonar hits, but it seems to be rounded in output
    if (robot->range > 998) {

         
        if (robot->pos_t < 0.1) {
            cout << "Moving to new file" << endl;
            file += 1;
        }

        wallFound = false;
        // keep track when we're along a wall (could be left or right)
        robot->set_vel(5.0, 5.0);
        return;
    }



    // move forwaard
    robot->set_vel(5.0, 5.0);
}

int
main(int argc, char* argv[])
{
    cout << "making robot" << endl;
    Robot robot(argc, argv, callback);
    robot.do_stuff();

    return 0;
}
