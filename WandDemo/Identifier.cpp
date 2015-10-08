#include "stdafx.h"
#include "Identifier.h"


Identifier::Identifier()
{
}


Identifier::Identifier(string id) : id_(id)
{
}


Identifier::Identifier(const char* id) : id_(id)
{
}


Identifier::~Identifier()
{
}

const string& Identifier::GetId() const {
	return id_;
}

bool Identifier::operator==(const Identifier& other) {
	return id_ == other.id_;
}

std::ostream& operator<<(std::ostream& os, const Identifier& ident) {
	return os << ident.GetId();
}