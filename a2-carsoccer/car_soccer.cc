/** CSci-4611 Assignment 2:  Car Soccer
 */

#include "car_soccer.h"
#include "config.h"
#include <stdio.h>
#include <iostream>

CarSoccer::CarSoccer() : GraphicsApp(1024, 768, "Car Soccer") {
	// Define a search path for finding data files (images and shaders)
	searchPath_.push_back(".");
	searchPath_.push_back("./data");
	searchPath_.push_back(DATA_DIR_INSTALL);
	searchPath_.push_back(DATA_DIR_BUILD);
}

CarSoccer::~CarSoccer() {
}


Vector2 CarSoccer::joystick_direction() {
	Vector2 dir;
	if (IsKeyDown(GLFW_KEY_LEFT))
		dir[0]--;
	if (IsKeyDown(GLFW_KEY_RIGHT))
		dir[0]++;
	if (IsKeyDown(GLFW_KEY_UP))
		dir[1]++;
	if (IsKeyDown(GLFW_KEY_DOWN))
		dir[1]--;
	return dir;
}

void CarSoccer::OnSpecialKeyDown(int key, int scancode, int modifiers) {
	if (key == GLFW_KEY_SPACE) {
		car_.Reset();
		ball_.Reset();
	}
}

float CarSoccer::DotProduct(Vector3 vec1, Vector3 vec2) {
	float x = vec1.x() * vec2.x();
	float y = vec1.y() * vec2.y();
	float z = vec1.z() * vec2.z();
	float sum = x + y + z;
	return sum;
}

//Reflect equation
Vector3 CarSoccer::Reflection(Vector3 direction, Vector3 normal_force) {
	float result = DotProduct(direction, normal_force);
	Vector3 newVel = direction - (2 * (result)* normal_force);
	return newVel;
}

//Calculate new position with physics equation
Point3 CarSoccer::newPosition(Point3 position, Vector3 velocity, double deltaTime) {
	Point3 newPos = position + (velocity * deltaTime);
	return newPos;
}

//Distance equation.
float CarSoccer::distance(Point3 position1, Point3 position2) {
	float x = position1.x() - position2.x();
	x *= x;
	float y = position1.y() - position2.y();
	y *= y;
	float z = position1.z() - position2.z();
	z *= z;
	return sqrt(x + y + z);
}


//Checks if a collision will happen between the ball and anything
bool CarSoccer::collisionWillHappen(Point3 position, Point3 newPosition) {
	if ((position.x() - ball_.radius() <= -40 && newPosition.x() - ball_.radius() <= -40) 
		|| (position.x() + ball_.radius() >= 40 && newPosition.x() + ball_.radius() >= 40) 
		|| (position.y() - ball_.radius() <= 0 && newPosition.y() - ball_.radius() <= 0) 
		|| (position.y() + ball_.radius() >= 35 && newPosition.y() + ball_.radius()  >= 35) 
		|| (position.z() - ball_.radius() <= -50 && newPosition.z() - ball_.radius() <= -50) 
		|| (position.z() + ball_.radius() >= 50 && newPosition.z() + ball_.radius() >= 50)) {
		return true;
	}
	float d = distance(car_.position(), ball_.position());
	if (car_.collision_radius() + ball_.radius() >= d) {
		return true;
	}
	return false;
}

//
bool CarSoccer::collisionWillHappenC(Point3 position, Point3 newPosition) {
	if ((position.x() - car_.collision_radius() <= -40 && newPosition.x() - car_.collision_radius() <= -40)
		|| (position.x() + car_.collision_radius() >= 40 && newPosition.x() + car_.collision_radius() >= 40)
		|| (position.z() - car_.collision_radius() <= -50 && newPosition.z() - car_.collision_radius() <= -50)
		|| (position.z() + car_.collision_radius() >= 50 && newPosition.z() + car_.collision_radius() >= 50)) {
		return true;
	}
	return false;
}

