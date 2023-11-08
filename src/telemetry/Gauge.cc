#include "zeek/telemetry/Gauge.h"

#include "zeek/telemetry/Manager.h"

#include "opentelemetry/metrics/provider.h"

using namespace zeek::telemetry;

IntGaugeFamily::IntGaugeFamily(std::string_view prefix, std::string_view name, Span<const std::string_view> labels,
                               std::string_view helptext, std::string_view unit, bool is_sum)
    : BaseGaugeFamily(prefix, name, labels, helptext, unit, is_sum) {
    auto p = opentelemetry::metrics::Provider::GetMeterProvider();
    auto m = p->GetMeter(std::string{prefix});

    instrument = m->CreateInt64UpDownCounter(FullName(), std::string{helptext});

    if ( is_sum )
        telemetry_mgr->AddView(FullName(), std::string{helptext}, "",
                               opentelemetry::sdk::metrics::InstrumentType::kUpDownCounter,
                               opentelemetry::sdk::metrics::AggregationType::kSum);
}

DblGaugeFamily::DblGaugeFamily(std::string_view prefix, std::string_view name, Span<const std::string_view> labels,
                               std::string_view helptext, std::string_view unit, bool is_sum)
    : BaseGaugeFamily(prefix, name, labels, helptext, unit, is_sum) {
    auto p = opentelemetry::metrics::Provider::GetMeterProvider();
    auto m = p->GetMeter(std::string{prefix});

    instrument = m->CreateDoubleUpDownCounter(FullName(), std::string{helptext});

    if ( is_sum )
        telemetry_mgr->AddView(FullName(), std::string{helptext}, "",
                               opentelemetry::sdk::metrics::InstrumentType::kUpDownCounter,
                               opentelemetry::sdk::metrics::AggregationType::kSum);
}
