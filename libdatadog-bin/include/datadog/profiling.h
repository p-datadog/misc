// Copyright 2021-Present Datadog, Inc. https://www.datadoghq.com/
// SPDX-License-Identifier: Apache-2.0


#ifndef DDOG_PROFILING_H
#define DDOG_PROFILING_H

#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "common.h"

typedef enum ddog_prof_DemangleOptions {
  DDOG_PROF_DEMANGLE_OPTIONS_COMPLETE,
  DDOG_PROF_DEMANGLE_OPTIONS_NAME_ONLY,
} ddog_prof_DemangleOptions;

/**
 * This enum represents operations a profiler might be engaged in.
 * The idea is that if a crash consistently occurs while a particular operation
 * is ongoing, its likely related.
 *
 * In the future, we might also track wall-clock time of operations
 * (or some statistical sampling thereof) using the same enum.
 *
 * NOTE: This enum is known to be non-exhaustive.  Feel free to add new types
 *       as needed.
 */
typedef enum ddog_prof_ProfilingOpTypes {
  DDOG_PROF_PROFILING_OP_TYPES_NOT_PROFILING = 0,
  DDOG_PROF_PROFILING_OP_TYPES_COLLECTING_SAMPLE,
  DDOG_PROF_PROFILING_OP_TYPES_UNWINDING,
  DDOG_PROF_PROFILING_OP_TYPES_SERIALIZING,
  /**
   * Dummy value to allow easier iteration
   */
  DDOG_PROF_PROFILING_OP_TYPES_SIZE,
} ddog_prof_ProfilingOpTypes;

/**
 * Stacktrace collection occurs in the context of a crashing process.
 * If the stack is sufficiently corruputed, it is possible (but unlikely),
 * for stack trace collection itself to crash.
 * We recommend fully enabling stacktrace collection, but having an environment
 * variable to allow downgrading the collector.
 */
typedef enum ddog_prof_StacktraceCollection {
  /**
   * Stacktrace collection occurs in the
   */
  DDOG_PROF_STACKTRACE_COLLECTION_DISABLED,
  DDOG_PROF_STACKTRACE_COLLECTION_WITHOUT_SYMBOLS,
  DDOG_PROF_STACKTRACE_COLLECTION_ENABLED_WITH_INPROCESS_SYMBOLS,
  DDOG_PROF_STACKTRACE_COLLECTION_ENABLED_WITH_SYMBOLS_IN_RECEIVER,
} ddog_prof_StacktraceCollection;

typedef struct ddog_CancellationToken ddog_CancellationToken;

typedef struct ddog_prof_Exporter ddog_prof_Exporter;

typedef struct ddog_prof_ProfiledEndpointsStats ddog_prof_ProfiledEndpointsStats;

typedef struct ddog_prof_Exporter_Request ddog_prof_Exporter_Request;

typedef struct ddog_Tag ddog_Tag;

/**
 * Holds the raw parts of a Rust Vec; it should only be created from Rust,
 * never from C.
 */
typedef struct ddog_Vec_U8 {
  const uint8_t *ptr;
  uintptr_t len;
  uintptr_t capacity;
} ddog_Vec_U8;

/**
 * Please treat this as opaque; do not reach into it, and especially don't
 * write into it! The most relevant APIs are:
 * * `ddog_Error_message`, to get the message as a slice.
 * * `ddog_Error_drop`.
 */
typedef struct ddog_Error {
  /**
   * This is a String stuffed into the vec.
   */
  struct ddog_Vec_U8 message;
} ddog_Error;

/**
 * A generic result type for when a crashtracking operation may fail,
 * but there's nothing to return in the case of success.
 */
typedef enum ddog_prof_CrashtrackerResult_Tag {
  DDOG_PROF_CRASHTRACKER_RESULT_OK,
  DDOG_PROF_CRASHTRACKER_RESULT_ERR,
} ddog_prof_CrashtrackerResult_Tag;

typedef struct ddog_prof_CrashtrackerResult {
  ddog_prof_CrashtrackerResult_Tag tag;
  union {
    struct {
      /**
       * Do not use the value of Ok. This value only exists to overcome
       * Rust -> C code generation.
       */
      bool ok;
    };
    struct {
      struct ddog_Error err;
    };
  };
} ddog_prof_CrashtrackerResult;

typedef struct ddog_Slice_CChar {
  /**
   * Must be non-null and suitably aligned for the underlying type.
   */
  const char *ptr;
  /**
   * The number of elements (not bytes) that `.ptr` points to. Must be less
   * than or equal to [isize::MAX].
   */
  uintptr_t len;
} ddog_Slice_CChar;

/**
 * Use to represent strings -- should be valid UTF-8.
 */
typedef struct ddog_Slice_CChar ddog_CharSlice;

typedef struct ddog_prof_Slice_CharSlice {
  /**
   * Must be non-null and suitably aligned for the underlying type.
   */
  const ddog_CharSlice *ptr;
  /**
   * The number of elements (not bytes) that `.ptr` points to. Must be less
   * than or equal to [isize::MAX].
   */
  uintptr_t len;
} ddog_prof_Slice_CharSlice;

typedef enum ddog_prof_Endpoint_Tag {
  DDOG_PROF_ENDPOINT_AGENT,
  DDOG_PROF_ENDPOINT_AGENTLESS,
  DDOG_PROF_ENDPOINT_FILE,
} ddog_prof_Endpoint_Tag;

typedef struct ddog_prof_Endpoint_ddog_prof_Agentless_Body {
  ddog_CharSlice _0;
  ddog_CharSlice _1;
} ddog_prof_Endpoint_ddog_prof_Agentless_Body;

typedef struct ddog_prof_Endpoint {
  ddog_prof_Endpoint_Tag tag;
  union {
    struct {
      ddog_CharSlice agent;
    };
    ddog_prof_Endpoint_ddog_prof_Agentless_Body AGENTLESS;
    struct {
      ddog_CharSlice file;
    };
  };
} ddog_prof_Endpoint;

