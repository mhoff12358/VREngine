#include "EntityId.h"


EntityId::EntityId(unsigned int render_group_index, unsigned int entity_index)
	: render_group_index_(render_group_index), entity_index_(entity_index)
{
}


EntityId::~EntityId()
{
}