Point3 CarSoccer::thresholding(Point3 vec) {
	float newX, newY, newZ;
	newX = vec.x();
	newY = vec.y();
	newZ = vec.z();
	if (vec.x() + ball_.radius() > 40) {
		newX = 40 - ball_.radius();
	}
	else if (vec.x() - ball_.radius() < -40) {
		newX = -40 + ball_.radius();
	}

	if (vec.y() - ball_.radius() < 0) {
		newY = 0 + ball_.radius();
	}
	else if (vec.y() + ball_.radius() > 35) {
		newY = 35 - ball_.radius();
	}

	if (vec.z() +ball_.radius()> 50) {
		newZ = 50 - ball_.radius();
	}
	else if (vec.z()-  ball_.radius() < -50) {
		newZ = -50 + ball_.radius();
	}
	return Point3(newX, newY, newZ);
}
Point3 CarSoccer::thresholdingC(Point3 vec) {
	float newX, newY, newZ;
	newX = vec.x();
	newY = vec.y();
	newZ = vec.z();
	if (vec.x() + car_.collision_radius() > 40.0f) {
		newX = 40.0f - car_.collision_radius();
	}
	else if (vec.x() - car_.collision_radius() < -40.0f) {
		newX = -40.0f + car_.collision_radius();
	}

	if (vec.y() - car_.collision_radius() < 0.0f) {
		newY = 0.0f + car_.collision_radius();
	}
	else if (vec.y() + car_.collision_radius() > 35.0f) {
		newY = 35.0f - car_.collision_radius();
	}

	if (vec.z() + car_.collision_radius() > 50.0f) {
		newZ = 50.0f - car_.collision_radius();
	}
	else if (vec.z() - car_.collision_radius() < -50.0f) {
		newZ = -50.0f + car_.collision_radius();
	}
	return Point3(newX, newY, newZ);
}