typedef struct ddog_prof_CrashtrackerConfiguration {
  struct ddog_prof_Slice_CharSlice additional_files;
  bool create_alt_stack;
  /**
   * The endpoint to send the crash report to (can be a file://)
   *
   * If ProfilingEndpoint is left to a zero value (enum value for Agent + empty charslice),
   * the crashtracker will infer the agent host from env variables.
   */
  struct ddog_prof_Endpoint endpoint;
  enum ddog_prof_StacktraceCollection resolve_frames;
  uint64_t timeout_secs;
  bool wait_for_receiver;
} ddog_prof_CrashtrackerConfiguration;

typedef struct ddog_prof_EnvVar {
  ddog_CharSlice key;
  ddog_CharSlice val;
} ddog_prof_EnvVar;

typedef struct ddog_prof_Slice_EnvVar {
  /**
   * Must be non-null and suitably aligned for the underlying type.
   */
  const struct ddog_prof_EnvVar *ptr;
  /**
   * The number of elements (not bytes) that `.ptr` points to. Must be less
   * than or equal to [isize::MAX].
   */
  uintptr_t len;
} ddog_prof_Slice_EnvVar;

typedef struct ddog_prof_CrashtrackerReceiverConfig {
  struct ddog_prof_Slice_CharSlice args;
  struct ddog_prof_Slice_EnvVar env;
  ddog_CharSlice path_to_receiver_binary;
  /**
   * Optional filename to forward stderr to (useful for logging/debugging)
   */
  ddog_CharSlice optional_stderr_filename;
  /**
   * Optional filename to forward stdout to (useful for logging/debugging)
   */
  ddog_CharSlice optional_stdout_filename;
} ddog_prof_CrashtrackerReceiverConfig;

/**
 * Holds the raw parts of a Rust Vec; it should only be created from Rust,
 * never from C.
 */
typedef struct ddog_Vec_Tag {
  const struct ddog_Tag *ptr;
  uintptr_t len;
  uintptr_t capacity;
} ddog_Vec_Tag;

typedef struct ddog_prof_CrashtrackerMetadata {
  ddog_CharSlice profiling_library_name;
  ddog_CharSlice profiling_library_version;
  ddog_CharSlice family;
  /**
   * Should include "service", "environment", etc
   */
  const struct ddog_Vec_Tag *tags;
} ddog_prof_CrashtrackerMetadata;

/**
 * Represents a CrashInfo. Do not access its member for any reason, only use
 * the C API functions on this struct.
 */
typedef struct ddog_prof_CrashInfo {
  struct ddog_prof_CrashInfo *inner;
} ddog_prof_CrashInfo;

/**
 * Returned by [ddog_prof_Profile_new].
 */
typedef enum ddog_prof_CrashInfoNewResult_Tag {
  DDOG_PROF_CRASH_INFO_NEW_RESULT_OK,
  DDOG_PROF_CRASH_INFO_NEW_RESULT_ERR,
} ddog_prof_CrashInfoNewResult_Tag;

typedef struct ddog_prof_CrashInfoNewResult {
  ddog_prof_CrashInfoNewResult_Tag tag;
  union {
    struct {
      struct ddog_prof_CrashInfo ok;
    };
    struct {
      struct ddog_Error err;
    };
  };
} ddog_prof_CrashInfoNewResult;

typedef struct ddog_prof_SigInfo {
  uint64_t signum;
  ddog_CharSlice signame;
} ddog_prof_SigInfo;

typedef enum ddog_prof_Option_U32_Tag {
  DDOG_PROF_OPTION_U32_SOME_U32,
  DDOG_PROF_OPTION_U32_NONE_U32,
} ddog_prof_Option_U32_Tag;

typedef struct ddog_prof_Option_U32 {
  ddog_prof_Option_U32_Tag tag;
  union {
    struct {
      uint32_t some;
    };
  };
} ddog_prof_Option_U32;

typedef struct ddog_prof_StackFrameNames {
  struct ddog_prof_Option_U32 colno;
  ddog_CharSlice filename;
  struct ddog_prof_Option_U32 lineno;
  ddog_CharSlice name;
} ddog_prof_StackFrameNames;

typedef struct ddog_prof_Slice_StackFrameNames {
  /**
   * Must be non-null and suitably aligned for the underlying type.
   */
  const struct ddog_prof_StackFrameNames *ptr;
  /**
   * The number of elements (not bytes) that `.ptr` points to. Must be less
   * than or equal to [isize::MAX].
   */
  uintptr_t len;
} ddog_prof_Slice_StackFrameNames;

typedef struct ddog_prof_StackFrame {
  uintptr_t ip;
  uintptr_t module_base_address;
  struct ddog_prof_Slice_StackFrameNames names;
  uintptr_t sp;
  uintptr_t symbol_address;
} ddog_prof_StackFrame;

typedef struct ddog_prof_Slice_StackFrame {
  /**
   * Must be non-null and suitably aligned for the underlying type.
   */
  const struct ddog_prof_StackFrame *ptr;
  /**
   * The number of elements (not bytes) that `.ptr` points to. Must be less
   * than or equal to [isize::MAX].
   */
  uintptr_t len;
} ddog_prof_Slice_StackFrame;

/**
 * A wrapper for returning owned strings from FFI
 */
typedef struct ddog_prof_StringWrapper {
  /**
   * This is a String stuffed into the vec.
   */
  struct ddog_Vec_U8 message;
} ddog_prof_StringWrapper;

/**
 * Returned by [ddog_prof_Profile_new].
 */
typedef enum ddog_prof_StringWrapperResult_Tag {
  DDOG_PROF_STRING_WRAPPER_RESULT_OK,
  DDOG_PROF_STRING_WRAPPER_RESULT_ERR,
} ddog_prof_StringWrapperResult_Tag;

typedef struct ddog_prof_StringWrapperResult {
  ddog_prof_StringWrapperResult_Tag tag;
  union {
    struct {
      struct ddog_prof_StringWrapper ok;
    };
    struct {
      struct ddog_Error err;
    };
  };
} ddog_prof_StringWrapperResult;

typedef struct ddog_Slice_U8 {
  /**
   * Must be non-null and suitably aligned for the underlying type.
   */
  const uint8_t *ptr;
  /**
   * The number of elements (not bytes) that `.ptr` points to. Must be less
   * than or equal to [isize::MAX].
   */
  uintptr_t len;
} ddog_Slice_U8;

