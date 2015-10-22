#pragma once
#include "stdafx.h"

class Identifier
{
public:
	Identifier();
	Identifier(string id);
	Identifier(const char* id);
	~Identifier();

	const string& GetId() const;

	bool operator==(const Identifier& other);
	bool operator!=(const Identifier& other);

private:
	string id_;
};


std::ostream& operator<<(std::ostream& os, const Identifier& ident);
