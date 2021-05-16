/*-----------------------------------------------------------
  Simulation Source File
  -----------------------------------------------------------*/
#include"stdafx.h"
#include"simulation.h"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>


/*-----------------------------------------------------------
  macros
  -----------------------------------------------------------*/
#define SMALL_VELOCITY		(0.01f)


/*-----------------------------------------------------------
  globals
  -----------------------------------------------------------*/
/*
vec2	gPlaneNormal_Left(15.0,0.0);
vec2	gPlaneNormal_Top(0.0,1.0);
vec2	gPlaneNormal_Right(-1.0,0.0);
vec2	gPlaneNormal_Bottom(0.0,-1.0);
*/

table gTable;

static const float gRackPositionX[] = {0.0f,0.0f,(BALL_RADIUS*2.0f),(-BALL_RADIUS*2.0f),(BALL_RADIUS*4.0f)}; 
static const float gRackPositionZ[] = {0.5f,0.0f,(-BALL_RADIUS*3.0f),(-BALL_RADIUS*3.0f)}; 

float gCoeffRestitution = 0.5f;
float gCoeffFriction = 0.03f;
float gGravityAccn = 9.8f;

/*-----------------------------------------------------------
player class members
-----------------------------------------------------------*/
void player::SetName(const char* _name)
{
	if (name != 0)
		delete[] name;

	name = new char[strlen(_name) + 1];
	strcpy(name, _name);
	score = 0;
}

player::~player()
{
	if (name != 0)
		delete[] name;
}

void player::UpdateScore(int _score)
{
	score += _score;
}

void player::ResetScore(void)
{
	score = 0;
}

/*-----------------------------------------------------------
scoreboard class members
-----------------------------------------------------------*/
void scoreboard::SetupPlayers(const char* _player1, const char* _player2)
{
	players[0].SetName(_player1);
	players[1].SetName(_player2);
}

void scoreboard::Reset(void)
{
	players[0].ResetScore();
	players[1].ResetScore();
}


/*-----------------------------------------------------------
  cushion class members
  -----------------------------------------------------------*/
void cushion::MakeNormal(void)
{
	//can do this in 2d
	vec2 temp = vertices[1]-vertices[0];
	normal(0) = temp(1);
	normal(1) = -temp(0);
	normal.Normalise();
}

void cushion::MakeCentre(void)
{
	centre = vertices[0];
	centre += vertices[1];
	centre/=2.0;
}

/*-----------------------------------------------------------
  ball class members
  -----------------------------------------------------------*/
int ball::ballIndexCnt = 0;

void ball::Reset(void)
{
	//set velocity to zero
	velocity = 0.0;

	//work out rack position
	if(index==0)
	{
		position(1) = 0.5;
		position(0) = 0.0;
		return;
	}
	
	static const float sep = (BALL_RADIUS*3.0f);
	static const float rowSep = (BALL_RADIUS*2.5f);
	int row = 1;
	int rowIndex = index;
	while(rowIndex > row)
	{
		rowIndex -= row;
		row++;
	}
	position(1) =  -(rowSep * (row-1));
	position(0) = (((row-1)*sep)/2.0f) - (sep*(row-rowIndex));
}

void ball::ApplyImpulse(vec2 imp)
{
	velocity = imp;
}

void ball::ApplyFrictionForce(int ms)
{
	if(velocity.Magnitude()<=0.0) return;

	//accelaration is opposite to direction of motion
	vec2 accelaration = -velocity.Normalised();
	//friction force = constant * mg
	//F=Ma, so accelaration = force/mass = constant*g
	accelaration *= (gCoeffFriction * gGravityAccn);
	//integrate velocity : find change in velocity
	vec2 velocityChange = ((accelaration * ms)/1000.0f);
	//cap magnitude of change in velocity to remove integration errors
	if(velocityChange.Magnitude() > velocity.Magnitude()) velocity = 0.0;
	else velocity += velocityChange;
}

void ball::DoBallCollision(ball &b)
{
	if(HasHitBall(b)) HitBall(b);
}

void ball::DoPlaneCollision(const cushion &b)
{
	if(HasHitPlane(b)) HitPlane(b);
}

void ball::DoPocketCollision(pocket &p)
{
	if (HasHitPocket(p)) HitPocket(p);
}

void ball::Update(int ms)
{
	//apply friction
	ApplyFrictionForce(ms);
	//integrate position
	position += ((velocity * ms)/1000.0f);
	//set small velocities to zero
	if(velocity.Magnitude()<SMALL_VELOCITY) velocity = 0.0;
}