/**
 * Use to represent bytes -- does not need to be valid UTF-8.
 */
typedef struct ddog_Slice_U8 ddog_ByteSlice;

typedef struct ddog_prof_Exporter_File {
  ddog_CharSlice name;
  ddog_ByteSlice file;
} ddog_prof_Exporter_File;

typedef struct ddog_prof_Exporter_Slice_File {
  /**
   * Must be non-null and suitably aligned for the underlying type.
   */
  const struct ddog_prof_Exporter_File *ptr;
  /**
   * The number of elements (not bytes) that `.ptr` points to. Must be less
   * than or equal to [isize::MAX].
   */
  uintptr_t len;
} ddog_prof_Exporter_Slice_File;

typedef enum ddog_prof_Exporter_NewResult_Tag {
  DDOG_PROF_EXPORTER_NEW_RESULT_OK,
  DDOG_PROF_EXPORTER_NEW_RESULT_ERR,
} ddog_prof_Exporter_NewResult_Tag;

typedef struct ddog_prof_Exporter_NewResult {
  ddog_prof_Exporter_NewResult_Tag tag;
  union {
    struct {
      struct ddog_prof_Exporter *ok;
    };
    struct {
      struct ddog_Error err;
    };
  };
} ddog_prof_Exporter_NewResult;

typedef enum ddog_prof_Exporter_Request_BuildResult_Tag {
  DDOG_PROF_EXPORTER_REQUEST_BUILD_RESULT_OK,
  DDOG_PROF_EXPORTER_REQUEST_BUILD_RESULT_ERR,
} ddog_prof_Exporter_Request_BuildResult_Tag;

typedef struct ddog_prof_Exporter_Request_BuildResult {
  ddog_prof_Exporter_Request_BuildResult_Tag tag;
  union {
    struct {
      struct ddog_prof_Exporter_Request *ok;
    };
    struct {
      struct ddog_Error err;
    };
  };
} ddog_prof_Exporter_Request_BuildResult;

/**
 * Represents time since the Unix Epoch in seconds plus nanoseconds.
 */
typedef struct ddog_Timespec {
  int64_t seconds;
  uint32_t nanoseconds;
} ddog_Timespec;

typedef struct ddog_HttpStatus {
  uint16_t code;
} ddog_HttpStatus;

typedef enum ddog_prof_Exporter_SendResult_Tag {
  DDOG_PROF_EXPORTER_SEND_RESULT_HTTP_RESPONSE,
  DDOG_PROF_EXPORTER_SEND_RESULT_ERR,
} ddog_prof_Exporter_SendResult_Tag;

typedef struct ddog_prof_Exporter_SendResult {
  ddog_prof_Exporter_SendResult_Tag tag;
  union {
    struct {
      struct ddog_HttpStatus http_response;
    };
    struct {
      struct ddog_Error err;
    };
  };
} ddog_prof_Exporter_SendResult;

/**
 * Represents a profile. Do not access its member for any reason, only use
 * the C API functions on this struct.
 */
typedef struct ddog_prof_Profile {
  struct ddog_prof_Profile *inner;
} ddog_prof_Profile;

/**
 * Returned by [ddog_prof_Profile_new].
 */
typedef enum ddog_prof_Profile_NewResult_Tag {
  DDOG_PROF_PROFILE_NEW_RESULT_OK,
  DDOG_PROF_PROFILE_NEW_RESULT_ERR,
} ddog_prof_Profile_NewResult_Tag;

typedef struct ddog_prof_Profile_NewResult {
  ddog_prof_Profile_NewResult_Tag tag;
  union {
    struct {
      struct ddog_prof_Profile ok;
    };
    struct {
      struct ddog_Error err;
    };
  };
} ddog_prof_Profile_NewResult;

typedef struct ddog_prof_ValueType {
  ddog_CharSlice type_;
  ddog_CharSlice unit;
} ddog_prof_ValueType;

typedef struct ddog_prof_Slice_ValueType {
  /**
   * Must be non-null and suitably aligned for the underlying type.
   */
  const struct ddog_prof_ValueType *ptr;
  /**
   * The number of elements (not bytes) that `.ptr` points to. Must be less
   * than or equal to [isize::MAX].
   */
  uintptr_t len;
} ddog_prof_Slice_ValueType;

typedef struct ddog_prof_Period {
  struct ddog_prof_ValueType type_;
  int64_t value;
} ddog_prof_Period;

/**
 * A generic result type for when a profiling operation may fail, but there's
 * nothing to return in the case of success.
 */
typedef enum ddog_prof_Profile_Result_Tag {
  DDOG_PROF_PROFILE_RESULT_OK,
  DDOG_PROF_PROFILE_RESULT_ERR,
} ddog_prof_Profile_Result_Tag;

typedef struct ddog_prof_Profile_Result {
  ddog_prof_Profile_Result_Tag tag;
  union {
    struct {
      /**
       * Do not use the value of Ok. This value only exists to overcome
       * Rust -> C code generation.
       */
      bool ok;
    };
    struct {
      struct ddog_Error err;
    };
  };
} ddog_prof_Profile_Result;

typedef struct ddog_prof_Mapping {
  /**
   * Address at which the binary (or DLL) is loaded into memory.
   */
  uint64_t memory_start;
  /**
   * The limit of the address range occupied by this mapping.
   */
  uint64_t memory_limit;
  /**
   * Offset in the binary that corresponds to the first mapped address.
   */
  uint64_t file_offset;
  /**
   * The object this entry is loaded from.  This can be a filename on
   * disk for the main binary and shared libraries, or virtual
   * abstractions like "[vdso]".
   */
  ddog_CharSlice filename;
  /**
   * A string that uniquely identifies a particular program version
   * with high probability. E.g., for binaries generated by GNU tools,
   * it could be the contents of the .note.gnu.build-id field.
   */
  ddog_CharSlice build_id;
} ddog_prof_Mapping;

