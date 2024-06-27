// Unless explicitly stated otherwise all files in this repository are licensed under the Apache License Version 2.0.
// This product includes software developed at Datadog (https://www.datadoghq.com/). Copyright 2021-Present Datadog, Inc.

typedef struct ddog_DebuggerCapture ddog_DebuggerCapture;
typedef struct ddog_DebuggerValue ddog_DebuggerValue;


#ifndef DDOG_LIVE_DEBUGGER_H
#define DDOG_LIVE_DEBUGGER_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include "common.h"

typedef enum ddog_EvaluateAt {
  DDOG_EVALUATE_AT_ENTRY,
  DDOG_EVALUATE_AT_EXIT,
} ddog_EvaluateAt;

typedef enum ddog_FieldType {
  DDOG_FIELD_TYPE_STATIC,
  DDOG_FIELD_TYPE_ARG,
  DDOG_FIELD_TYPE_LOCAL,
} ddog_FieldType;

typedef enum ddog_InBodyLocation {
  DDOG_IN_BODY_LOCATION_NONE,
  DDOG_IN_BODY_LOCATION_START,
  DDOG_IN_BODY_LOCATION_END,
} ddog_InBodyLocation;

typedef enum ddog_MetricKind {
  DDOG_METRIC_KIND_COUNT,
  DDOG_METRIC_KIND_GAUGE,
  DDOG_METRIC_KIND_HISTOGRAM,
  DDOG_METRIC_KIND_DISTRIBUTION,
} ddog_MetricKind;

typedef enum ddog_SpanProbeTarget {
  DDOG_SPAN_PROBE_TARGET_ACTIVE,
  DDOG_SPAN_PROBE_TARGET_ROOT,
} ddog_SpanProbeTarget;

typedef struct ddog_DebuggerPayload ddog_DebuggerPayload;

typedef struct ddog_DslString ddog_DslString;

typedef struct ddog_Entry ddog_Entry;

typedef struct ddog_HashMap_CowStr__Value ddog_HashMap_CowStr__Value;

typedef struct ddog_InternalIntermediateValue ddog_InternalIntermediateValue;

typedef struct ddog_ProbeCondition ddog_ProbeCondition;

typedef struct ddog_ProbeValue ddog_ProbeValue;

typedef struct ddog_SenderHandle ddog_SenderHandle;

typedef struct ddog_SnapshotEvaluationError ddog_SnapshotEvaluationError;

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

typedef struct ddog_Tag {
  ddog_CharSlice name;
  const struct ddog_DslString *value;
} ddog_Tag;

typedef struct ddog_SpanProbeTag {
  struct ddog_Tag tag;
  bool next_condition;
} ddog_SpanProbeTag;

typedef struct ddog_SpanDecorationProbe {
  enum ddog_SpanProbeTarget target;
  const struct ddog_ProbeCondition *const *conditions;
  const struct ddog_SpanProbeTag *span_tags;
  uintptr_t span_tags_num;
} ddog_SpanDecorationProbe;

typedef struct ddog_CaptureConfiguration {
  uint32_t max_reference_depth;
  uint32_t max_collection_size;
  uint32_t max_length;
  uint32_t max_field_count;
} ddog_CaptureConfiguration;

typedef enum ddog_IntermediateValue_Tag {
  DDOG_INTERMEDIATE_VALUE_STRING,
  DDOG_INTERMEDIATE_VALUE_NUMBER,
  DDOG_INTERMEDIATE_VALUE_BOOL,
  DDOG_INTERMEDIATE_VALUE_NULL,
  DDOG_INTERMEDIATE_VALUE_REFERENCED,
} ddog_IntermediateValue_Tag;

typedef struct ddog_IntermediateValue {
  ddog_IntermediateValue_Tag tag;
  union {
    struct {
      ddog_CharSlice string;
    };
    struct {
      double number;
    };
    struct {
      bool bool_;
    };
    struct {
      const void *referenced;
    };
  };
} ddog_IntermediateValue;

typedef struct ddog_VoidCollection {
  intptr_t count;
  const void *elements;
  void (*free)(struct ddog_VoidCollection);
} ddog_VoidCollection;

