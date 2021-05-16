/*-----------------------------------------------------------
  Simulation Header File
  -----------------------------------------------------------*/
#include"vecmath.h"

/*-----------------------------------------------------------
  Macros
  -----------------------------------------------------------*/
#define TABLE_X			(0.6f) 
#define TABLE_Z			(1.2f)
#define TABLE_Y			(0.1f)
#define BALL_RADIUS		(0.035f)
#define BALL_MASS		(0.1f)
#define TWO_PI			(6.2832f)
#define	SIM_UPDATE_MS	(10)
#define NUM_BALLS		(7)		
#define NUM_CUSHIONS	(4)		
#define MAX_PARTICLES	(200)
#define NUM_POCKETS		(6)
#define POCKET_RADIUS   (0.05f)
#define NUM_PLAYERS		(2)
#define BALL_POINTS		(10)
/*-----------------------------------------------------------
  plane normals
  -----------------------------------------------------------*/
/*
extern vec2	gPlaneNormal_Left;
extern vec2	gPlaneNormal_Top;
extern vec2	gPlaneNormal_Right;
extern vec2	gPlaneNormal_Bottom;
*/

/*-----------------------------------------------------------
player class
-----------------------------------------------------------*/
class player
{
	char*	name;
	int		score;
public:

	player() { name = 0; score = 0; }
	~player();

	void SetName(const char* _name);
	void UpdateScore(int _score);
	void ResetScore(void);
	int GetScore() const { return score; }
	const char* GetName() const { return name; }
};

/*-----------------------------------------------------------
scoreboard class
-----------------------------------------------------------*/
class scoreboard
{
public:
	player players[NUM_PLAYERS];
	bool player1Turn;

	void SetupPlayers(const char* _player1, const char* _player2);
	void Reset(void);
};

/*-----------------------------------------------------------
  cushion class
  -----------------------------------------------------------*/
class cushion
{
public:
	vec2	vertices[2]; //2d
	vec2	centre;
	vec2	normal;

	void MakeNormal(void);
	void MakeCentre(void);
};

/*-----------------------------------------------------------
pocket class
-----------------------------------------------------------*/
class pocket
{
public:
	vec2	position;
	float	raduis;

	pocket() { position = 0; raduis = POCKET_RADIUS; }
};

/*-----------------------------------------------------------
  ball class
  -----------------------------------------------------------*/

class ball
{
	static int ballIndexCnt;
public:
	vec2	position;
	vec2	velocity;
	float	radius;
	float	mass;
	int		index;
	pocket  *pPocket;
	int		points;
	bool	isCueBall;


	ball(): position(0.0), velocity(0.0), radius(BALL_RADIUS), 
		mass(BALL_MASS), pPocket(NULL), points(BALL_POINTS), isCueBall(false) {index = ballIndexCnt++; Reset();}
	
	void Reset(void);
	void ApplyImpulse(vec2 imp);
	void ApplyFrictionForce(int ms);
	void DoPlaneCollision(const cushion &c);
	void DoBallCollision(ball &b);
	void DoPocketCollision(pocket &p);
	void Update(int ms);
	
	bool HasHitPlane(const cushion &c) const;
	bool HasHitBall(const ball &b) const;
	bool HasHitPocket(const pocket &p) const;

	void HitPlane(const cushion &c);
	void HitBall(ball &b);
	void HitPocket(pocket &p);

	void SetCueBall(void);
};

class particle 
{
public:
	vec3 position;
	vec3 velocity;

	particle() {position=0;velocity=0;}
	void update(int ms);
};

class particleSet 
{
public:
	particle *particles[MAX_PARTICLES];
	int num;

	particleSet()
	{
		for(int i=0;i<MAX_PARTICLES;i++) particles[i] = 0;
		num=0;
	}

	~particleSet()
	{
		for(int i=0;i<MAX_PARTICLES;i++)
		{
			if(particles[i]) delete particles[i];
		}
	}

	void AddParticle(const vec3 &pos);
	void update(int ms);
};


/*-----------------------------------------------------------
  table class
  -----------------------------------------------------------*/
class table
{
public:
	ball balls[NUM_BALLS];	
	cushion cushions[NUM_CUSHIONS];
	pocket pockets[NUM_POCKETS];
	particleSet parts;
	scoreboard sb;

	void SetupCushions(void);
	void SetupPockets(void);
	void Update(int ms);	
	bool AnyBallsMoving(void) const;
};

/*-----------------------------------------------------------
  global table
  -----------------------------------------------------------*/
extern table gTable;
