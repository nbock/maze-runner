          
#include <iostream>
#include <math.h>

#include "robot.hh"

using std::cout;
using std::endl;

// we start on the 0th file
int file = 0;
int lastFile = 0;
bool wallFound = false;
bool facingDoor = false;
bool transition = false;
bool backWall = false;
bool turningRight = false;


// we know the goal state is always on the 11th file
int goalFile = 10;

void
callback(Robot* robot)
{
    bool backFacing = (robot->pos_t < -1.6 || robot->pos_t > 1.6);
    cout << "Range: " << robot->range << endl;
    cout << "Heading: " << robot->pos_t << endl;
    cout << "File: " << file << endl;
    cout << "Lastfile:" << lastFile << endl;
    cout << "Wallfound: " << wallFound << endl;
    cout << "Backwall: " << backWall << endl;
    cout << "Turningright: " << turningRight << endl;
    cout << "Backfacing: " << backFacing << endl;

    if (robot->range < 998) {
        wallFound = true;
    }

    
    // if facing backwards, orient another direction
    if (wallFound && backFacing) {
        backWall = true;
    }

    if (robot->range < 1.2 || (file == 10 && robot->range < 1.5)) {
        float heading = robot->pos_t;

        if (heading > 0 && heading < 1.5) {
            // facing left
            backWall = false;
        } else if (heading < 0 && heading > -1.5) {
            // facing right
            backWall = false;
        }

        robot->set_vel(-5.0, 5.0);

        wallFound = true;
        transition = false;

        return;
 	}

    if (robot->range < 1.7) {
        // now we know we can turn through a door
        
        transition = false;
        wallFound = true;

        float heading = robot->pos_t;
        
        if (heading > 0 && heading < 0.75) {
            // facing left
            backWall = false;
        } else if (heading < 0 && heading > -0.75) {
            // facing right
            backWall = false;
        }

        robot->set_vel(5.0, 5.0);

        return;
    }

 
    // range is 999 when we have no sonar hits, but it seems to be rounded in output
    if (robot->range > 998) {

        wallFound = false;
        if (!backWall || file == 11 || (lastFile == 11 && file == 10)) {
            // keep track when we're along a wall (could be left or right)
        
            // decide which way to turn
            // send commands based on that
            // turningRight == false --> turning left
            // turningRight == true --> turning right

            if (turningRight) {
                if (robot->pos_t > -0.75 && robot->pos_t < 0.75) { 
                    robot->set_vel(5.0, -2.0);
                } else { 
                    robot->set_vel(5.0, 0.5);
                }
            } 
            
            if (!turningRight) {
                // turning left
                if (robot->pos_t < -0.75 && robot->pos_t > 0.75) {
                    robot->set_vel(-2.0, 5.0);
                } else {
                    robot->set_vel(0.5, 5.0);
                }
            } 
            
           
            if (!transition) {
                // on the 11th file, I want to be able to turn "backwards"
                if (robot->pos_t > 0) {
                    turningRight = true;
                    
                    if (file == 11 || lastFile == 11) {
                        turningRight = false;
                    }
                } else {
                    turningRight = false;

                    if (file == 11 || lastFile == 11) {
                        turningRight = true;
                    }
                }

                cout << "Moving to new file" << endl;
                lastFile = file; 
                if (file == 11) { 
                    file -= 1;
                } else {
                    file += 1;
                }
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