typedef struct ddog_prof_Function {
  /**
   * Name of the function, in human-readable form if available.
   */
  ddog_CharSlice name;
  /**
   * Name of the function, as identified by the system.
   * For instance, it can be a C++ mangled name.
   */
  ddog_CharSlice system_name;
  /**
   * Source file containing the function.
   */
  ddog_CharSlice filename;
  /**
   * Line number in source file.
   */
  int64_t start_line;
} ddog_prof_Function;

typedef struct ddog_prof_Location {
  /**
   * todo: how to handle unknown mapping?
   */
  struct ddog_prof_Mapping mapping;
  struct ddog_prof_Function function;
  /**
   * The instruction address for this location, if available.  It
   * should be within [Mapping.memory_start...Mapping.memory_limit]
   * for the corresponding mapping. A non-leaf address may be in the
   * middle of a call instruction. It is up to display tools to find
   * the beginning of the instruction if necessary.
   */
  uint64_t address;
  int64_t line;
} ddog_prof_Location;

typedef struct ddog_prof_Slice_Location {
  /**
   * Must be non-null and suitably aligned for the underlying type.
   */
  const struct ddog_prof_Location *ptr;
  /**
   * The number of elements (not bytes) that `.ptr` points to. Must be less
   * than or equal to [isize::MAX].
   */
  uintptr_t len;
} ddog_prof_Slice_Location;

typedef struct ddog_Slice_I64 {
  /**
   * Must be non-null and suitably aligned for the underlying type.
   */
  const int64_t *ptr;
  /**
   * The number of elements (not bytes) that `.ptr` points to. Must be less
   * than or equal to [isize::MAX].
   */
  uintptr_t len;
} ddog_Slice_I64;

typedef struct ddog_prof_Label {
  ddog_CharSlice key;
  /**
   * At most one of the following must be present
   */
  ddog_CharSlice str;
  int64_t num;
  /**
   * Should only be present when num is present.
   * Specifies the units of num.
   * Use arbitrary string (for example, "requests") as a custom count unit.
   * If no unit is specified, consumer may apply heuristic to deduce the unit.
   * Consumers may also  interpret units like "bytes" and "kilobytes" as memory
   * units and units like "seconds" and "nanoseconds" as time units,
   * and apply appropriate unit conversions to these.
   */
  ddog_CharSlice num_unit;
} ddog_prof_Label;

typedef struct ddog_prof_Slice_Label {
  /**
   * Must be non-null and suitably aligned for the underlying type.
   */
  const struct ddog_prof_Label *ptr;
  /**
   * The number of elements (not bytes) that `.ptr` points to. Must be less
   * than or equal to [isize::MAX].
   */
  uintptr_t len;
} ddog_prof_Slice_Label;

typedef struct ddog_prof_Sample {
  /**
   * The leaf is at locations[0].
   */
  struct ddog_prof_Slice_Location locations;
  /**
   * The type and unit of each value is defined by the corresponding
   * entry in Profile.sample_type. All samples must have the same
   * number of values, the same as the length of Profile.sample_type.
   * When aggregating multiple samples into a single sample, the
   * result has a list of values that is the element-wise sum of the
   * lists of the originals.
   */
  struct ddog_Slice_I64 values;
  /**
   * label includes additional context for this sample. It can include
   * things like a thread id, allocation size, etc
   */
  struct ddog_prof_Slice_Label labels;
} ddog_prof_Sample;

typedef struct ddog_prof_Slice_Usize {
  /**
   * Must be non-null and suitably aligned for the underlying type.
   */
  const uintptr_t *ptr;
  /**
   * The number of elements (not bytes) that `.ptr` points to. Must be less
   * than or equal to [isize::MAX].
   */
  uintptr_t len;
} ddog_prof_Slice_Usize;

typedef struct ddog_prof_EncodedProfile {
  struct ddog_Timespec start;
  struct ddog_Timespec end;
  struct ddog_Vec_U8 buffer;
  struct ddog_prof_ProfiledEndpointsStats *endpoints_stats;
} ddog_prof_EncodedProfile;

typedef enum ddog_prof_Profile_SerializeResult_Tag {
  DDOG_PROF_PROFILE_SERIALIZE_RESULT_OK,
  DDOG_PROF_PROFILE_SERIALIZE_RESULT_ERR,
} ddog_prof_Profile_SerializeResult_Tag;

typedef struct ddog_prof_Profile_SerializeResult {
  ddog_prof_Profile_SerializeResult_Tag tag;
  union {
    struct {
      struct ddog_prof_EncodedProfile ok;
    };
    struct {
      struct ddog_Error err;
    };
  };
} ddog_prof_Profile_SerializeResult;

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/**
 * Cleans up after the crash-tracker:
 * Unregister the crash handler, restore the previous handler (if any), and
 * shut down the receiver.  Note that the use of this function is optional:
 * the receiver will automatically shutdown when the pipe is closed on program
 * exit.
 *
 * # Preconditions
 *     This function assumes that the crash-tracker has previously been
 *     initialized.
 * # Safety
 *     Crash-tracking functions are not reentrant.
 *     No other crash-handler functions should be called concurrently.
 * # Atomicity
 *     This function is not atomic. A crash during its execution may lead to
 *     unexpected crash-handling behaviour.
 */
DDOG_CHECK_RETURN struct ddog_prof_CrashtrackerResult ddog_prof_Crashtracker_shutdown(void);

/**
 * Reinitialize the crash-tracking infrastructure after a fork.
 * This should be one of the first things done after a fork, to minimize the
 * chance that a crash occurs between the fork, and this call.
 * In particular, reset the counters that track the profiler state machine,
 * and start a new receiver to collect data from this fork.
 * NOTE: An alternative design would be to have a 1:many sidecar listening on a
 * socket instead of 1:1 receiver listening on a pipe, but the only real
 * advantage would be to have fewer processes in `ps -a`.
 *
 * # Preconditions
 *     This function assumes that the crash-tracker has previously been
 *     initialized.
 * # Safety
 *     Crash-tracking functions are not reentrant.
 *     No other crash-handler functions should be called concurrently.
 * # Atomicity
 *     This function is not atomic. A crash during its execution may lead to
 *     unexpected crash-handling behaviour.
 */