typedef struct ddog_Evaluator {
  bool (*equals)(void*, struct ddog_IntermediateValue, struct ddog_IntermediateValue);
  bool (*greater_than)(void*, struct ddog_IntermediateValue, struct ddog_IntermediateValue);
  bool (*greater_or_equals)(void*, struct ddog_IntermediateValue, struct ddog_IntermediateValue);
  const void *(*fetch_identifier)(void*, const ddog_CharSlice*);
  const void *(*fetch_index)(void*, const void*, struct ddog_IntermediateValue);
  const void *(*fetch_nested)(void*, const void*, struct ddog_IntermediateValue);
  uintptr_t (*length)(void*, const void*);
  struct ddog_VoidCollection (*try_enumerate)(void*, const void*);
  ddog_CharSlice (*stringify)(void*, const void*);
  ddog_CharSlice (*get_string)(void*, const void*);
  intptr_t (*convert_index)(void*, const void*);
  bool (*instanceof)(void*, const void*, const ddog_CharSlice*);
} ddog_Evaluator;

/**
 * Holds the raw parts of a Rust Vec; it should only be created from Rust,
 * never from C.
 */
typedef struct ddog_Vec_SnapshotEvaluationError {
  const struct ddog_SnapshotEvaluationError *ptr;
  uintptr_t len;
  uintptr_t capacity;
} ddog_Vec_SnapshotEvaluationError;

typedef enum ddog_ConditionEvaluationResult_Tag {
  DDOG_CONDITION_EVALUATION_RESULT_SUCCESS,
  DDOG_CONDITION_EVALUATION_RESULT_FAILURE,
  DDOG_CONDITION_EVALUATION_RESULT_ERROR,
} ddog_ConditionEvaluationResult_Tag;

typedef struct ddog_ConditionEvaluationResult {
  ddog_ConditionEvaluationResult_Tag tag;
  union {
    struct {
      struct ddog_Vec_SnapshotEvaluationError *error;
    };
  };
} ddog_ConditionEvaluationResult;

typedef enum ddog_ValueEvaluationResult_Tag {
  DDOG_VALUE_EVALUATION_RESULT_SUCCESS,
  DDOG_VALUE_EVALUATION_RESULT_ERROR,
} ddog_ValueEvaluationResult_Tag;

typedef struct ddog_ValueEvaluationResult {
  ddog_ValueEvaluationResult_Tag tag;
  union {
    struct {
      struct ddog_InternalIntermediateValue *success;
    };
    struct {
      struct ddog_Vec_SnapshotEvaluationError *error;
    };
  };
} ddog_ValueEvaluationResult;

typedef enum ddog_Option_CharSlice_Tag {
  DDOG_OPTION_CHAR_SLICE_SOME_CHAR_SLICE,
  DDOG_OPTION_CHAR_SLICE_NONE_CHAR_SLICE,
} ddog_Option_CharSlice_Tag;

typedef struct ddog_Option_CharSlice {
  ddog_Option_CharSlice_Tag tag;
  union {
    struct {
      ddog_CharSlice some;
    };
  };
} ddog_Option_CharSlice;

typedef struct ddog_CharSliceVec {
  const ddog_CharSlice *strings;
  uintptr_t string_count;
} ddog_CharSliceVec;

typedef struct ddog_ProbeTarget {
  struct ddog_Option_CharSlice type_name;
  struct ddog_Option_CharSlice method_name;
  struct ddog_Option_CharSlice source_file;
  struct ddog_Option_CharSlice signature;
  struct ddog_CharSliceVec lines;
  enum ddog_InBodyLocation in_body_location;
} ddog_ProbeTarget;

typedef struct ddog_MetricProbe {
  enum ddog_MetricKind kind;
  ddog_CharSlice name;
  const struct ddog_ProbeValue *value;
} ddog_MetricProbe;

typedef struct ddog_LogProbe {
  const struct ddog_DslString *segments;
  const struct ddog_ProbeCondition *when;
  const struct ddog_CaptureConfiguration *capture;
  bool capture_snapshot;
  uint32_t sampling_snapshots_per_second;
} ddog_LogProbe;

typedef enum ddog_ProbeType_Tag {
  DDOG_PROBE_TYPE_METRIC,
  DDOG_PROBE_TYPE_LOG,
  DDOG_PROBE_TYPE_SPAN,
  DDOG_PROBE_TYPE_SPAN_DECORATION,
} ddog_ProbeType_Tag;

