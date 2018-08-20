/*
 * Copyright 2018 Google LLC. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. You may obtain a copy of
 * the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations under
 * the License.
 */
#include <emscripten.h>
#include <chipmunk/chipmunk.h>
#include <stdlib.h>
#include <memory.h>

/**
 * A struct that manages our instantiation of the Chipmunk2D physics engine
 */
typedef struct {
    cpSpace *cmi_Space;
    float cmi_Location[3];
} CM_Instance;

/**
 * Constructor for the Chipmunk2D engine
 */
CM_Instance *
CM_Instance_new() 
{
    CM_Instance *cm;

    cm = malloc(sizeof *cm);
    if (cm) 
    {
        memset(cm, 0, sizeof *cm);
    }

    // cpVect is a 2D vector and cpv() is a shortcut for initializing them.
    cpVect gravity = cpv(0, 50);

    // Create an empty space.
    cm->cmi_Space = cpSpaceNew();
    cpSpaceSetGravity(cm->cmi_Space, gravity);

    return cm;
}

/**
 * Descturo for when we are all done.
 * Frees everything
 */
void 
CM_Instance_destroy(CM_Instance *cmi) 
{
    if (cmi != NULL)
    {
        cpSpaceFree(cmi->cmi_Space);

        free(cmi);
    }
}

/**
 * Add a static wall to our space.
 * This is for containing the balls as they're dropping through the space.
 * 
 */
void
CM_Add_wall(CM_Instance *cmi, int id, int x1, int y1, int x2, int y2)
{
    cpVect p1, p2;
    cpShape *segment;

    p1.x = x1;
    p1.y = y1;

    p2.x = x2;
    p2.y = y2;

    segment = cpSegmentShapeNew(cpSpaceGetStaticBody(cmi->cmi_Space), p1, p2, 1.5);
    cpShapeSetFriction(segment, 1);
    cpSpaceAddShape(cmi->cmi_Space, segment);
}

/** 
 * Add a circular dynamic object to our space.
 * This is for adding grains of sand.
 * 
 * @return Pointer to the cpBody created 0 on error.
 */
cpBody *
CM_Add_circle(CM_Instance *cmi, int id, float x, float y, float radius)
{
    static cpFloat mass = 1;
    cpFloat moment = cpMomentForCircle(mass, 0, radius, cpvzero);
    cpBody *ballBody = cpSpaceAddBody(cmi->cmi_Space, cpBodyNew(mass, moment));
    cpShape *ballShape;

    // Set the position of the circle.
    cpBodySetPosition(ballBody, cpv(x, y));
    // Add the collision shape to the circle.
    ballShape = cpSpaceAddShape(cmi->cmi_Space, 
                                cpCircleShapeNew(ballBody, radius, cpvzero)
                );
    cpShapeSetElasticity(ballShape, 0.0);
    cpShapeSetFriction(ballShape, 0.0);

    return ballBody;
}

/**
 * Step the space.
 * This runs one tick of the simulation, and upates all our object positions.
 * To be used with requestAnimationFrame.
 */
void
CM_Step(CM_Instance *cmi)
{
    static cpFloat timeStep = 1.0 / 60.0;
    cpSpaceStep(cmi->cmi_Space, timeStep);
}

/**
 * Get the location of an object in our space.
 * 
 * @return An array of 3 floats - x, y position and rotation
 */
float *
CM_Get_location(CM_Instance *cmi, cpBody *what) 
{
    cpVect pos = cpBodyGetPosition(what);
    cpFloat rot = cpBodyGetAngularVelocity(what);

    cmi->cmi_Location[0] = pos.x;
    cmi->cmi_Location[1] = pos.y;
    cmi->cmi_Location[2] = rot;

    return cmi->cmi_Location;
}

/**
 * Set the gravity vector.
 */
void
CM_Set_gravity(CM_Instance *cmi, float x, float y)
{
    cpVect gravity = cpv(x,y);
    cpSpaceSetGravity(cmi->cmi_Space, gravity);
}