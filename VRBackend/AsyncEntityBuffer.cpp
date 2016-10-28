#include "stdafx.h"
#include "AsyncEntityBuffer.h"

#include "RenderGroup.h"

AsyncEntityBuffer::AsyncEntityBuffer()
{
}


AsyncEntityBuffer::~AsyncEntityBuffer()
{
}

RenderGroup* AsyncEntityBuffer::Initialize(unsigned int requested_render_groups) {
	num_render_groups = requested_render_groups;
	render_groups.resize(num_render_groups * 3);
	writing_group = render_groups.data();
	new_reading_group = render_groups.data() + num_render_groups;
	old_reading_group = render_groups.data() + 2 * num_render_groups;
	return writing_group;
}

RenderGroup* AsyncEntityBuffer::ProducerFinish() {
	state_ownership_lock.lock();
	// Mark W as N
	RenderGroup* previous_new_reading_group = new_reading_group;
	new_reading_group = writing_group;

	if (reader_on_new) {
		// Mark O as W and N as O
		writing_group = old_reading_group;
		old_reading_group = previous_new_reading_group;
		reader_on_new = false;
	} else {
		// Mark N as W
		writing_group = previous_new_reading_group;
	}
	state_ownership_lock.unlock();

	// Copy the contents of the old writing group (now the new reading group)
	// to the new writing group
	for (unsigned int group_index = 0; group_index < num_render_groups; group_index++) {
		writing_group[group_index].Update(new_reading_group + group_index);
	}
	return writing_group;
}

RenderGroup* AsyncEntityBuffer::ConsumerFinish() {
	state_ownership_lock.lock();
	// Clean up the RenderGroup that is currently being released.
	RenderGroup* cleanup_group;
	if (reader_on_new) {
		cleanup_group = new_reading_group;
	} else {
		cleanup_group = old_reading_group;
	}
	for (int i = 0; i < num_render_groups; i++) {
		cleanup_group[i].Cleanup();
	}

	// Claim the current new reading group
	reader_on_new = true;
	RenderGroup* returned_group = new_reading_group;

	state_ownership_lock.unlock();
	return returned_group;
}