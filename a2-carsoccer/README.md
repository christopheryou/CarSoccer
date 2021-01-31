# My solution to Assignment 2
 
Christopher You
Project Designs:

Functions Added:

Dot Product - Returns dot product of two vectors

Reflection - Returns the bounce of an object off another

Distance - Distance between two points

collisionWillHappen - Checks if ball will hit anything

collisionWillHappenC - Checks if car will hit wall

thresholding - Checks if any points in a vector3 exceed wall, and thresholds them

thresholdingC - Same thing but for car.

Car movement:

Get rotation (check left/right arrow keys) and combine it with the car.
This is the rotation in the Y and the speed added is the (thrust - drag) * timeStep

Anytime a collision happens with the car and wall, we just simply make sure that the car is not going past the wall.

Ball Movement:
//I'm only going to comment this first part because the x,y, and z are all the same.
//I check if the distance between the ball and radius are == 0 (this is for the ground level)
//And I also check if the ball is heading towards this direction by checking what its position would be if i moved it.
//If they are exactly touching, I make it bounce

//If they are intersecting, then I make the ball position equal to touching the floor.
//This way, next frame, the difference will be 0, and they will bounce.

//I repeat this process for the x and z but use different numbers.  This is all done through using normal vectors and what not.

I do this for all of the walls,  but in the Z section, I check for goals as well.
At the end of each frame I make sure gravity affects the ball.

//Reset
Makes sure the ball flies towards the user's side of the field.
