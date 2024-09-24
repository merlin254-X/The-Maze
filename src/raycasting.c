#include "../headers/header.h"

ray_t rays[NUM_RAYS]; /* Array to store data for each raycast like distance & wall hit */

static bool foundHorzWallHit, foundVertWallHit; /* flags indicating if a wall hit was found*/
static float horzWallHitX, horzWallHitY, vertWallHitX, vertWallHitY; /* coordinates of the wall hit*/
static int horzWallContent, vertWallContent; /* content of the map at the wall hit locations */


/**
 * horzIntersection - Finds horizontal intersection with the wall
 * @rayAngle: current ray angle
 *
 */

void horzIntersection(float rayAngle)
{
	/* Variables for next points of intersection, and steps */
	float nextHorzTouchX, nextHorzTouchY, xintercept, yintercept, xstep, ystep;

	foundHorzWallHit = false; /* reset wall hit flag*/
	horzWallHitX = horzWallHitY = horzWallContent = 0; /* Reset hit position and content */


	/* Calculate y-intercept of the first horizontal grid line the ray touches */
	yintercept = floor(player.y / TILE_SIZE) * TILE_SIZE;
	yintercept += isRayFacingDown(rayAngle) ? TILE_SIZE : 0;/* Adjust if facing downwards */

	/* Calculate x-intercept using the ray angle */
	xintercept = player.x + (yintercept - player.y) / tan(rayAngle);

	/* Determine the step sizes based on ray direction */
	ystep = TILE_SIZE;
	ystep *= isRayFacingUp(rayAngle) ? -1 : 1; /* Step up or down */
	xstep = TILE_SIZE / tan(rayAngle); /* Horizontal step adjusted for angle  */
	xstep *= (isRayFacingLeft(rayAngle) && xstep > 0) ? -1 : 1; /* Adjust for leftward rays  */
	xstep *= (isRayFacingRight(rayAngle) && xstep < 0) ? -1 : 1; /* Adjust for rightward rays */
	nextHorzTouchX = xintercept;
	nextHorzTouchY = yintercept;

	/* Traverse through the grid while inside map boundaries */
	while (isInsideMap(nextHorzTouchX, nextHorzTouchY))
	{
		float xToCheck = nextHorzTouchX;
		float yToCheck = nextHorzTouchY + (isRayFacingUp(rayAngle) ? -1 : 0);

		/* Check for wall collision */
		if (DetectCollision(xToCheck, yToCheck))
		{
			/* Record the hit coordinates and content */
			horzWallHitX = nextHorzTouchX;
			horzWallHitY = nextHorzTouchY;
			horzWallContent = getMapValue((int)floor(yToCheck / TILE_SIZE),
									   (int)floor(xToCheck / TILE_SIZE));
			foundHorzWallHit = true;
			break; /* Stop if wall hit is found */
		}

		/* Move to the next intersection point */
		nextHorzTouchX += xstep;
		nextHorzTouchY += ystep;
	}
}

/**
 * vertIntersection - Finds vertical intersection with the wall
 * @rayAngle: current ray angle
 *
 */

void vertIntersection(float rayAngle)
{
	float nextVertTouchX, nextVertTouchY;
	float xintercept, yintercept, xstep, ystep;

	foundVertWallHit = false;
	vertWallHitX = 0;
	vertWallHitY = 0;
	vertWallContent = 0;

	xintercept = floor(player.x / TILE_SIZE) * TILE_SIZE;
	xintercept += isRayFacingRight(rayAngle) ? TILE_SIZE : 0;
	yintercept = player.y + (xintercept - player.x) * tan(rayAngle);

	xstep = TILE_SIZE;
	xstep *= isRayFacingLeft(rayAngle) ? -1 : 1;
	ystep = TILE_SIZE * tan(rayAngle);
	ystep *= (isRayFacingUp(rayAngle) && ystep > 0) ? -1 : 1;
	ystep *= (isRayFacingDown(rayAngle) && ystep < 0) ? -1 : 1;
	nextVertTouchX = xintercept;
	nextVertTouchY = yintercept;

	while (isInsideMap(nextVertTouchX, nextVertTouchY))
	{
		float xToCheck = nextVertTouchX + (isRayFacingLeft(rayAngle) ? -1 : 0);
		float yToCheck = nextVertTouchY;

		if (DetectCollision(xToCheck, yToCheck))
		{
			vertWallHitX = nextVertTouchX;
			vertWallHitY = nextVertTouchY;
			vertWallContent = getMapValue((int)floor(yToCheck / TILE_SIZE),
									   (int)floor(xToCheck / TILE_SIZE));
			foundVertWallHit = true;
			break;
		}
		nextVertTouchX += xstep;
		nextVertTouchY += ystep;
	}
}

/**
 * castRay - casting of each ray
 * @rayAngle: current ray angle
 * @stripId: ray strip identifier
 */

void castRay(float rayAngle, int stripId)
{
	float horzHitDistance, vertHitDistance;

	rayAngle = remainder(rayAngle, TWO_PI);
	if (rayAngle < 0)
		rayAngle = TWO_PI + rayAngle;

	horzIntersection(rayAngle);

	vertIntersection(rayAngle);

	horzHitDistance = foundHorzWallHit
		? distanceBetweenPoints(player.x, player.y, horzWallHitX, horzWallHitY)
		: FLT_MAX;
	vertHitDistance = foundVertWallHit
		? distanceBetweenPoints(player.x, player.y, vertWallHitX, vertWallHitY)
		: FLT_MAX;

	if (vertHitDistance < horzHitDistance)
	{
		rays[stripId].distance = vertHitDistance;
		rays[stripId].wallHitX = vertWallHitX;
		rays[stripId].wallHitY = vertWallHitY;
		rays[stripId].wallHitContent = vertWallContent;
		rays[stripId].wasHitVertical = true;
		rays[stripId].rayAngle = rayAngle;
	}
	else
	{
		rays[stripId].distance = horzHitDistance;
		rays[stripId].wallHitX = horzWallHitX;
		rays[stripId].wallHitY = horzWallHitY;
		rays[stripId].wallHitContent = horzWallContent;
		rays[stripId].wasHitVertical = false;
		rays[stripId].rayAngle = rayAngle;
	}

}

/**
 * castAllRays - cast of all rays
 *
 */

void castAllRays(void)
{
	int col;

	for (col = 0; col < NUM_RAYS; col++)
	{
		float rayAngle = player.rotationAngle +
							atan((col - NUM_RAYS / 2) / PROJ_PLANE);
		castRay(rayAngle, col);
	}
}

/**
 * renderRays - draw all the rays
 *
 */

void renderRays(void)
{
	int i;

	for (i = 0; i < NUM_RAYS; i += 50)
	{
		drawLine(
			player.x * MINIMAP_SCALE_FACTOR,
			player.y * MINIMAP_SCALE_FACTOR,
			rays[i].wallHitX * MINIMAP_SCALE_FACTOR,
			rays[i].wallHitY * MINIMAP_SCALE_FACTOR,
			0xFF0000FF
		);
	}
}