DDOG_CHECK_RETURN
struct ddog_prof_CrashtrackerResult ddog_prof_Crashtracker_update_on_fork(struct ddog_prof_CrashtrackerConfiguration config,
                                                                          struct ddog_prof_CrashtrackerReceiverConfig receiver_config,
                                                                          struct ddog_prof_CrashtrackerMetadata metadata);

/**
 * Receives data from a crash collector via a pipe on `stdin`, formats it into
 * `CrashInfo` json, and emits it to the endpoint/file defined in `config`.
 *
 * At a high-level, this exists because doing anything in a
 * signal handler is dangerous, so we fork a sidecar to do the stuff we aren't
 * allowed to do in the handler.
 *
 * See comments in [profiling/crashtracker/mod.rs] for a full architecture
 * description.
 * # Safety
 * No safety concerns
 */
DDOG_CHECK_RETURN
struct ddog_prof_CrashtrackerResult ddog_prof_Crashtracker_receiver_entry_point_stdin(void);

/**
 * Receives data from a crash collector via a pipe on `stdin`, formats it into
 * `CrashInfo` json, and emits it to the endpoint/file defined in `config`.
 *
 * At a high-level, this exists because doing anything in a
 * signal handler is dangerous, so we fork a sidecar to do the stuff we aren't
 * allowed to do in the handler.
 *
 * See comments in [profiling/crashtracker/mod.rs] for a full architecture
 * description.
 * # Safety
 * No safety concerns
 */
DDOG_CHECK_RETURN
struct ddog_prof_CrashtrackerResult ddog_prof_Crashtracker_receiver_entry_point_unix_socket(ddog_CharSlice socket_path);

/**
 * Initialize the crash-tracking infrastructure.
 *
 * # Preconditions
 *     None.
 * # Safety
 *     Crash-tracking functions are not reentrant.
 *     No other crash-handler functions should be called concurrently.
 * # Atomicity
 *     This function is not atomic. A crash during its execution may lead to
 *     unexpected crash-handling behaviour.
 */
DDOG_CHECK_RETURN
struct ddog_prof_CrashtrackerResult ddog_prof_Crashtracker_init_with_receiver(struct ddog_prof_CrashtrackerConfiguration config,
                                                                              struct ddog_prof_CrashtrackerReceiverConfig receiver_config,
                                                                              struct ddog_prof_CrashtrackerMetadata metadata);

/**
 * Resets all counters to 0.
 * Expected to be used after a fork, to reset the counters on the child
 * ATOMICITY:
 *     This is NOT ATOMIC.
 *     Should only be used when no conflicting updates can occur,
 *     e.g. after a fork but before profiling ops start on the child.
 * # Safety
 * No safety concerns.
 */
DDOG_CHECK_RETURN struct ddog_prof_CrashtrackerResult ddog_prof_Crashtracker_reset_counters(void);

/**
 * Atomically increments the count associated with `op`.
 * Useful for tracking what operations were occuring when a crash occurred.
 *
 * # Safety
 * No safety concerns.
 */
DDOG_CHECK_RETURN
struct ddog_prof_CrashtrackerResult ddog_prof_Crashtracker_begin_profiling_op(enum ddog_prof_ProfilingOpTypes op);

/**
 * Atomically decrements the count associated with `op`.
 * Useful for tracking what operations were occuring when a crash occurred.
 *
 * # Safety
 * No safety concerns.
 */
DDOG_CHECK_RETURN
struct ddog_prof_CrashtrackerResult ddog_prof_Crashtracker_end_profiling_op(enum ddog_prof_ProfilingOpTypes op);

/**
 * Create a new crashinfo, and returns an opaque reference to it.
 * # Safety
 * No safety issues.
 */
DDOG_CHECK_RETURN struct ddog_prof_CrashInfoNewResult ddog_crashinfo_new(void);

/**
 * # Safety
 * The `crash_info` can be null, but if non-null it must point to a CrashInfo
 * made by this module, which has not previously been dropped.
 */
void ddog_crashinfo_drop(struct ddog_prof_CrashInfo *crashinfo);

/**
 * Adds a "counter" variable, with the given value.  Useful for determining if
 * "interesting" operations were occurring when the crash did.
 *
 * # Safety
 * `crashinfo` must be a valid pointer to a `CrashInfo` object.
 * `name` should be a valid reference to a utf8 encoded String.
 * The string is copied into the crashinfo, so it does not need to outlive this
 * call.
 */
DDOG_CHECK_RETURN
struct ddog_prof_CrashtrackerResult ddog_crashinfo_add_counter(struct ddog_prof_CrashInfo *crashinfo,
                                                               ddog_CharSlice name,
                                                               int64_t val);

/**
 * Adds the contents of "file" to the crashinfo
 *
 * # Safety
 * `crashinfo` must be a valid pointer to a `CrashInfo` object.
 * `name` should be a valid reference to a utf8 encoded String.
 * The string is copied into the crashinfo, so it does not need to outlive this
 * call.
 */
DDOG_CHECK_RETURN
struct ddog_prof_CrashtrackerResult ddog_crashinfo_add_file(struct ddog_prof_CrashInfo *crashinfo,
                                                            ddog_CharSlice name);

/**
 * Adds the tag with given "key" and "value" to the crashinfo
 *
 * # Safety
 * `crashinfo` must be a valid pointer to a `CrashInfo` object.
 * `key` should be a valid reference to a utf8 encoded String.
 * `value` should be a valid reference to a utf8 encoded String.
 * The string is copied into the crashinfo, so it does not need to outlive this
 * call.
 */
DDOG_CHECK_RETURN
struct ddog_prof_CrashtrackerResult ddog_crashinfo_add_tag(struct ddog_prof_CrashInfo *crashinfo,
                                                           ddog_CharSlice key,
                                                           ddog_CharSlice value);

/**
 * Sets the crashinfo metadata
 *
 * # Safety
 * `crashinfo` must be a valid pointer to a `CrashInfo` object.
 * All references inside `metadata` must be valid.
 * Strings are copied into the crashinfo, and do not need to outlive this call.
 */
