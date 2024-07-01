// Copyright 2021-Present Datadog, Inc. https://www.datadoghq.com/
// SPDX-License-Identifier: Apache-2.0


#ifndef DDOG_TELEMETRY_H
#define DDOG_TELEMETRY_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include "common.h"

typedef enum ddog_ConfigurationOrigin {
  DDOG_CONFIGURATION_ORIGIN_ENV_VAR,
  DDOG_CONFIGURATION_ORIGIN_CODE,
  DDOG_CONFIGURATION_ORIGIN_DD_CONFIG,
  DDOG_CONFIGURATION_ORIGIN_REMOTE_CONFIG,
  DDOG_CONFIGURATION_ORIGIN_DEFAULT,
} ddog_ConfigurationOrigin;

typedef enum ddog_LogLevel {
  DDOG_LOG_LEVEL_ERROR,
  DDOG_LOG_LEVEL_WARN,
  DDOG_LOG_LEVEL_DEBUG,
} ddog_LogLevel;

typedef enum ddog_MetricNamespace {
  DDOG_METRIC_NAMESPACE_TRACERS,
  DDOG_METRIC_NAMESPACE_PROFILERS,
  DDOG_METRIC_NAMESPACE_RUM,
  DDOG_METRIC_NAMESPACE_APPSEC,
  DDOG_METRIC_NAMESPACE_IDE_PLUGINS,
  DDOG_METRIC_NAMESPACE_LIVE_DEBUGGER,
  DDOG_METRIC_NAMESPACE_IAST,
  DDOG_METRIC_NAMESPACE_GENERAL,
  DDOG_METRIC_NAMESPACE_TELEMETRY,
  DDOG_METRIC_NAMESPACE_APM,
  DDOG_METRIC_NAMESPACE_SIDECAR,
} ddog_MetricNamespace;

typedef enum ddog_MetricType {
  DDOG_METRIC_TYPE_GAUGE,
  DDOG_METRIC_TYPE_COUNT,
  DDOG_METRIC_TYPE_DISTRIBUTION,
} ddog_MetricType;

typedef enum ddog_TelemetryWorkerBuilderBoolProperty {
  DDOG_TELEMETRY_WORKER_BUILDER_BOOL_PROPERTY_CONFIG_TELEMETRY_DEBUG_LOGGING_ENABLED,
} ddog_TelemetryWorkerBuilderBoolProperty;

typedef enum ddog_TelemetryWorkerBuilderEndpointProperty {
  DDOG_TELEMETRY_WORKER_BUILDER_ENDPOINT_PROPERTY_CONFIG_ENDPOINT,
} ddog_TelemetryWorkerBuilderEndpointProperty;

typedef enum ddog_TelemetryWorkerBuilderStrProperty {
  DDOG_TELEMETRY_WORKER_BUILDER_STR_PROPERTY_APPLICATION_SERVICE_VERSION,
  DDOG_TELEMETRY_WORKER_BUILDER_STR_PROPERTY_APPLICATION_ENV,
  DDOG_TELEMETRY_WORKER_BUILDER_STR_PROPERTY_APPLICATION_RUNTIME_NAME,
  DDOG_TELEMETRY_WORKER_BUILDER_STR_PROPERTY_APPLICATION_RUNTIME_VERSION,
  DDOG_TELEMETRY_WORKER_BUILDER_STR_PROPERTY_APPLICATION_RUNTIME_PATCHES,
  DDOG_TELEMETRY_WORKER_BUILDER_STR_PROPERTY_HOST_CONTAINER_ID,
  DDOG_TELEMETRY_WORKER_BUILDER_STR_PROPERTY_HOST_OS,
  DDOG_TELEMETRY_WORKER_BUILDER_STR_PROPERTY_HOST_KERNEL_NAME,
  DDOG_TELEMETRY_WORKER_BUILDER_STR_PROPERTY_HOST_KERNEL_RELEASE,
  DDOG_TELEMETRY_WORKER_BUILDER_STR_PROPERTY_HOST_KERNEL_VERSION,
  DDOG_TELEMETRY_WORKER_BUILDER_STR_PROPERTY_RUNTIME_ID,
} ddog_TelemetryWorkerBuilderStrProperty;

