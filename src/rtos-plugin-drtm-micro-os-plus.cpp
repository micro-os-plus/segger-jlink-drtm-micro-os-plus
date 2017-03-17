/*
 * This file is part of the µOS++ distribution.
 *   (https://github.com/micro-os-plus)
 * Copyright (c) 2017 Liviu Ionescu.
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom
 * the Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

/*
 * The plug-in client API definitions (the RTOS_* functions) and the
 * GDB server API definitions are compatible with the SEGGER GDBServer
 * RTOS Plug-in SDK API definitions.
 *
 * All IP rights, title and interest in the GDBServer RTOS Plug-in SDK
 * are and shall at all times remain with SEGGER.
 *
 * Copyright (c) 2004-2016 SEGGER Microcontroller GmbH & Co. KG
 * Internet: www.segger.com        Support:  support@segger.com
 */

#include <drtm/drtm.h>
#include <assert.h>
#include <segger-jlink-rtos-plugin-sdk/drtm-backend.h>
#include <segger-jlink-rtos-plugin-sdk/drtm-memory.h>
#include <stdio.h>
#include <new>

// ---------------------------------------------------------------------------

#define PLUGIN_API_VERSION 100
//#define USE_CUSTOM_ALLOCATOR

// ---------------------------------------------------------------------------
// Templates.

// Template explicit instantiation.
template class segger::drtm::backend<rtos_plugin_server_api_t,
    rtos_plugin_symbols_t>;
// Define a type alias.
using backend_t = class segger::drtm::backend<rtos_plugin_server_api_t,
rtos_plugin_symbols_t>;

#if defined(USE_CUSTOM_ALLOCATOR)

// Template explicit instantiation.
template class segger::drtm::memory_resource<rtos_plugin_server_api_t>;
// Define a type alias.
using backend_memory_resource_t = class segger::drtm::memory_resource<rtos_plugin_server_api_t>;

// Template explicit instantiation.
template class drtm::polymorphic_allocator<void*>;
// Define a type alias.
using backend_allocator_t = class drtm::polymorphic_allocator<void*>;

#else

using backend_allocator_t = class std::allocator<void*>;

#endif

// ---------------------------------------------------------------------------

// Template explicit instantiation.
template class drtm::metadata<backend_t>;
// Define a type alias.
using metadata_type = class drtm::metadata<backend_t>;

// Template explicit instantiation.
template class drtm::thread<backend_t, backend_allocator_t>;
// Define a type alias.
using thread_type = class drtm::thread<backend_t, backend_allocator_t>;

// Template explicit instantiation.
template class drtm::threads<backend_t, backend_allocator_t>;
// Define a type alias.
using threads_type = class drtm::threads<backend_t, backend_allocator_t>;

// Template explicit instantiation.
template class drtm::run_time_data<backend_t, backend_allocator_t>;
// Define a type alias.
using rtd_type = class drtm::run_time_data<backend_t, backend_allocator_t>;

// Template explicit instantiation.
template class drtm::frontend<backend_t, backend_allocator_t>;
// Define a type alias.
using frontend_t = class drtm::frontend<backend_t, backend_allocator_t>;

// ---------------------------------------------------------------------------
// Local data structures.

static rtos_plugin_symbols_t symbols[] =
  {
    { DRTM_SYMBOL_NAME, 0, 0 },
    { } /* End of array. */
  /**/
  };

static struct
{
  backend_t* backend;
#if defined(USE_CUSTOM_ALLOCATOR)
  backend_memory_resource_t* memory_resource;
#endif
  backend_allocator_t* allocator;

  metadata_type* metadata;
  threads_type* threads;
  rtd_type* rtd;
  frontend_t* frontend;
} drtm_;

// ---------------------------------------------------------------------------
// SEGGER plug-in API implementation.
// It basically forwards calls to the DRTM library.

