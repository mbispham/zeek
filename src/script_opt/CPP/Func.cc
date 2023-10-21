// See the file "COPYING" in the main distribution directory for copyright.

#include "zeek/script_opt/CPP/Func.h"

#include <broker/error.hh>

#include "zeek/Desc.h"
#include "zeek/broker/Data.h"

namespace zeek::detail
	{

using namespace std;

unordered_map<p_hash_type, CompiledScript> compiled_scripts;
unordered_map<string, unordered_set<p_hash_type>> added_bodies;
unordered_map<p_hash_type, void (*)()> standalone_callbacks;
vector<void (*)()> standalone_finalizations;

void CPPFunc::Describe(ODesc* d) const
	{
	d->AddSP("compiled function");
	d->Add(name);
	}

CPPStmt::CPPStmt(const char* _name, const char* filename, int line_num)
	: Stmt(STMT_CPP), name(_name)
	{
	// We build a fake CallExpr node to be used for error-reporting.
	// It doesn't matter that it matches the actual function/event/hook
	// type-checking-wise, but it *does* need to type-check.
	auto no_args = make_intrusive<RecordType>(nullptr);
	auto no_yield = base_type(TYPE_VOID);
	auto ft = make_intrusive<FuncType>(no_args, no_yield, FUNC_FLAVOR_FUNCTION);

	vector<StmtPtr> no_bodies;
	vector<int> no_priorities;

	auto sf = make_intrusive<ScriptFunc>(name, ft, no_bodies, no_priorities);
	auto fv = make_intrusive<FuncVal>(sf);
	auto empty_args = make_intrusive<ListExpr>();

	ce = make_intrusive<CallExpr>(make_intrusive<ConstExpr>(fv), empty_args);
	Location loc(filename, line_num, line_num, 1, 1);
	ce->SetLocationInfo(&loc);
	}

CPPLambdaFunc::CPPLambdaFunc(string _name, FuncTypePtr ft, CPPStmtPtr _l_body)
	: ScriptFunc(std::move(_name), std::move(ft), {_l_body}, {0})
	{
	l_body = std::move(_l_body);
	}

std::optional<BrokerData> CPPLambdaFunc::SerializeCaptures() const
	{
	static constexpr std::string_view name = "CopyFrame";

	auto vals = l_body->SerializeLambdaCaptures();

	BrokerListBuilder body_builder;
	body_builder.Reserve(vals.size());

	for ( const auto& val : vals )
		{
		BrokerData tmp;
		if ( ! tmp.Convert(val) )
			return std::nullopt;

		TypeTag tag = val->GetType()->Tag();
		body_builder.AddList(std::move(tmp), static_cast<int64_t>(tag));
		}

	BrokerListBuilder result_builder;
	result_builder.Reserve(2);
	result_builder.AddString(name.data(), name.size());
	result_builder.Add(std::move(body_builder));

	return {std::move(result_builder).Build()};
	}

void CPPLambdaFunc::SetCaptures(Frame* f)
	{
	l_body->SetLambdaCaptures(f);
	}

FuncPtr CPPLambdaFunc::DoClone()
	{
	return make_intrusive<CPPLambdaFunc>(name, type, l_body->Clone());
	}

	} // zeek::detail