void CarSoccer::UpdateSimulation(double timeStep) {
	Vector2 dir = joystick_direction();

	//Increase/Decrease angle while Right/Left Arrow are held.
	float angle = car_.angle();
	if (dir[0] > 0) {
		angle -= 0.25f;
		car_.set_angle(angle);
	}
	else if (dir[0] < 0) {
		angle += 0.25f;
		car_.set_angle(angle);
	}

	//Forward Movement
	if (dir[1] > 0) {
		//Get the amount of rotation in the Y
		Matrix4 Rotation =
			Matrix4::RotationY(car_.angle());
		//Get the thrust and subtract the drag from it.
		//Set the car's speed accordingly

		Vector3 thrust = Vector3(0, 0, -1) * 35;
		Vector3 drag = car_.speed();
		Vector3 speedInc = (thrust - drag) * timeStep;
		car_.set_speed(car_.speed() + speedInc);

		//Move the Rotation of the car accordingly.
		Vector3 mov = Rotation * car_.speed();
		//Get what the new position would be given the velocity
		//Get what the position would be if it would go out of bounds using fixedC.
		Point3 newPosC = newPosition(car_.position(), mov, timeStep);
		Point3 fixedC = thresholdingC(newPosC);

		//Check if the car will collide with any of the walls (not ceiling).
		if (collisionWillHappenC(car_.position(), newPosC)) {
			//this makes sure that it doesnt go past the outside edges.
			
			//Check if the newPos's coordinate is going to go out of bounds
			//If it does, set it to inbounds.
			if (newPosC.x() - car_.collision_radius() <= -40.0f) {
				car_.set_position(Point3(-40.0f + car_.collision_radius(), 0, fixedC.z()));
			}
			else if (newPosC.x() + car_.collision_radius() >= 40.0f) {
				car_.set_position(Point3(40.0f - car_.collision_radius(), 0, fixedC.z()));
			}

			if (newPosC.z() - car_.collision_radius() <= -50) {
				car_.set_position(Point3(fixedC.x(), 0, -50 + car_.collision_radius()));
			}
			else if (newPosC.z() + car_.collision_radius() >= 50) {
				car_.set_position(Point3(fixedC.x(), 0, 50 - car_.collision_radius()));
			}

		}
		else {
			//If no collision will happen, just move the car.
			car_.set_position(newPosC);
		}

	}
	else if (dir[1] < 0) {
		Matrix4 Rotation =
			Matrix4::RotationY(car_.angle());
		Vector3 thrust = Vector3(0, 0, 1) * 35;
		Vector3 drag = car_.speed();
		Vector3 speedInc = (thrust - drag) * timeStep;
		car_.set_speed(car_.speed() + speedInc);
		Vector3 mov = Rotation * car_.speed();
		Point3 newPosC = newPosition(car_.position(), mov, timeStep);
		Point3 fixedC = thresholdingC(newPosC);
		if (collisionWillHappenC(car_.position(), newPosC)) {
			if (newPosC.x() - car_.collision_radius() <= -40.0f) {
				car_.set_position(Point3(-40.0f + car_.collision_radius(), 0, fixedC.z()));
			}
			else if (newPosC.x() + car_.collision_radius() >= 40.0f) {
				car_.set_position(Point3(40.0f - car_.collision_radius(), 0, fixedC.z()));
			}

			if (newPosC.z() - car_.collision_radius() <= -50.0f) {
				car_.set_position(Point3(fixedC.x(), 0, -50.0f + car_.collision_radius()));
			}
			else if (newPosC.z() + car_.collision_radius() >= 50) {
				car_.set_position(Point3(fixedC.x(), 0, 50.0f - car_.collision_radius()));
			}

		}
		else {
		car_.set_position(newPosC);

		}
	}


	else {
		//Slowly decrease speed if arrow keys aren't up.
	float z = car_.speed().z();
	if (z > 0) {
		z--;
	}
	else if (z < 0) {
		z++;
	}

	Vector3 speed = Vector3(car_.speed().x(), 0, z);
	car_.set_speed(speed);
	}


	//Check if ball is on the ground hit ground
	//If it does then make it bounce using the reflection function
	Point3 newPos = newPosition(ball_.position(), ball_.velocity(), timeStep);
	Point3 fixed = thresholding(newPos);

	//Check if a collision will happen
	if (collisionWillHappen(ball_.position(), newPos)) {
		/// Car
		//Get the distance between car and ball
		float curDistance = distance(car_.position(), ball_.position());
		//float newDistance = distance(car_.position(), newPos);

		/*if (curDistance == car_.collision_radius() + ball_.radius()) {
			//ball - car or car - ball?

			Vector3 cartoball = ball_.position() - car_.position();
			Vector3 carNormal = Vector3::Normalize(cartoball);
			Vector3 relVel = ball_.velocity() - car_.speed();
			Vector3 newVel = Reflection(relVel, carNormal);

			ball_.set_velocity(newVel + car_.speed());
			ball_.set_position(newPosition(ball_.position(), ball_.velocity(), timeStep));
			ball_.set_velocity(ball_.velocity() * 0.8f);
		}*/
		//Check if they are colliding
		if (curDistance < car_.collision_radius() + ball_.radius()) {
			float wantedDistance = car_.collision_radius() + ball_.radius();
			float resultedDistance = wantedDistance - curDistance;
			//Move ball by getting the vector between it, getting a normalized vector, and moving the ball out by an appropriate vector
			Vector3 cartoball = ball_.position() - car_.position();
			Vector3 carNormal = Vector3::Normalize(cartoball);
			Vector3 addedDistance = resultedDistance * carNormal;
			
			//Set ball position to be touching
			ball_.set_position(ball_.position() + addedDistance);
			
			//Set velocity based on relative velocity, reflection, and friction
			Vector3 relVel = ball_.velocity() - car_.speed();
			Vector3 newVel = Reflection(relVel, carNormal);
			Vector3 resVel = newVel - (newVel / 5.0000000f);
			ball_.set_velocity(resVel);
			ball_.set_position(newPosition(ball_.position(), ball_.velocity(), timeStep));

		}
		

		//I'm only going to comment this first part because the x,y, and z are all the same.
		//I check if the distance between the ball and radius are == 0.
		//And I also check if the ball is heading towards this direction by checking what its position would be if i moved it.
		//If they are exactly touching, I make it bounce
		if (ball_.position().y() - ball_.radius() == 0 && newPos.y() - ball_.radius() < 0) {
			Vector3 newVel = Reflection(ball_.velocity(), Vector3(0, 1, 0));
			Vector3 resVel = newVel - (newVel / 5.0000000f);
			ball_.set_velocity(resVel);
			ball_.set_position(newPosition(ball_.position(), ball_.velocity(), timeStep));

		}
		//If they are intersecting, then I make the ball position equal to touching the floor.
		//This way, next frame, the difference will be 0, and they will bounce.
		else if (newPos.y() - ball_.radius() < 0.0f) {
			ball_.set_position(Point3(fixed.x(), ball_.radius(), fixed.z()));
		}

		if (ball_.position().y() + ball_.radius() == 35.0f && newPos.y() + ball_.radius() > 35.0f) {
			Vector3 newVel = Reflection(ball_.velocity(), Vector3(0, -1, 0));
			Vector3 resVel = newVel - (newVel / 5.0f);
			ball_.set_velocity(resVel);
			ball_.set_position(newPosition(ball_.position(), ball_.velocity(), timeStep));
		}
		else if (newPos.y() + ball_.radius() > 35.0f) {
			ball_.set_position(Point3(fixed.x(), 35.0f - ball_.radius(), fixed.z()));
		}

		/// X

		if ((ball_.position().x() - ball_.radius() == -40.0f) && newPos.x() - ball_.radius() < -40) {
			Vector3 newVel = Reflection(ball_.velocity(), Vector3(-1, 0, 0));
			Vector3 resVel = newVel - (newVel / 5.0f);
			ball_.set_velocity(resVel); 
			ball_.set_position(newPosition(ball_.position(), ball_.velocity(), timeStep));
		}
		else if (newPos.x() - ball_.radius() < -40.0f) {
			ball_.set_position(Point3(-40.0f + ball_.radius(), fixed.y(), fixed.z()));
		}

		if ((ball_.position().x() + ball_.radius() == 40.0f) && newPos.x() + ball_.radius() > 40) {
			Vector3 newVel = Reflection(ball_.velocity(), Vector3(1, 0, 0));
			Vector3 resVel = newVel - (newVel / 5);
			ball_.set_velocity(resVel); 
			ball_.set_position(newPosition(ball_.position(), ball_.velocity(), timeStep));
		}
		else if (newPos.x() + ball_.radius() > 40) {
			ball_.set_position(Point3(40 - ball_.radius(), fixed.y(), fixed.z()));
		}


		/// Z
		//Car Goals

		if ((ball_.position().z() - ball_.radius() == -50) && newPos.z() - ball_.radius() < -50) {
			Vector3 newVel = Reflection(ball_.velocity(), Vector3(0, 0, 1));
			Vector3 resVel = newVel - (newVel / 5);
			ball_.set_velocity(resVel);
			ball_.set_position(newPosition(ball_.position(), ball_.velocity(), timeStep));
		}
		else if (newPos.z() - ball_.radius() < -50) {
			if (newPos.x() >= -10 && newPos.x() <= 10 && newPos.y() > 0 && newPos.y() < 10) {
				ball_.set_position(newPos);
				ball_.Reset();
				car_.Reset();
				return;
			}
			ball_.set_position(Point3(fixed.x(), fixed.y(), -50 + ball_.radius()));
		}

		if ((ball_.position().z() + ball_.radius() == 50) && newPos.z() + ball_.radius() > 50) {
			Vector3 newVel = Reflection(ball_.velocity(), Vector3(0, 0, -1));
			Vector3 resVel = newVel - (newVel / 5);
			ball_.set_velocity(resVel);
			ball_.set_position(newPosition(ball_.position(), ball_.velocity(), timeStep));
		}
		else if (newPos.z() + ball_.radius() > 50) {
			if (newPos.x() >= -10 && newPos.x() <= 10 && newPos.y() > 0 && newPos.y() < 10) {
				ball_.set_position(newPos);
				ball_.Reset();
				car_.Reset();
				return;
			}
			ball_.set_position(Point3(fixed.x(), fixed.y(), 50 - ball_.radius()));
		}
	}

	//If its just somewhere in the air, make it fall or rise as normal
	else {
		ball_.set_position(ball_.position() + (ball_.velocity() * timeStep));
	}


	//Gravity
	float x = ball_.velocity().x();
	float y = ball_.velocity().y();
	y -= (9.8 * timeStep);
	float z = ball_.velocity().z();
	ball_.set_velocity(Vector3(x, y, z));

}


