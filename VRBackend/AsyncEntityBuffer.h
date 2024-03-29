#pragma once

#include "stdafx.h"
#include <condition_variable>

class RenderGroup;

class AsyncEntityBuffer
{
public:
	AsyncEntityBuffer();
	~AsyncEntityBuffer();

	RenderGroup* Initialize(unsigned int requested_render_groups);

	// These two methods are called by the writer and consumer repsectively to
	// alert that they have finished with the RenderGroup they were using.
	RenderGroup* ProducerFinish();
	RenderGroup* ConsumerFinish();

	void BlockUntilConsumerDone();

private:
	mutex state_ownership_lock;

	mutex consumer_wait_mutex;
	std::condition_variable consumer_wait;
	bool consumer_alerted = false;

	// This is the number of render groups for each group, so there are 3*num_render_groups individual RenderGroup objects.
	unsigned int num_render_groups;

	vector<RenderGroup> render_groups;
	RenderGroup* writing_group;
	RenderGroup* new_reading_group;
	RenderGroup* old_reading_group;
	bool reader_on_new;
};

