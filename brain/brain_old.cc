          
#include <iostream>
#include <math.h>

#include "robot.hh"

using std::cout;
using std::endl;

// we start on the 0th file
int file = 9;
int lastFile = 10;
float headingDesired = 0.0;
float lastHeading = 0.0;
bool wallFound = false;
bool facingDoor = false;
bool transition = false;
bool backWall = false;
bool turningRight = false;
bool turningRightOut = false;

bool leftWallFound = false;
bool rightWallFound = false;
bool frontWallFound = false;
bool fullyExplored = false;
bool files[11][3] = {{false, false, false}, {false, false, false}, {false, false, false}, 
                     {false, false, false}, {false, false, false}, {false, false, false}, 
                     {false, false, false}, {false, false, false}, {false, false, false}, 
                     {false, false, false}, {false, false, false}};

// we know the goal state is always on the 11th file
int goalFile = 10;

void
callback(Robot* robot)
{
    bool backFacing = (robot->pos_t < -1.6 || robot->pos_t > 1.6);
    bool facingForward = (robot->pos_t > -0.75 && robot->pos_t < 0.75);
    bool facingBackward = ((robot->pos_t < 3.2 && robot->pos_t > 2.4) || (robot->pos_t > -3.2 && robot->pos_t < -2.4));
   
    if (!transition) { 
        lastHeading = robot->pos_t;
    }

    cout << "Range: " << robot->range << endl;
    cout << "Heading: " << robot->pos_t << endl;
    cout << "Desired heading: " << headingDesired << endl;
    cout << "Last heading: " << lastHeading << endl;
    cout << "File: " << file << endl;
    cout << "Lastfile: " << lastFile << endl;
    cout << "Wallfound: " << wallFound << endl;
    cout << "Backwall: " << backWall << endl;
    cout << "Turningright: " << turningRight << endl;
    cout << "Turningright out: " << turningRightOut << endl;
    cout << "Backfacing: " << backFacing << endl;
    cout << "Facing backward: " << facingBackward << endl;
    cout << "Fully explored: " << fullyExplored << endl;
    cout << "Transition: " << transition << endl;


    files[file][0] = leftWallFound;
    files[file][1] = rightWallFound;
    files[file][2] = frontWallFound;
    fullyExplored = (files[file][2] && files[file][1] && files[file][0]);
    
    cout << "File " << file << " leftwall found: " << files[file][0] << endl;
    cout << "File " << file << " rightwall found: " << files[file][1] << endl;
    cout << "File " << file << " frontwall found: " << files[file][2] << endl;

    if (robot->range < 998) {
        wallFound = true;
    }

           
    
    // if facing backwards, orient another direction
    if (wallFound && backFacing) {
        backWall = true;
    }

    if (robot->range < 1.2 || (file == 10 && robot->range < 1.5)) {
        float heading = robot->pos_t;

        if (robot->range < 1.15) {
            if (wallFound && heading < -1.5 && heading > -1.7) {
                rightWallFound = true;
            } else if (wallFound && heading > 1.5 && heading < 1.7) {
                leftWallFound = true;
            } else if (wallFound && heading < 0.3 && heading > -0.3) {
                frontWallFound = true;
            }
        }
        
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

        if (!backWall || file == 11 || (lastFile == 11 && file == 10) || fullyExplored) {
            // turningRight == false --> turning left
            // turningRight == true --> turning right
            
            if (turningRight) {
                if (facingForward || (lastFile == 11 && facingBackward)) { 
                    robot->set_vel(5.0, -2.0);
                } else { 
                    robot->set_vel(5.0, 0.5);
                }
            } 
            
            if (!turningRight) {
                // turning left
                if (facingForward || (lastFile == 11 && facingBackward)) {
                    robot->set_vel(-2.0, 5.0);
                } else {
                    robot->set_vel(0.5, 5.0);
                }
            } 

            if (fabs(lastHeading) < 0.3 && lastHeading != 0.0) {
                robot->set_vel(3.0, 5.0);
            }
            
           
            if (!transition) {
                
                // on the 11th file, I want to be able to turn "backwards"
                if (robot->pos_t > 0) {
                    turningRight = true;
                    turningRightOut = true;
                    
                    if (file == 11 || lastFile == 11) {
                        turningRight = false;
                        turningRightOut = false;
                    }
                } else {
                    turningRight = false;
                    turningRightOut = false;

                    if (file == 11 || lastFile == 11) {
                        turningRight = true;
                        turningRightOut = true;
                    }
                }

                // if we're leaving a file, we want to turn out the way we turned in (so we don't double travel a file)
                if (files[file][2] && files[file][0] && files[file][1] && (robot->pos_t > 2.7 || robot->pos_t < -2.7)) {
                    turningRight = turningRightOut;
                }

                cout << "Moving to new file" << endl;
                lastFile = file; 
                if (backWall) { 
                    file -= 1;
                } else {
                    file += 1;
                }
                
                leftWallFound = false;
                rightWallFound = false;
                frontWallFound = false;

                if (facingBackward && !fullyExplored && (fabs(robot->pos_t - headingDesired)) < 0.1) {
                    transition = false;
                } else {
                    transition = true;
                }
            
            }

            if (facingBackward && !fullyExplored && !(fabs(robot->pos_t - headingDesired) < 0.1) && file != 11 && lastFile != 11) {
                if (!files[file][0]) {
                    // haven't found the left wall
                    headingDesired = 1.57;
                    robot->set_vel(-5.0, 5.0);
                } else if (!files[file][1]) {
                    // haven't found the right wall
                    headingDesired = -1.57;
                    robot->set_vel(5.0, -5.0);
                } else {
                    // haven't found the front wall (kinda an error state)
                    headingDesired = 0.00;
                    robot->set_vel(5.0, -5.0);
                }
            }

            if (facingBackward && !fullyExplored && (fabs(robot->pos_t - headingDesired) < 0.1)) {
                robot->set_vel(5.0, 5.0);
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
