#pragma once

#include <stdint.h>

// the IRenderBackend interface abstracts away details of what graphics API is used for implementing rendering

struct IRenderBackend {
  virtual ~IRenderBackend() {}

  virtual RenderBackendType Type() const = 0;
};
