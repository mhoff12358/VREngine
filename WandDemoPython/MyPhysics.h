#pragma once

#include "stdafx.h"

#include "SceneSystem/PhysXInternal.h"

namespace mp {

struct DummyTop {};

template <typename T, typename Parent>
struct DW : public Parent {
	T* real() { return static_cast<T*>(this); }
	const T* real() const { return static_cast<T*>(this); }
};

template <typename T>
struct dw_trait {
	typedef void real_type;
	typedef void dw_type;
};

#define DWWRAP(child, parent) \
typedef DW<::child, parent> child; \
template <> \
struct dw_trait<child> { typedef ::child real_type; typedef child dw_type; }; \
template <> \
struct dw_trait<::child> { typedef ::child real_type; typedef child dw_type; };

template <typename T>
typename dw_trait<T>::real_type* Unwrap(T* t) { return reinterpret_cast<dw_trait<T>::real_type*>(t); }
template <typename T>
typename const dw_trait<T>::real_type* Unwrap(const T* t) { return reinterpret_cast<const dw_trait<T>::real_type*>(t); }
template <typename T>
typename dw_trait<T>::real_type& Unwrap(T& t) { return reinterpret_cast<dw_trait<T>::real_type&>(t); }
template <typename T>
typename const dw_trait<T>::real_type& Unwrap(const T& t) { return reinterpret_cast<const dw_trait<T>::real_type&>(t); }

template <typename T>
typename dw_trait<T>::dw_type* Wrap(T* t) { return reinterpret_cast<dw_trait<T>::dw_type*>(t); }
template <typename T>
typename const dw_trait<T>::dw_type* Wrap(const T* t) { return reinterpret_cast<const dw_trait<T>::dw_type*>(t); }
template <typename T>
typename dw_trait<T>::dw_type& Wrap(T& t) { return reinterpret_cast<dw_trait<T>::dw_type&>(t); }
template <typename T>
typename const dw_trait<T>::dw_type& Wrap(const T& t) { return reinterpret_cast<const dw_trait<T>::dw_type&>(t); }

DWWRAP(PxActor, DummyTop)
DWWRAP(PxRigidActor, PxActor)
DWWRAP(PxRigidBody, PxRigidActor)
DWWRAP(PxRigidDynamic, PxRigidBody)
DWWRAP(PxRigidStatic, PxRigidActor)

DWWRAP(PxMaterial, DummyTop)

DWWRAP(PxScene, DummyTop)

DWWRAP(PxPhysics, DummyTop)

}