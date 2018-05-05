#pragma once

#include <lwe_platform.h>

typedef uint64_t LWEHandle;

constexpr LWEHandle LWE_INVALID_HANDLE = 0;

bool     LWEHandleIsValid(LWEHandle handle);

uint16_t LWEHandleRegisterType();
uint16_t LWEHandleGetType(LWEHandle handle);

LWEHandle LWEHandleCreate(uint16_t type, void *object);
void     *LWEHandleGetObject(LWEHandle handle, uint16_t expected_type);

// note: this doesn't delete the underlying object. That should be deleted before destroying handle
void      LWEHandleDestroy(LWEHandle handle);