DDOG_CHECK_RETURN
struct ddog_prof_CrashtrackerResult ddog_crashinfo_set_metadata(struct ddog_prof_CrashInfo *crashinfo,
                                                                struct ddog_prof_CrashtrackerMetadata metadata);

/**
 * Sets the crashinfo siginfo
 *
 * # Safety
 * `crashinfo` must be a valid pointer to a `CrashInfo` object.
 * All references inside `metadata` must be valid.
 * Strings are copied into the crashinfo, and do not need to outlive this call.
 */
DDOG_CHECK_RETURN
struct ddog_prof_CrashtrackerResult ddog_crashinfo_set_siginfo(struct ddog_prof_CrashInfo *crashinfo,
                                                               struct ddog_prof_SigInfo siginfo);

/**
 * If `thread_id` is empty, sets `stacktrace` as the default stacktrace.
 * Otherwise, adds an additional stacktrace with id "thread_id".
 *
 * # Safety
 * `crashinfo` must be a valid pointer to a `CrashInfo` object.
 * All references inside `stacktraces` must be valid.
 * Strings are copied into the crashinfo, and do not need to outlive this call.
 */
DDOG_CHECK_RETURN
struct ddog_prof_CrashtrackerResult ddog_crashinfo_set_stacktrace(struct ddog_prof_CrashInfo *crashinfo,
                                                                  ddog_CharSlice thread_id,
                                                                  struct ddog_prof_Slice_StackFrame stacktrace);

/**
 * Sets the timestamp to the given unix timestamp
 *
 * # Safety
 * `crashinfo` must be a valid pointer to a `CrashInfo` object.
 */
DDOG_CHECK_RETURN
struct ddog_prof_CrashtrackerResult ddog_crashinfo_set_timestamp(struct ddog_prof_CrashInfo *crashinfo,
                                                                 int64_t secs,
                                                                 uint32_t nsecs);

/**
 * Sets the timestamp to the current time
 *
 * # Safety
 * `crashinfo` must be a valid pointer to a `CrashInfo` object.
 */
DDOG_CHECK_RETURN
struct ddog_prof_CrashtrackerResult ddog_crashinfo_set_timestamp_to_now(struct ddog_prof_CrashInfo *crashinfo);

/**
 * Exports `crashinfo` to the backend at `endpoint`
 * Note that we support the "file://" endpoint for local file output.
 * # Safety
 * `crashinfo` must be a valid pointer to a `CrashInfo` object.
 */
DDOG_CHECK_RETURN
struct ddog_prof_CrashtrackerResult ddog_crashinfo_upload_to_endpoint(struct ddog_prof_CrashInfo *crashinfo,
                                                                      struct ddog_prof_CrashtrackerConfiguration config);

/**
 * Demangles the string "name".
 * If demangling fails, returns an empty string ""
 *
 * # Safety
 * `name` should be a valid reference to a utf8 encoded String.
 * The string is copied into the result, and does not need to outlive this call
 */
DDOG_CHECK_RETURN
struct ddog_prof_StringWrapperResult ddog_demangle(ddog_CharSlice name,
                                                   enum ddog_prof_DemangleOptions options);

DDOG_CHECK_RETURN struct ddog_prof_Exporter_Slice_File ddog_prof_Exporter_Slice_File_empty(void);

/**
 * Creates an endpoint that uses the agent.
 * # Arguments
 * * `base_url` - Contains a URL with scheme, host, and port e.g. "https://agent:8126/".
 */
struct ddog_prof_Endpoint ddog_prof_Endpoint_agent(ddog_CharSlice base_url);

/**
 * Creates an endpoint that uses the Datadog intake directly aka agentless.
 * # Arguments
 * * `site` - Contains a host and port e.g. "datadoghq.com".
 * * `api_key` - Contains the Datadog API key.
 */
struct ddog_prof_Endpoint ddog_prof_Endpoint_agentless(ddog_CharSlice site, ddog_CharSlice api_key);

/**
 * Creates an endpoint that writes to a file.
 * Useful for local debugging.
 * Currently only supported by the crashtracker.
 * # Arguments
 * * `filename` - Path to the output file "/tmp/file.txt".
 */
struct ddog_prof_Endpoint ddog_Endpoint_file(ddog_CharSlice filename);

/**
 * Creates a new exporter to be used to report profiling data.
 * # Arguments
 * * `profiling_library_name` - Profiling library name, usually dd-trace-something, e.g.
 *   "dd-trace-rb". See
 *   https://datadoghq.atlassian.net/wiki/spaces/PROF/pages/1538884229/Client#Header-values
 *   (Datadog internal link)
 *   for a list of common values.
 * * `profliling_library_version` - Version used when publishing the profiling library to a package
 *   manager
 * * `family` - Profile family, e.g. "ruby"
 * * `tags` - Tags to include with every profile reported by this exporter. It's also possible to
 *   include profile-specific tags, see `additional_tags` on `profile_exporter_build`.
 * * `endpoint` - Configuration for reporting data
 * # Safety
 * All pointers must refer to valid objects of the correct types.
 */
DDOG_CHECK_RETURN
struct ddog_prof_Exporter_NewResult ddog_prof_Exporter_new(ddog_CharSlice profiling_library_name,
                                                           ddog_CharSlice profiling_library_version,
                                                           ddog_CharSlice family,
                                                           const struct ddog_Vec_Tag *tags,
                                                           struct ddog_prof_Endpoint endpoint);

/**
 * # Safety
 * The `exporter` may be null, but if non-null the pointer must point to a
 * valid `ddog_prof_Exporter_Request` object made by the Rust Global
 * allocator that has not already been dropped.
 */
void ddog_prof_Exporter_drop(struct ddog_prof_Exporter *exporter);

/**
 * If successful, builds a `ddog_prof_Exporter_Request` object based on the
 * profile data supplied. If unsuccessful, it returns an error message.
 *
 * For details on the `optional_internal_metadata_json`, please reference the Datadog-internal
 * "RFC: Attaching internal metadata to pprof profiles".
 * If you use this parameter, please update the RFC with your use-case, so we can keep track of how
 * this is getting used.
 *
 * For details on the `optional_info_json`, please reference the Datadog-internal
 * "RFC: Pprof System Info Support".
 *
 * # Safety
 * The `exporter`, `optional_additional_stats`, and `optional_endpoint_stats` args should be
 * valid objects created by this module.
 * NULL is allowed for `optional_additional_tags`, `optional_endpoints_stats`,
 * `optional_internal_metadata_json` and `optional_info_json`.
 */