typedef struct ddog_ProbeType {
  ddog_ProbeType_Tag tag;
  union {
    struct {
      struct ddog_MetricProbe metric;
    };
    struct {
      struct ddog_LogProbe log;
    };
    struct {
      struct ddog_SpanDecorationProbe span_decoration;
    };
  };
} ddog_ProbeType;

typedef struct ddog_Probe {
  ddog_CharSlice id;
  uint64_t version;
  struct ddog_Option_CharSlice language;
  struct ddog_CharSliceVec tags;
  struct ddog_ProbeTarget target;
  enum ddog_EvaluateAt evaluate_at;
  struct ddog_ProbeType probe;
} ddog_Probe;

typedef struct ddog_FilterList {
  struct ddog_CharSliceVec package_prefixes;
  struct ddog_CharSliceVec classes;
} ddog_FilterList;

typedef struct ddog_ServiceConfiguration {
  ddog_CharSlice id;
  struct ddog_FilterList allow;
  struct ddog_FilterList deny;
  uint32_t sampling_snapshots_per_second;
} ddog_ServiceConfiguration;

typedef enum ddog_LiveDebuggingData_Tag {
  DDOG_LIVE_DEBUGGING_DATA_NONE,
  DDOG_LIVE_DEBUGGING_DATA_PROBE,
  DDOG_LIVE_DEBUGGING_DATA_SERVICE_CONFIGURATION,
} ddog_LiveDebuggingData_Tag;

typedef struct ddog_LiveDebuggingData {
  ddog_LiveDebuggingData_Tag tag;
  union {
    struct {
      struct ddog_Probe probe;
    };
    struct {
      struct ddog_ServiceConfiguration service_configuration;
    };
  };
} ddog_LiveDebuggingData;

typedef struct ddog_LiveDebuggingParseResult {
  struct ddog_LiveDebuggingData data;
  struct ddog_LiveDebuggingData *opaque_data;
} ddog_LiveDebuggingParseResult;

/**
 * Holds the raw parts of a Rust Vec; it should only be created from Rust,
 * never from C.
 */
typedef struct ddog_Vec_DebuggerPayload {
  const struct ddog_DebuggerPayload *ptr;
  uintptr_t len;
  uintptr_t capacity;
} ddog_Vec_DebuggerPayload;

typedef struct ddog_HashMap_CowStr__Value ddog_Fields;

/**
 * Holds the raw parts of a Rust Vec; it should only be created from Rust,
 * never from C.
 */
typedef struct ddog_Vec_DebuggerValue {
  const ddog_DebuggerValue *ptr;
  uintptr_t len;
  uintptr_t capacity;
} ddog_Vec_DebuggerValue;

/**
 * Holds the raw parts of a Rust Vec; it should only be created from Rust,
 * never from C.
 */
typedef struct ddog_Vec_Entry {
  const struct ddog_Entry *ptr;
  uintptr_t len;
  uintptr_t capacity;
} ddog_Vec_Entry;

typedef struct ddog_CaptureValue {
  ddog_CharSlice type;
  ddog_CharSlice value;
  ddog_Fields *fields;
  struct ddog_Vec_DebuggerValue elements;
  struct ddog_Vec_Entry entries;
  bool is_null;
  bool truncated;
  ddog_CharSlice not_captured_reason;
  ddog_CharSlice size;
} ddog_CaptureValue;

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

typedef enum ddog_Option_Error_Tag {
  DDOG_OPTION_ERROR_SOME_ERROR,
  DDOG_OPTION_ERROR_NONE_ERROR,
} ddog_Option_Error_Tag;

typedef struct ddog_Option_Error {
  ddog_Option_Error_Tag tag;
  union {
    struct {
      struct ddog_Error some;
    };
  };
} ddog_Option_Error;

typedef struct ddog_Option_Error ddog_MaybeError;

typedef struct ddog_OwnedCharSlice {
  ddog_CharSlice slice;
  void (*free)(ddog_CharSlice);
} ddog_OwnedCharSlice;

void drop_span_decoration_probe(struct ddog_SpanDecorationProbe);

struct ddog_CaptureConfiguration ddog_capture_defaults(void);

void ddog_register_expr_evaluator(const struct ddog_Evaluator *eval);

struct ddog_ConditionEvaluationResult ddog_evaluate_condition(const struct ddog_ProbeCondition *condition,
                                                              void *context);

