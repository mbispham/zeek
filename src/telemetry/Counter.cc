#include "zeek/telemetry/Counter.h"

#include "opentelemetry/metrics/provider.h"

using namespace zeek::telemetry;

IntCounterFamily::IntCounterFamily(std::string_view prefix, std::string_view name, Span<const std::string_view> labels,
                                   std::string_view helptext, std::string_view unit, bool is_sum,
                                   opentelemetry::metrics::ObservableCallbackPtr callback)
    : BaseCounterFamily(prefix, name, labels, helptext, unit, is_sum) {
    auto p = opentelemetry::metrics::Provider::GetMeterProvider();
    auto m = p->GetMeter(std::string{prefix});

    if ( ! callback )
        instrument = m->CreateUInt64Counter(std::string{prefix} + "-" + std::string{name}, std::string{helptext},
                                            std::string{unit});
    else {
        observable = m->CreateInt64ObservableCounter(std::string{prefix} + "-" + std::string{name},
                                                     std::string{helptext}, std::string{unit});
        observable->AddCallback(callback, nullptr);
    }
}

DblCounterFamily::DblCounterFamily(std::string_view prefix, std::string_view name, Span<const std::string_view> labels,
                                   std::string_view helptext, std::string_view unit, bool is_sum,
                                   opentelemetry::metrics::ObservableCallbackPtr callback)
    : BaseCounterFamily(prefix, name, labels, helptext, unit, is_sum) {
    auto p = opentelemetry::metrics::Provider::GetMeterProvider();
    auto m = p->GetMeter(std::string{prefix});
    if ( ! callback )
        instrument = m->CreateDoubleCounter(std::string{prefix} + "-" + std::string{name}, std::string{helptext},
                                            std::string{unit});
    else {
        observable = m->CreateDoubleObservableCounter(std::string{prefix} + "-" + std::string{name},
                                                      std::string{helptext}, std::string{unit});
        observable->AddCallback(callback, nullptr);
    }
}
