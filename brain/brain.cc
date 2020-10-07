          
#include <iostream>
#include <math.h>

#include "robot.hh"

using std::cout;
using std::endl;

// we start on the 0th file
int file = 0;
bool wallFound = false;
bool facingDoor = false;
bool transition = false;
bool backWall = false;

// we know the goal state is always on the 11th file
int goalFile = 10;

void
callback(Robot* robot)
{
    
    // Celebrate: backwall and file tracker are WORKING
    // TODO: filing and not turning "back" works, pending a full test (tested from corner in the middle of the maze
    // TODO: goal file has no logic, gonna leave that for last as that should be relatively simple.

    cout << "Range: " << robot->range << endl;
    cout << "Heading: " << robot->pos_t << endl;
    cout << "File: " << file << endl;
    cout << "Wallfound: " << wallFound << endl;
    cout << "Backwall: " << backWall << endl;

    if (robot->range < 998) {
        wallFound = true;
    }

    
    // if facing backwards, orient another direction
    if (wallFound && (robot->pos_t < -1.7 || robot->pos_t > 1.7)) {
        backWall = true;
    }

    if (robot->range < 1.0) {
        int heading = robot->pos_t;

        if (heading > 0 && heading < 0.75) {
            // facing left
            robot->set_vel(-5.0, 5.0);
        } else if (heading < 0 && heading > -0.75) {
            // facing right
            robot->set_vel(5.0, -5.0);
        } else {
            robot->set_vel(-5.0, 5.0);
        }

        if (!(robot->pos_t < -1.7 || robot->pos_t > 1.7)) {
            backWall = false;
        }

        wallFound = true;
        transition = false;
        return;
 	}

    if (robot->range < 1.7 && !wallFound) {
        // now we know we can turn through a door

        if (robot->pos_t > 0) {
            robot->set_vel(5.0, -2.0);
        } else if (robot->pos_t < 0) {
            robot->set_vel(-2.0, 5.0);
        }

        transition = false;

        return;
    }

 
    // range is 999 when we have no sonar hits, but it seems to be rounded in output
    if (robot->range > 998 && !transition) {

        wallFound = false;
        if (!backWall) {
            // keep track when we're along a wall (could be left or right)
       
            if (robot->pos_t > 0) { 
                robot->set_vel(5.0, 0.0);
            } else if (robot->pos_t <= 0) {
                robot->set_vel(0.0, 5.0);
            }
            if (!transition) {
                cout << "Moving to new file" << endl;
                file += 1;
                transition = true;
            
            }
            return;
        }
    }

 
    
    if (!transition) {
        robot->set_vel(5.0, 5.0);
    }

}


int
main(int argc, char* argv[])
{
    cout << "making robot" << endl;
    Robot robot(argc, argv, callback);
    robot.do_stuff();

    return 0;
}