EXPORT int
RTOS_Init (const rtos_plugin_server_api_t* api, uint32_t core)
{
  api->output ("µOS++ IIIe thread-aware debugging plug-in\n");

  if ((core != JLINK_CORE_CORTEX_M0) && (core != JLINK_CORE_CORTEX_M1)
      && (core != JLINK_CORE_CORTEX_M3) && (core != JLINK_CORE_CORTEX_M4)
      && (core != JLINK_CORE_CORTEX_M7))
    {
#if defined(DEBUG_)
      api->output_error ("Non Cortex-M environment, plug-in disabled.\n");
#endif /* defined(DEBUG) */

      return 0;
    }

  // Allocate space for the DRTM backend object instance.
  drtm_.backend = (backend_t*) api->allocate (sizeof(backend_t));

  // Construct the already allocated DRTM backend object instance.
  new (drtm_.backend) backend_t
    { api, symbols };

#if defined(USE_CUSTOM_ALLOCATOR)

  // Allocate space for the DRTM memory resource object instance.
  drtm_.memory_resource = (backend_memory_resource_t*) api->allocate (
      sizeof(backend_memory_resource_t));

  // Construct the already allocated DRTM resource object object instance.
  new (drtm_.memory_resource) backend_memory_resource_t
    { api};

  // Allocate space for the DRTM allocator object instance.
  drtm_.allocator = (backend_allocator_t*) api->allocate (
      sizeof(backend_allocator_t));

  // Construct the already allocated DRTM allocator object instance.
  new (drtm_.allocator) backend_allocator_t
    { drtm_.memory_resource};

#else

  // Allocate space for the DRTM allocator object instance.
  drtm_.allocator = (backend_allocator_t*) api->allocate (
      sizeof(backend_allocator_t));

  // Construct the already allocated DRTM allocator object instance.
  new (drtm_.allocator) backend_allocator_t
    { };

#endif

  // --------------------------------------------------------------------------

  // Allocate space for the DRTM frontend object instance.
  drtm_.frontend = (frontend_t*) api->allocate (sizeof(frontend_t));

  // Construct the already allocated DRTM frontend object instance.
  new (drtm_.frontend) frontend_t
    { *drtm_.backend, *drtm_.allocator };

  return 1;
}

EXPORT uint32_t
RTOS_GetVersion ()
{
  return PLUGIN_API_VERSION;
}

EXPORT rtos_plugin_symbols_t *
RTOS_GetSymbols ()
{
  return symbols;
}

EXPORT uint32_t
RTOS_GetNumThreads ()
{
  assert(drtm_.frontend != nullptr);
  return drtm_.frontend->get_threads_count ();
}

EXPORT uint32_t
RTOS_GetThreadId (uint32_t index)
{
  assert(drtm_.frontend != nullptr);
  return drtm_.frontend->get_thread_id (index);
}

EXPORT uint32_t
RTOS_GetCurrentThreadId ()
{
  assert(drtm_.frontend != nullptr);
  return drtm_.frontend->get_current_thread_id ();
}

EXPORT int
RTOS_GetThreadDisplay (char* out_description, uint32_t thread_id)
{
  assert(drtm_.frontend != nullptr);
  return drtm_.frontend->get_thread_description (thread_id, out_description);
}

EXPORT int
RTOS_GetThreadReg (char* out_hex_value, uint32_t reg_index, uint32_t thread_id)
{
  assert(drtm_.frontend != nullptr);
  return drtm_.frontend->get_thread_register (thread_id, reg_index,
                                              out_hex_value);
}

EXPORT int
RTOS_GetThreadRegList (char* out_hex_values, uint32_t thread_id)
{
  assert(drtm_.frontend != nullptr);
  return drtm_.frontend->get_thread_registers (thread_id, out_hex_values);
}

EXPORT int
RTOS_SetThreadReg (char* hex_value, uint32_t reg_index, uint32_t thread_id)
{
  assert(drtm_.frontend != nullptr);
  return drtm_.frontend->set_thread_register (thread_id, reg_index, hex_value);
}

EXPORT int
RTOS_SetThreadRegList (char* hex_values, uint32_t thread_id)
{
  assert(drtm_.frontend != nullptr);
  return drtm_.frontend->set_thread_registers (thread_id, hex_values);
}

EXPORT int
RTOS_UpdateThreads ()
{
  assert(drtm_.frontend != nullptr);
  return drtm_.frontend->update_thread_list ();
}

// ---------------------------------------------------------------------------