typedef struct ddog_Endpoint ddog_Endpoint;

typedef struct ddog_Tag ddog_Tag;

typedef struct ddog_TelemetryWorkerBuilder ddog_TelemetryWorkerBuilder;

/**
 * TelemetryWorkerHandle is a handle which allows interactions with the telemetry worker.
 * The handle is safe to use across threads.
 *
 * The worker won't send data to the agent until you call `TelemetryWorkerHandle::send_start`
 *
 * To stop the worker, call `TelemetryWorkerHandle::send_stop` which trigger flush asynchronously
 * then `TelemetryWorkerHandle::wait_for_shutdown`
 */
typedef struct ddog_TelemetryWorkerHandle ddog_TelemetryWorkerHandle;

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

typedef enum ddog_Option_Bool_Tag {
  DDOG_OPTION_BOOL_SOME_BOOL,
  DDOG_OPTION_BOOL_NONE_BOOL,
} ddog_Option_Bool_Tag;

typedef struct ddog_Option_Bool {
  ddog_Option_Bool_Tag tag;
  union {
    struct {
      bool some;
    };
  };
} ddog_Option_Bool;

typedef struct ddog_ContextKey {
  uint32_t _0;
  enum ddog_MetricType _1;
} ddog_ContextKey;

/**
 * Holds the raw parts of a Rust Vec; it should only be created from Rust,
 * never from C.
 */
typedef struct ddog_Vec_Tag {
  const struct ddog_Tag *ptr;
  uintptr_t len;
  uintptr_t capacity;
} ddog_Vec_Tag;

/**
 * # Safety
 * * builder should be a non null pointer to a null pointer to a builder
 */
ddog_MaybeError ddog_telemetry_builder_instantiate(struct ddog_TelemetryWorkerBuilder **out_builder,
                                                   ddog_CharSlice service_name,
                                                   ddog_CharSlice language_name,
                                                   ddog_CharSlice language_version,
                                                   ddog_CharSlice tracer_version);

/**
 * # Safety
 * * builder should be a non null pointer to a null pointer to a builder
 */
ddog_MaybeError ddog_telemetry_builder_instantiate_with_hostname(struct ddog_TelemetryWorkerBuilder **out_builder,
                                                                 ddog_CharSlice hostname,
                                                                 ddog_CharSlice service_name,
                                                                 ddog_CharSlice language_name,
                                                                 ddog_CharSlice language_version,
                                                                 ddog_CharSlice tracer_version);

ddog_MaybeError ddog_telemetry_builder_with_native_deps(struct ddog_TelemetryWorkerBuilder *builder,
                                                        bool include_native_deps);

ddog_MaybeError ddog_telemetry_builder_with_rust_shared_lib_deps(struct ddog_TelemetryWorkerBuilder *builder,
                                                                 bool include_rust_shared_lib_deps);

ddog_MaybeError ddog_telemetry_builder_with_config(struct ddog_TelemetryWorkerBuilder *builder,
                                                   ddog_CharSlice name,
                                                   ddog_CharSlice value,
                                                   enum ddog_ConfigurationOrigin origin);

/**
 * Builds the telemetry worker and return a handle to it
 *
 * # Safety
 * * handle should be a non null pointer to a null pointer
 */
ddog_MaybeError ddog_telemetry_builder_run(struct ddog_TelemetryWorkerBuilder *builder,
                                           struct ddog_TelemetryWorkerHandle **out_handle);

/**
 * Builds the telemetry worker and return a handle to it. The worker will only process and send
 * telemetry metrics and telemetry logs. Any lifecyle/dependency/configuration event will be
 * ignored
 *
 * # Safety
 * * handle should be a non null pointer to a null pointer
 */
ddog_MaybeError ddog_telemetry_builder_run_metric_logs(struct ddog_TelemetryWorkerBuilder *builder,
                                                       struct ddog_TelemetryWorkerHandle **out_handle);

ddog_MaybeError ddog_telemetry_builder_with_str_application_service_version(struct ddog_TelemetryWorkerBuilder *telemetry_builder,
                                                                            ddog_CharSlice param);

ddog_MaybeError ddog_telemetry_builder_with_str_application_env(struct ddog_TelemetryWorkerBuilder *telemetry_builder,
                                                                ddog_CharSlice param);

