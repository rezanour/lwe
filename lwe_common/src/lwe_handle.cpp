#include <lwe_assert.h>
#include <lwe_handle.h>

#include <atomic>

namespace {
  struct Handle {
    uint16_t type;
    uint16_t generation;
    uint32_t index;
  };

  struct TableEntry {
    uint16_t type;
    uint16_t generation;
    void    *object;
  };

  constexpr uint32_t      MaxEntries                 = 100000;
  TableEntry              g_handle_table[MaxEntries] = {};
  uint32_t                g_free_list[MaxEntries]    = {};
  std::atomic_uint32_t    g_free_list_size           = ATOMIC_VAR_INIT(0);
  std::atomic_uint32_t    g_next_unused_index        = ATOMIC_VAR_INIT(0);
  std::atomic_uint16_t    g_next_type                = ATOMIC_VAR_INIT(1);
}

bool LWEHandleIsValid(LWEHandle handle) {
  if (LWE_INVALID_HANDLE == handle) {
    return false;
  }

  Handle const *h = reinterpret_cast<Handle const *>(&handle);
  if (h->index >= g_next_unused_index) {
    return false;
  }

  TableEntry const &entry = g_handle_table[h->index];
  if (entry.type != h->type || entry.generation != h->generation) {
    return false;
  }

  return true;
}

uint16_t LWEHandleRegisterType() {
  return g_next_type++;
}

uint16_t LWEHandleGetType(LWEHandle handle) {
  Handle const *h = reinterpret_cast<Handle const *>(&handle);
  return h->type;
}

LWEHandle LWEHandleCreate(uint16_t type, void *object) {
  uint32_t index = static_cast<uint32_t>(-1);
  while (true) {
    uint32_t free_list_size = g_free_list_size;
    if (0 == free_list_size) {
      break;
    }

    uint32_t new_free_list_size = free_list_size - 1;
    if (std::atomic_compare_exchange_weak(&g_free_list_size, &free_list_size, new_free_list_size)) {
      index = free_list_size;
      break;
    }
  }

  if (static_cast<uint32_t>(-1) == index) {
    index = g_next_unused_index++;
  }

  auto &entry = g_handle_table[index];
  entry.type = type;
  entry.object = object;

  Handle h = {};
  h.type = type;
  h.index = index;
  h.generation = entry.generation;

  return *reinterpret_cast<LWEHandle *>(&h);
}

void *LWEHandleGetObject(LWEHandle handle, uint16_t expected_type) {
  Handle const *h = reinterpret_cast<Handle const *>(&handle);
  if (h->type != expected_type) {
    return nullptr;
  }

  if (h->index >= g_next_unused_index) {
    return nullptr;
  }

  TableEntry const &entry = g_handle_table[h->index];
  if (entry.type != h->type || entry.generation != h->generation) {
    return false;
  }

  return entry.object;
}

void LWEHandleDestroy(LWEHandle handle) {
  Handle const *h = reinterpret_cast<Handle const *>(&handle);

  if (h->index >= g_next_unused_index) {
    return;
  }

  TableEntry &entry = g_handle_table[h->index];
  if (entry.type != h->type || entry.generation != h->generation) {
    LWE_ASSERT(false);
    return;
  }

  ++entry.generation;
  entry.object = nullptr;

  uint32_t free_index = g_free_list_size++;
  g_free_list[free_index] = h->index;
}