DDOG_CHECK_RETURN
struct ddog_prof_Exporter_Request_BuildResult ddog_prof_Exporter_Request_build(struct ddog_prof_Exporter *exporter,
                                                                               struct ddog_Timespec start,
                                                                               struct ddog_Timespec end,
                                                                               struct ddog_prof_Exporter_Slice_File files_to_compress_and_export,
                                                                               struct ddog_prof_Exporter_Slice_File files_to_export_unmodified,
                                                                               const struct ddog_Vec_Tag *optional_additional_tags,
                                                                               const struct ddog_prof_ProfiledEndpointsStats *optional_endpoints_stats,
                                                                               const ddog_CharSlice *optional_internal_metadata_json,
                                                                               const ddog_CharSlice *optional_info_json,
                                                                               uint64_t timeout_ms);

/**
 * # Safety
 * Each pointer of `request` may be null, but if non-null the inner-most
 * pointer must point to a valid `ddog_prof_Exporter_Request` object made by
 * the Rust Global allocator.
 */
void ddog_prof_Exporter_Request_drop(struct ddog_prof_Exporter_Request **request);

/**
 * Sends the request, returning the HttpStatus.
 *
 * # Arguments
 * * `exporter` - Borrows the exporter for sending the request.
 * * `request` - Takes ownership of the request, replacing it with a null pointer. This is why it
 *   takes a double-pointer, rather than a single one.
 * * `cancel` - Borrows the cancel, if any.
 *
 * # Safety
 * All non-null arguments MUST have been created by created by apis in this module.
 */
DDOG_CHECK_RETURN
struct ddog_prof_Exporter_SendResult ddog_prof_Exporter_send(struct ddog_prof_Exporter *exporter,
                                                             struct ddog_prof_Exporter_Request **request,
                                                             const struct ddog_CancellationToken *cancel);

/**
 * Can be passed as an argument to send and then be used to asynchronously cancel it from a
 * different thread.
 */
DDOG_CHECK_RETURN struct ddog_CancellationToken *ddog_CancellationToken_new(void);

/**
 * A cloned CancellationToken is connected to the CancellationToken it was created from.
 * Either the cloned or the original token can be used to cancel or provided as arguments to send.
 * The useful part is that they have independent lifetimes and can be dropped separately.
 *
 * Thus, it's possible to do something like:
 * ```c
 * cancel_t1 = ddog_CancellationToken_new();
 * cancel_t2 = ddog_CancellationToken_clone(cancel_t1);
 *
 * // On thread t1:
 *     ddog_prof_Exporter_send(..., cancel_t1);
 *     ddog_CancellationToken_drop(cancel_t1);
 *
 * // On thread t2:
 *     ddog_CancellationToken_cancel(cancel_t2);
 *     ddog_CancellationToken_drop(cancel_t2);
 * ```
 *
 * Without clone, both t1 and t2 would need to synchronize to make sure neither was using the
 * cancel before it could be dropped. With clone, there is no need for such synchronization, both
 * threads have their own cancel and should drop that cancel after they are done with it.
 *
 * # Safety
 * If the `token` is non-null, it must point to a valid object.
 */
DDOG_CHECK_RETURN
struct ddog_CancellationToken *ddog_CancellationToken_clone(const struct ddog_CancellationToken *token);

/**
 * Cancel send that is being called in another thread with the given token.
 * Note that cancellation is a terminal state; cancelling a token more than once does nothing.
 * Returns `true` if token was successfully cancelled.
 */
bool ddog_CancellationToken_cancel(const struct ddog_CancellationToken *cancel);

/**
 * # Safety
 * The `token` can be null, but non-null values must be created by the Rust
 * Global allocator and must have not been dropped already.
 */
void ddog_CancellationToken_drop(struct ddog_CancellationToken *token);

/**
 * Create a new profile with the given sample types. Must call
 * `ddog_prof_Profile_drop` when you are done with the profile.
 *
 * # Arguments
 * * `sample_types`
 * * `period` - Optional period of the profile. Passing None/null translates to zero values.
 * * `start_time` - Optional time the profile started at. Passing None/null will use the current
 *   time.
 *
 * # Safety
 * All slices must be have pointers that are suitably aligned for their type
 * and must have the correct number of elements for the slice.
 */
DDOG_CHECK_RETURN
struct ddog_prof_Profile_NewResult ddog_prof_Profile_new(struct ddog_prof_Slice_ValueType sample_types,
                                                         const struct ddog_prof_Period *period,
                                                         const struct ddog_Timespec *start_time);

/**
 * # Safety
 * The `profile` can be null, but if non-null it must point to a Profile
 * made by this module, which has not previously been dropped.
 */
void ddog_prof_Profile_drop(struct ddog_prof_Profile *profile);

/**
 * # Safety
 * The `profile` ptr must point to a valid Profile object created by this
 * module. All pointers inside the `sample` need to be valid for the duration
 * of this call.
 *
 * If successful, it returns the Ok variant.
 * On error, it holds an error message in the error variant.
 *
 * # Safety
 * The `profile` ptr must point to a valid Profile object created by this
 * module.
 * This call is _NOT_ thread-safe.
 */
DDOG_CHECK_RETURN
struct ddog_prof_Profile_Result ddog_prof_Profile_add(struct ddog_prof_Profile *profile,
                                                      struct ddog_prof_Sample sample,
                                                      int64_t timestamp);

/**
 * Associate an endpoint to a given local root span id.
 * During the serialization of the profile, an endpoint label will be added
 * to all samples that contain a matching local root span id label.
 *
 * Note: calling this API causes the "trace endpoint" and "local root span id" strings
 * to be interned, even if no matching sample is found.
 *
 * # Arguments
 * * `profile` - a reference to the profile that will contain the samples.
 * * `local_root_span_id`
 * * `endpoint` - the value of the endpoint label to add for matching samples.
 *
 * # Safety
 * The `profile` ptr must point to a valid Profile object created by this
 * module.
 * This call is _NOT_ thread-safe.
 */