void ddog_drop_void_collection_string(struct ddog_VoidCollection void_);

struct ddog_VoidCollection ddog_evaluate_unmanaged_string(const struct ddog_DslString *segments,
                                                          void *context,
                                                          struct ddog_Vec_SnapshotEvaluationError **errors);

struct ddog_ValueEvaluationResult ddog_evaluate_value(const struct ddog_ProbeValue *value,
                                                      void *context);

struct ddog_IntermediateValue ddog_evaluated_value_get(const struct ddog_InternalIntermediateValue *value);

void ddog_evaluated_value_drop(struct ddog_InternalIntermediateValue*);

struct ddog_VoidCollection ddog_evaluated_value_into_unmanaged_string(struct ddog_InternalIntermediateValue *value,
                                                                      void *context);

struct ddog_LiveDebuggingParseResult ddog_parse_live_debugger_json(ddog_CharSlice json);

void ddog_drop_live_debugger_parse_result(struct ddog_LiveDebuggingParseResult);

ddog_DebuggerCapture *ddog_create_exception_snapshot(struct ddog_Vec_DebuggerPayload *buffer,
                                                     ddog_CharSlice service,
                                                     ddog_CharSlice language,
                                                     ddog_CharSlice id,
                                                     ddog_CharSlice exception_id,
                                                     uint64_t timestamp);

struct ddog_DebuggerPayload *ddog_create_log_probe_snapshot(const struct ddog_Probe *probe,
                                                            const ddog_CharSlice *message,
                                                            ddog_CharSlice service,
                                                            ddog_CharSlice language,
                                                            uint64_t timestamp);

void ddog_update_payload_message(struct ddog_DebuggerPayload *payload, ddog_CharSlice message);

ddog_DebuggerCapture *ddog_snapshot_entry(struct ddog_DebuggerPayload *payload);

ddog_DebuggerCapture *ddog_snapshot_lines(struct ddog_DebuggerPayload *payload, uint32_t line);

ddog_DebuggerCapture *ddog_snapshot_exit(struct ddog_DebuggerPayload *payload);

void ddog_snapshot_add_field(ddog_DebuggerCapture *capture,
                             enum ddog_FieldType type,
                             ddog_CharSlice name,
                             struct ddog_CaptureValue value);

void ddog_capture_value_add_element(struct ddog_CaptureValue *value,
                                    struct ddog_CaptureValue element);

void ddog_capture_value_add_entry(struct ddog_CaptureValue *value,
                                  struct ddog_CaptureValue key,
                                  struct ddog_CaptureValue element);

void ddog_capture_value_add_field(struct ddog_CaptureValue *value,
                                  ddog_CharSlice key,
                                  struct ddog_CaptureValue element);

void ddog_snapshot_format_new_uuid(uint8_t (*buf)[36]);

ddog_CharSlice ddog_evaluation_error_first_msg(const struct ddog_Vec_SnapshotEvaluationError *vec);

void ddog_evaluation_error_drop(struct ddog_Vec_SnapshotEvaluationError*);

struct ddog_DebuggerPayload *ddog_evaluation_error_snapshot(const struct ddog_Probe *probe,
                                                            ddog_CharSlice service,
                                                            ddog_CharSlice language,
                                                            struct ddog_Vec_SnapshotEvaluationError *errors,
                                                            uint64_t timestamp);

void ddog_serialize_debugger_payload(const struct ddog_DebuggerPayload *payload,
                                     void (*callback)(ddog_CharSlice));

void ddog_drop_debugger_payload(struct ddog_DebuggerPayload*);

ddog_MaybeError ddog_live_debugger_spawn_sender(const ddog_Endpoint *endpoint,
                                                struct ddog_SenderHandle **handle);

bool ddog_live_debugger_send_raw_data(struct ddog_SenderHandle *handle,
                                      struct ddog_OwnedCharSlice data);

bool ddog_live_debugger_send_payload(struct ddog_SenderHandle *handle,
                                     const struct ddog_DebuggerPayload *data);

void ddog_live_debugger_drop_sender(struct ddog_SenderHandle *sender);

void ddog_live_debugger_join_sender(struct ddog_SenderHandle *sender);

#endif /* DDOG_LIVE_DEBUGGER_H */