void CarSoccer::InitOpenGL() {
    // Set up the camera in a good position to see the entire field
    projMatrix_ = Matrix4::Perspective(60, aspect_ratio(), 1, 1000);
    modelMatrix_ = Matrix4::LookAt(Point3(0,60,70), Point3(0,0,10), Vector3(0,1,0));
 
    // Set a background color for the screen
    glClearColor(0.8,0.8,0.8, 1);
    
    // Load some image files we'll use
    fieldTex_.InitFromFile(Platform::FindFile("pitch.png", searchPath_));
    crowdTex_.InitFromFile(Platform::FindFile("crowd.png", searchPath_));
}

void CarSoccer::DrawUsingOpenGL() {
    // Draw the crowd as a fullscreen background image
    quickShapes_.DrawFullscreenTexture(Color(1,1,1), crowdTex_);
    
    // Draw the field with the field texture on it.
    Color col(16.0/255.0, 46.0/255.0, 9.0/255.0);
    Matrix4 M = Matrix4::Translation(Vector3(0,-0.201,0)) * Matrix4::Scale(Vector3(50, 1, 60));
    quickShapes_.DrawSquare(modelMatrix_ * M, viewMatrix_, projMatrix_, col);
    M = Matrix4::Translation(Vector3(0,-0.2,0)) * Matrix4::Scale(Vector3(40, 1, 50));
    quickShapes_.DrawSquare(modelMatrix_ * M, viewMatrix_, projMatrix_, Color(1,1,1), fieldTex_);
    
    // Draw the car
    Color carcol(0.8, 0.2, 0.2);
    Matrix4 Mcar =
        Matrix4::Translation(car_.position() - Point3(0,0,0)) *
        Matrix4::RotationY(car_.angle()) *
        Matrix4::Scale(car_.size()) *
        Matrix4::Scale(Vector3(0.5,0.5,0.5));
    quickShapes_.DrawCube(modelMatrix_ * Mcar, viewMatrix_, projMatrix_, carcol);
    
    
    // Draw the ball
    Color ballcol(1,1,1);
    Matrix4 Mball =
        Matrix4::Translation(ball_.position() - Point3(0,0,0)) *
        Matrix4::Scale(Vector3(ball_.radius(), ball_.radius(), ball_.radius()));
    quickShapes_.DrawSphere(modelMatrix_ * Mball, viewMatrix_, projMatrix_, ballcol);
    
    
    // Draw the ball's shadow -- this is a bit of a hack, scaling Y by zero
    // flattens the sphere into a pancake, which we then draw just a bit
    // above the ground plane.
    Color shadowcol(0.2,0.4,0.15);
    Matrix4 Mshadow =
        Matrix4::Translation(Vector3(ball_.position()[0], -0.1, ball_.position()[2])) *
        Matrix4::Scale(Vector3(ball_.radius(), 0, ball_.radius())) *
        Matrix4::RotationX(90);
    quickShapes_.DrawSphere(modelMatrix_ * Mshadow, viewMatrix_, projMatrix_, shadowcol);
    
    
    // You should add drawing the goals and the boundary of the playing area
    // using quickShapes_.DrawLines()
	Color white(1, 1, 1);

	//Ceiling Boundary
	//The Ceiling Boundary is not needed as it is technically drawn in by the other functions but I wrote it anyways
	std::vector<Point3> cloop;
	cloop.push_back(Point3(-40.0, 35.0, -50.0));
	cloop.push_back(Point3(-40.0, 35.0, 50.0));
	cloop.push_back(Point3(40.0, 35.0, 50.0));
	cloop.push_back(Point3(40.0, 35.0, -50.0));

	quickShapes_.DrawLines(modelMatrix_, viewMatrix_, projMatrix_, white, cloop, QuickShapes::LinesType::LINE_LOOP, 0.1);
	
	std::vector<Point3> thloop;
	//Top Half of Field Boundary
	thloop.push_back(Point3(-40, 0, -50));
	thloop.push_back(Point3(-40, 0, 0));
	thloop.push_back(Point3(40, 0, 0));
	thloop.push_back(Point3(40, 0, -50));

	quickShapes_.DrawLines(modelMatrix_, viewMatrix_, projMatrix_, white, thloop, QuickShapes::LinesType::LINE_LOOP, 0.1);

	std::vector<Point3> bhloop;
	bhloop.push_back(Point3(-40, 0, 50));
	bhloop.push_back(Point3(-40, 0, 0));
	bhloop.push_back(Point3(40, 0, 0));
	bhloop.push_back(Point3(40, 0, 50));

	quickShapes_.DrawLines(modelMatrix_, viewMatrix_, projMatrix_, white, bhloop, QuickShapes::LinesType::LINE_LOOP, 0.1);

	std::vector<Point3> lsloop;
	lsloop.push_back(Point3(-40, 0, -50));
	lsloop.push_back(Point3(-40, 0, 50));
	lsloop.push_back(Point3(-40, 35, 50));
	lsloop.push_back(Point3(-40, 35, -50));

	quickShapes_.DrawLines(modelMatrix_, viewMatrix_, projMatrix_, white, lsloop, QuickShapes::LinesType::LINE_LOOP, 0.1);

	std::vector<Point3> thsloop;
	thsloop.push_back(Point3(-40, 0, -50));
	thsloop.push_back(Point3(40, 0, -50));
	thsloop.push_back(Point3(40, 35, -50));
	thsloop.push_back(Point3(-40, 35, -50));

	quickShapes_.DrawLines(modelMatrix_, viewMatrix_, projMatrix_, white, thsloop, QuickShapes::LinesType::LINE_LOOP, 0.1);

	std::vector<Point3> rsloop;
	rsloop.push_back(Point3(40, 0, -50));
	rsloop.push_back(Point3(40, 0, 50));
	rsloop.push_back(Point3(40, 35, 50));
	rsloop.push_back(Point3(40, 35, -50));

	quickShapes_.DrawLines(modelMatrix_, viewMatrix_, projMatrix_, white, rsloop, QuickShapes::LinesType::LINE_LOOP, 0.1);

	std::vector<Point3> bhsloop;
	bhsloop.push_back(Point3(-40, 0, 50));
	bhsloop.push_back(Point3(40, 0, 50));
	bhsloop.push_back(Point3(40, 35, 50));
	bhsloop.push_back(Point3(-40, 35, 50));

	quickShapes_.DrawLines(modelMatrix_, viewMatrix_, projMatrix_, white, bhsloop, QuickShapes::LinesType::LINE_LOOP, 0.1);

	std::vector<Point3> bhglloop;
	Color orange(1, 0.647, 0);
	int i = 0;
	while (i <= 10) {
		bhglloop.push_back(Point3(-10, i, 50));
		bhglloop.push_back(Point3(10, i, 50));
		quickShapes_.DrawLines(modelMatrix_, viewMatrix_, projMatrix_, orange, bhglloop, QuickShapes::LinesType::LINES, 0.1);
		i++;
	}
	i = -10;
	while (i <= 10) {
		bhglloop.push_back(Point3(i, 10, 50));
		bhglloop.push_back(Point3(i, 0, 50));
		quickShapes_.DrawLines(modelMatrix_, viewMatrix_, projMatrix_, orange, bhglloop, QuickShapes::LinesType::LINES, 0.1);
		i++;
	}

	std::vector<Point3> thglloop;
	Color blue(0.1176, 0.5647, 1);
	i = 0;
	while (i <= 10) {
		thglloop.push_back(Point3(-10, i, -50));
		thglloop.push_back(Point3(10, i, -50));
		quickShapes_.DrawLines(modelMatrix_, viewMatrix_, projMatrix_, blue, thglloop, QuickShapes::LinesType::LINES, 0.1);
		i++;
	}
	i = -10;
	while (i <= 10) {
		thglloop.push_back(Point3(i, 10, -50));
		thglloop.push_back(Point3(i, 0, -50));
		quickShapes_.DrawLines(modelMatrix_, viewMatrix_, projMatrix_, blue, thglloop, QuickShapes::LinesType::LINES, 0.1);
		i++;
	}

	//Ball keeps bouncing
	//Make ball position correct.
}