bool ball::HasHitPlane(const cushion &c) const
{
	if (pPocket != NULL)
		return false;

	//if moving away from plane, cannot hit
	if(velocity.Dot(c.normal) >= 0.0) return false;
	
	//if in front of plane, then have not hit
	vec2 relPos = position - c.vertices[0];
	double sep = relPos.Dot(c.normal);
	if(sep > radius) return false;
	return true;
}

bool ball::HasHitPocket(const pocket &p) const
{
	//work out relative position of ball from pocket,
	//distance between ball and pocket
	//and relative velocity
	vec2 relPosn = position - p.position;
	float dist = (float)relPosn.Magnitude();
	vec2 relPosnNorm = relPosn.Normalised();
	
	//if moving away from plane, cannot hit
	if (velocity.Dot(relPosnNorm) >= 0.0) return false;
	//if distnce is more than sum of radii, have not hit
	if (dist > (radius + p.raduis)) return false;
	return true;
}

bool ball::HasHitBall(const ball &b) const
{
	if (pPocket != NULL || b.pPocket != NULL)
		return false;

	//work out relative position of ball from other ball,
	//distance between balls
	//and relative velocity
	vec2 relPosn = position - b.position;
	float dist = (float) relPosn.Magnitude();
	vec2 relPosnNorm = relPosn.Normalised();
	vec2 relVelocity = velocity - b.velocity;

	//if moving apart, cannot have hit
	if(relVelocity.Dot(relPosnNorm) >= 0.0) return false;
	//if distnce is more than sum of radii, have not hit
	if(dist > (radius+b.radius)) return false;
	return true;
}

void ball::HitPlane(const cushion &c)
{
	//reverse velocity component perpendicular to plane  
	double comp = velocity.Dot(c.normal) * (1.0+gCoeffRestitution);
	vec2 delta = -(c.normal * comp);
	velocity += delta; 

	//make some particles
	int n = (rand()%4)+3;
	vec3 pos(position(0),radius/2.0,position(1));
	vec3 oset(c.normal(0),0.0,c.normal(1));
	pos+=(oset*radius);
	for(int i=0;i<n;i++)
	{
		gTable.parts.AddParticle(pos);
	}

/*
	//assume elastic collision
	//find plane normal
	vec2 planeNorm = gPlaneNormal_Left;
	//split velocity into 2 components:
	//find velocity component perpendicular to plane
	vec2 perp = planeNorm*(velocity.Dot(planeNorm));
	//find velocity component parallel to plane
	vec2 parallel = velocity - perp;
	//reverse perpendicular component
	//parallel component is unchanged
	velocity = parallel + (-perp)*gCoeffRestitution;
*/
}

void ball::HitPocket(pocket &p)
{
	// set pocket for ball, prevent ball from being drawn...
	pPocket = &p;
	velocity = 0.0;

	// turn flag is already reversed by the time ball hits pocket
	if (gTable.sb.player1Turn)
		gTable.sb.players[1].UpdateScore(points);
	else
		gTable.sb.players[0].UpdateScore(points);
}

void ball::HitBall(ball &b)
{
	//find direction from other ball to this ball
	vec2 relDir = (position - b.position).Normalised();

	//split velocities into 2 parts:  one component perpendicular, and one parallel to 
	//the collision plane, for both balls
	//(NB the collision plane is defined by the point of contact and the contact normal)
	float perpV = (float)velocity.Dot(relDir);
	float perpV2 = (float)b.velocity.Dot(relDir);
	vec2 parallelV = velocity-(relDir*perpV);
	vec2 parallelV2 = b.velocity-(relDir*perpV2);
	
	//Calculate new perpendicluar components:
	//v1 = (2*m2 / m1+m2)*u2 + ((m1 - m2)/(m1+m2))*u1;
	//v2 = (2*m1 / m1+m2)*u1 + ((m2 - m1)/(m1+m2))*u2;
	float sumMass = mass + b.mass;
	float perpVNew = (float)((perpV*(mass-b.mass))/sumMass) + (float)((perpV2*(2.0*b.mass))/sumMass);
	float perpVNew2 = (float)((perpV2*(b.mass-mass))/sumMass) + (float)((perpV*(2.0*mass))/sumMass);
	
	//find new velocities by adding unchanged parallel component to new perpendicluar component
	velocity = parallelV + (relDir*perpVNew);
	b.velocity = parallelV2 + (relDir*perpVNew2);


	//make some particles
	int n = (rand()%5)+5;
	vec3 pos(position(0),radius/2.0,position(1));
	vec3 oset(relDir(0),0.0,relDir(1));
	pos+=(oset*radius);
	for(int i=0;i<n;i++)
	{
		gTable.parts.AddParticle(pos);
	}
}

