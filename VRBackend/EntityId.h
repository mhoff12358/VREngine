#pragma once
class EntityId
{
public:
	EntityId(unsigned int render_group_index, unsigned int entity_index);
	~EntityId();

	unsigned int render_group_index_;
	unsigned int entity_index_;
};