DDOG_CHECK_RETURN
struct ddog_prof_Profile_Result ddog_prof_Profile_set_endpoint(struct ddog_prof_Profile *profile,
                                                               uint64_t local_root_span_id,
                                                               ddog_CharSlice endpoint);

/**
 * Count the number of times an endpoint has been seen.
 *
 * # Arguments
 * * `profile` - a reference to the profile that will contain the samples.
 * * `endpoint` - the endpoint label for which the count will be incremented
 *
 * # Safety
 * The `profile` ptr must point to a valid Profile object created by this
 * module.
 * This call is _NOT_ thread-safe.
 */
DDOG_CHECK_RETURN
struct ddog_prof_Profile_Result ddog_prof_Profile_add_endpoint_count(struct ddog_prof_Profile *profile,
                                                                     ddog_CharSlice endpoint,
                                                                     int64_t value);

/**
 * Add a poisson-based upscaling rule which will be use to adjust values and make them
 * closer to reality.
 *
 * # Arguments
 * * `profile` - a reference to the profile that will contain the samples.
 * * `offset_values` - offset of the values
 * * `label_name` - name of the label used to identify sample(s)
 * * `label_value` - value of the label used to identify sample(s)
 * * `sum_value_offset` - offset of the value used as a sum (compute the average with
 *   `count_value_offset`)
 * * `count_value_offset` - offset of the value used as a count (compute the average with
 *   `sum_value_offset`)
 * * `sampling_distance` - this is the threshold for this sampling window. This value must not be
 *   equal to 0
 *
 * # Safety
 * This function must be called before serialize and must not be called after.
 * The `profile` ptr must point to a valid Profile object created by this
 * module.
 * This call is _NOT_ thread-safe.
 */
DDOG_CHECK_RETURN
struct ddog_prof_Profile_Result ddog_prof_Profile_add_upscaling_rule_poisson(struct ddog_prof_Profile *profile,
                                                                             struct ddog_prof_Slice_Usize offset_values,
                                                                             ddog_CharSlice label_name,
                                                                             ddog_CharSlice label_value,
                                                                             uintptr_t sum_value_offset,
                                                                             uintptr_t count_value_offset,
                                                                             uint64_t sampling_distance);

/**
 * Add a proportional-based upscaling rule which will be use to adjust values and make them
 * closer to reality.
 *
 * # Arguments
 * * `profile` - a reference to the profile that will contain the samples.
 * * `offset_values` - offset of the values
 * * `label_name` - name of the label used to identify sample(s)
 * * `label_value` - value of the label used to identify sample(s)
 * * `total_sampled` - number of sampled event (found in the pprof). This value must not be equal
 *   to 0
 * * `total_real` - number of events the profiler actually witnessed. This value must not be equal
 *   to 0
 *
 * # Safety
 * This function must be called before serialize and must not be called after.
 * The `profile` ptr must point to a valid Profile object created by this
 * module.
 * This call is _NOT_ thread-safe.
 */
DDOG_CHECK_RETURN
struct ddog_prof_Profile_Result ddog_prof_Profile_add_upscaling_rule_proportional(struct ddog_prof_Profile *profile,
                                                                                  struct ddog_prof_Slice_Usize offset_values,
                                                                                  ddog_CharSlice label_name,
                                                                                  ddog_CharSlice label_value,
                                                                                  uint64_t total_sampled,
                                                                                  uint64_t total_real);

/**
 * # Safety
 * Only pass a reference to a valid `ddog_prof_EncodedProfile`, or null. A
 * valid reference also means that it hasn't already been dropped (do not
 * call this twice on the same object).
 */
void ddog_prof_EncodedProfile_drop(struct ddog_prof_EncodedProfile *profile);

/**
 * Serialize the aggregated profile.
 * Drains the data, and then resets the profile for future use.
 *
 * Don't forget to clean up the ok with `ddog_prof_EncodedProfile_drop` or
 * the error variant with `ddog_Error_drop` when you are done with them.
 *
 * # Arguments
 * * `profile` - a reference to the profile being serialized.
 * * `end_time` - optional end time of the profile. If None/null is passed, the current time will
 *   be used.
 * * `duration_nanos` - Optional duration of the profile. Passing None or a negative duration will
 *   mean the duration will based on the end time minus the start time, but under anomalous
 *   conditions this may fail as system clocks can be adjusted, or the programmer accidentally
 *   passed an earlier time. The duration of the serialized profile will be set to zero for these
 *   cases.
 * * `start_time` - Optional start time for the next profile.
 *
 * # Safety
 * The `profile` must point to a valid profile object.
 * The `end_time` must be null or otherwise point to a valid TimeSpec object.
 * The `duration_nanos` must be null or otherwise point to a valid i64.
 */
DDOG_CHECK_RETURN
struct ddog_prof_Profile_SerializeResult ddog_prof_Profile_serialize(struct ddog_prof_Profile *profile,
                                                                     const struct ddog_Timespec *end_time,
                                                                     const int64_t *duration_nanos,
                                                                     const struct ddog_Timespec *start_time);

DDOG_CHECK_RETURN struct ddog_Slice_U8 ddog_Vec_U8_as_slice(const struct ddog_Vec_U8 *vec);

/**
 * Resets all data in `profile` except the sample types and period. Returns
 * true if it successfully reset the profile and false otherwise. The profile
 * remains valid if false is returned.
 *
 * # Arguments
 * * `profile` - A mutable reference to the profile to be reset.
 * * `start_time` - The time of the profile (after reset). Pass None/null to use the current time.
 *
 * # Safety
 * The `profile` must meet all the requirements of a mutable reference to the profile. Given this
 * can be called across an FFI boundary, the compiler cannot enforce this.
 * If `time` is not null, it must point to a valid Timespec object.
 */
DDOG_CHECK_RETURN
struct ddog_prof_Profile_Result ddog_prof_Profile_reset(struct ddog_prof_Profile *profile,
                                                        const struct ddog_Timespec *start_time);

#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus

#endif /* DDOG_PROFILING_H */