void ball::SetCueBall()
{
	isCueBall = true;
	points = 0;
}

/*-----------------------------------------------------------
  particle class members
  -----------------------------------------------------------*/
void particle::update(int ms)
{
	position += (velocity*ms)/1000.0;
	velocity(1) -= (4.0*ms)/1000.0; //(9.8*ms)/1000.0;
}

/*-----------------------------------------------------------
  particle set class members
  -----------------------------------------------------------*/
void particleSet::AddParticle(const vec3 &pos)
{
	if(num >= MAX_PARTICLES) return;
	particles[num] = new particle;
	particles[num]->position = pos;

	particles[num]->velocity(0) = ((rand() % 200)-100)/200.0;
	particles[num]->velocity(2) = ((rand() % 200)-100)/200.0;
	particles[num]->velocity(1) = 2.0*((rand() % 100)/100.0);

	num++;
}

void particleSet::update(int ms)
{
	int i=0;
	while(i<num)
	{
		particles[i]->update(ms);
		if((particles[i]->position(1) < 0.0) && (particles[i]->velocity(1)<0.0))
		{
			delete particles[i];
			particles[i] = particles[num-1];
			num--;
		}
		else i++;
	}
}

/*-----------------------------------------------------------
  table class members
  -----------------------------------------------------------*/
void table::SetupCushions(void)
{
	cushions[0].vertices[0](0) = -TABLE_X; 
	cushions[0].vertices[0](1) = -TABLE_Z; 
	cushions[0].vertices[1](0) = -TABLE_X; 
	cushions[0].vertices[1](1) = TABLE_Z; 

	cushions[1].vertices[0](0) = -TABLE_X;
	cushions[1].vertices[0](1) = TABLE_Z;
	cushions[1].vertices[1](0) = TABLE_X;
	cushions[1].vertices[1](1) = TABLE_Z;

	cushions[2].vertices[0](0) = TABLE_X;
	cushions[2].vertices[0](1) = TABLE_Z;
	cushions[2].vertices[1](0) = TABLE_X;
	cushions[2].vertices[1](1) = -TABLE_Z; // +0.3;

	cushions[3].vertices[0](0) = TABLE_X;// -0.3;
	cushions[3].vertices[0](1) = -TABLE_Z; 
	cushions[3].vertices[1](0) = -TABLE_X; 
	cushions[3].vertices[1](1) = -TABLE_Z; 

	for(int i=0;i<NUM_CUSHIONS;i++)
	{
		cushions[i].MakeCentre();
		cushions[i].MakeNormal();
	}
}

void table::SetupPockets(void)
{
	// bottom left
	pockets[0].position(0) = gTable.cushions[0].vertices[0](0);
	pockets[0].position(1) = gTable.cushions[0].vertices[0](1);
	// bottom right
	pockets[1].position(0) = gTable.cushions[2].vertices[0](0);
	pockets[1].position(1) = gTable.cushions[0].vertices[0](1);
	// top left
	pockets[2].position(0) = gTable.cushions[0].vertices[0](0);
	pockets[2].position(1) = gTable.cushions[2].vertices[0](1);
	// top right
	pockets[3].position(0) = gTable.cushions[2].vertices[0](0);
	pockets[3].position(1) = gTable.cushions[2].vertices[0](1);
	// middle left
	pockets[4].position(0) = gTable.cushions[0].vertices[0](0);
	pockets[4].position(1) = 0.0;
	// middle right
	pockets[5].position(0) = gTable.cushions[2].vertices[0](0);
	pockets[5].position(1) = 0.0;
}

void table::Update(int ms)
{
	//check for collisions for each ball
	for(int i=0;i<NUM_BALLS;i++) 
	{
		for (int j = (i + 1); j<NUM_POCKETS; j++)
		{
			balls[i].DoPocketCollision(pockets[j]);
		}

		for(int j=0;j<NUM_CUSHIONS;j++)
		{
			balls[i].DoPlaneCollision(cushions[j]);
		}

		for(int j=(i+1);j<NUM_BALLS;j++) 
		{
			balls[i].DoBallCollision(balls[j]);
		}
	}
	
	//update all balls
	for(int i=0;i<NUM_BALLS;i++) balls[i].Update(ms);

	//update particles
	parts.update(ms);

}

bool table::AnyBallsMoving(void) const
{
	//return true if any ball has a non-zero velocity
	for(int i=0;i<NUM_BALLS;i++) 
	{
		if(balls[i].velocity(0)!=0.0) return true;
		if(balls[i].velocity(1)!=0.0) return true;
	}
	return false;
}
