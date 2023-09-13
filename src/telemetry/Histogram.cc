#include "zeek/telemetry/Histogram.h"

#include "opentelemetry/metrics/provider.h"

using namespace zeek::telemetry;

IntHistogramFamily::IntHistogramFamily(std::string_view prefix, std::string_view name,
                                       Span<const std::string_view> labels, std::string_view helptext,
                                       std::string_view unit, bool is_sum)
    : BaseHistogramFamily(prefix, name, labels, helptext, unit, is_sum) {
    auto p = opentelemetry::metrics::Provider::GetMeterProvider();
    auto m = p->GetMeter(std::string{prefix});
    instrument = m->CreateUInt64Histogram(std::string{prefix} + "-" + std::string{name}, std::string{helptext},
                                          std::string{unit});
}

DblHistogramFamily::DblHistogramFamily(std::string_view prefix, std::string_view name,
                                       Span<const std::string_view> labels, std::string_view helptext,
                                       std::string_view unit, bool is_sum)
    : BaseHistogramFamily(prefix, name, labels, helptext, unit, is_sum) {
    auto p = opentelemetry::metrics::Provider::GetMeterProvider();
    auto m = p->GetMeter(std::string{prefix});
    instrument = m->CreateDoubleHistogram(std::string{prefix} + "-" + std::string{name}, std::string{helptext},
                                          std::string{unit});
}