ddog_MaybeError ddog_telemetry_builder_with_str_application_runtime_name(struct ddog_TelemetryWorkerBuilder *telemetry_builder,
                                                                         ddog_CharSlice param);

ddog_MaybeError ddog_telemetry_builder_with_str_application_runtime_version(struct ddog_TelemetryWorkerBuilder *telemetry_builder,
                                                                            ddog_CharSlice param);

ddog_MaybeError ddog_telemetry_builder_with_str_application_runtime_patches(struct ddog_TelemetryWorkerBuilder *telemetry_builder,
                                                                            ddog_CharSlice param);

ddog_MaybeError ddog_telemetry_builder_with_str_host_container_id(struct ddog_TelemetryWorkerBuilder *telemetry_builder,
                                                                  ddog_CharSlice param);

ddog_MaybeError ddog_telemetry_builder_with_str_host_os(struct ddog_TelemetryWorkerBuilder *telemetry_builder,
                                                        ddog_CharSlice param);

ddog_MaybeError ddog_telemetry_builder_with_str_host_kernel_name(struct ddog_TelemetryWorkerBuilder *telemetry_builder,
                                                                 ddog_CharSlice param);

ddog_MaybeError ddog_telemetry_builder_with_str_host_kernel_release(struct ddog_TelemetryWorkerBuilder *telemetry_builder,
                                                                    ddog_CharSlice param);

ddog_MaybeError ddog_telemetry_builder_with_str_host_kernel_version(struct ddog_TelemetryWorkerBuilder *telemetry_builder,
                                                                    ddog_CharSlice param);

ddog_MaybeError ddog_telemetry_builder_with_str_runtime_id(struct ddog_TelemetryWorkerBuilder *telemetry_builder,
                                                           ddog_CharSlice param);

/**
 *      Sets a property from it's string value.
 *
 *     Available properties:
 *
 *     * application.service_version
 *
 *     * application.env
 *
 *     * application.runtime_name
 *
 *     * application.runtime_version
 *
 *     * application.runtime_patches
 *
 *     * host.container_id
 *
 *     * host.os
 *
 *     * host.kernel_name
 *
 *     * host.kernel_release
 *
 *     * host.kernel_version
 *
 *     * runtime_id
 *
 *
 */
ddog_MaybeError ddog_telemetry_builder_with_property_str(struct ddog_TelemetryWorkerBuilder *telemetry_builder,
                                                         enum ddog_TelemetryWorkerBuilderStrProperty property,
                                                         ddog_CharSlice param);

/**
 *      Sets a property from it's string value.
 *
 *     Available properties:
 *
 *     * application.service_version
 *
 *     * application.env
 *
 *     * application.runtime_name
 *
 *     * application.runtime_version
 *
 *     * application.runtime_patches
 *
 *     * host.container_id
 *
 *     * host.os
 *
 *     * host.kernel_name
 *
 *     * host.kernel_release
 *
 *     * host.kernel_version
 *
 *     * runtime_id
 *
 *
 */
ddog_MaybeError ddog_telemetry_builder_with_str_named_property(struct ddog_TelemetryWorkerBuilder *telemetry_builder,
                                                               ddog_CharSlice property,
                                                               ddog_CharSlice param);

ddog_MaybeError ddog_telemetry_builder_with_bool_config_telemetry_debug_logging_enabled(struct ddog_TelemetryWorkerBuilder *telemetry_builder,
                                                                                        bool param);

/**
 *      Sets a property from it's string value.
 *
 *     Available properties:
 *
 *     * config.telemetry_debug_logging_enabled
 *
 *
 */
ddog_MaybeError ddog_telemetry_builder_with_property_bool(struct ddog_TelemetryWorkerBuilder *telemetry_builder,
                                                          enum ddog_TelemetryWorkerBuilderBoolProperty property,
                                                          bool param);

/**
 *      Sets a property from it's string value.
 *
 *     Available properties:
 *
 *     * config.telemetry_debug_logging_enabled
 *
 *
 */
ddog_MaybeError ddog_telemetry_builder_with_bool_named_property(struct ddog_TelemetryWorkerBuilder *telemetry_builder,
                                                                ddog_CharSlice property,
                                                                bool param);

