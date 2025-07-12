#include "scripts/elephant_script.h"

#include "MLScript.h"

#include "MLCore.h"


DEFAULT_CREATE_FUNCTION(ElephantScript, ElephantContext)

READY_FUNCTION(ElephantScript)
{

}

UPDATE_FUNCTION(ElephantScript)
{

}

DESTROY_FUNCTION(ElephantScript)
{

}

COLLISION_ENTER_FUNCTION(ElephantScript)
{
	printf("collision entered!\n");
}

COLLISION_EXIT_FUNCTION(ElephantScript)
{
	printf("Collision Exited!\n");
}