ddog_MaybeError ddog_telemetry_builder_with_endpoint_config_endpoint(struct ddog_TelemetryWorkerBuilder *telemetry_builder,
                                                                     const struct ddog_Endpoint *param);

/**
 *      Sets a property from it's string value.
 *
 *     Available properties:
 *
 *     * config.endpoint
 *
 *
 */
ddog_MaybeError ddog_telemetry_builder_with_property_endpoint(struct ddog_TelemetryWorkerBuilder *telemetry_builder,
                                                              enum ddog_TelemetryWorkerBuilderEndpointProperty property,
                                                              const struct ddog_Endpoint *param);

/**
 *      Sets a property from it's string value.
 *
 *     Available properties:
 *
 *     * config.endpoint
 *
 *
 */
ddog_MaybeError ddog_telemetry_builder_with_endpoint_named_property(struct ddog_TelemetryWorkerBuilder *telemetry_builder,
                                                                    ddog_CharSlice property,
                                                                    const struct ddog_Endpoint *param);

ddog_MaybeError ddog_telemetry_handle_add_dependency(const struct ddog_TelemetryWorkerHandle *handle,
                                                     ddog_CharSlice dependency_name,
                                                     ddog_CharSlice dependency_version);

ddog_MaybeError ddog_telemetry_handle_add_integration(const struct ddog_TelemetryWorkerHandle *handle,
                                                      ddog_CharSlice dependency_name,
                                                      ddog_CharSlice dependency_version,
                                                      bool enabled,
                                                      struct ddog_Option_Bool compatible,
                                                      struct ddog_Option_Bool auto_enabled);

/**
 * * indentifier: identifies a logging location uniquely. This can for instance be the template
 *   using for the log message or the concatenated file + line of the origin of the log
 * * stack_trace: stack trace associated with the log. If no stack trace is available, an empty
 *   string should be passed
 */
ddog_MaybeError ddog_telemetry_handle_add_log(const struct ddog_TelemetryWorkerHandle *handle,
                                              ddog_CharSlice indentifier,
                                              ddog_CharSlice message,
                                              enum ddog_LogLevel level,
                                              ddog_CharSlice stack_trace);

ddog_MaybeError ddog_telemetry_handle_start(const struct ddog_TelemetryWorkerHandle *handle);

struct ddog_TelemetryWorkerHandle *ddog_telemetry_handle_clone(const struct ddog_TelemetryWorkerHandle *handle);

ddog_MaybeError ddog_telemetry_handle_stop(const struct ddog_TelemetryWorkerHandle *handle);

/**
 * * compatible: should be false if the metric is language specific, true otherwise
 */
struct ddog_ContextKey ddog_telemetry_handle_register_metric_context(const struct ddog_TelemetryWorkerHandle *handle,
                                                                     ddog_CharSlice name,
                                                                     enum ddog_MetricType metric_type,
                                                                     struct ddog_Vec_Tag tags,
                                                                     bool common,
                                                                     enum ddog_MetricNamespace namespace_);

ddog_MaybeError ddog_telemetry_handle_add_point(const struct ddog_TelemetryWorkerHandle *handle,
                                                const struct ddog_ContextKey *context_key,
                                                double value);

ddog_MaybeError ddog_telemetry_handle_add_point_with_tags(const struct ddog_TelemetryWorkerHandle *handle,
                                                          const struct ddog_ContextKey *context_key,
                                                          double value,
                                                          struct ddog_Vec_Tag extra_tags);

/**
 * This function takes ownership of the handle. It should not be used after calling it
 */
void ddog_telemetry_handle_wait_for_shutdown(struct ddog_TelemetryWorkerHandle *handle);

/**
 * This function takes ownership of the handle. It should not be used after calling it
 */
void ddog_telemetry_handle_wait_for_shutdown_ms(struct ddog_TelemetryWorkerHandle *handle,
                                                uint64_t wait_for_ms);

/**
 * Drops the handle without waiting for shutdown. The worker will continue running in the
 * background until it exits by itself
 */
void ddog_telemetry_handle_drop(struct ddog_TelemetryWorkerHandle *handle);

#endif /* DDOG_TELEMETRY_H */
