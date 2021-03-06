/*	Copyright 2016 (c) Michael Thomas (malinka) <malinka@entropy-development.com>
	Distributed under the terms of the GNU Affero General Public License v3
*/

#include "Check.hh"

using namespace Entropy::Hecate;
using namespace std;

PercentType Check::default_luck(ENTROPY_HECATE_DEFAULT_LUCK);

Check::Check()
	: _modifiers(), _luck(default_luck)
{}

Check::Check(PercentType &luck)
	: _modifiers(), _luck(luck)
{}

Check::Check(const vector<Modifier> &modifiers)
	: Check()
{
	for(auto &mod : modifiers) {
		Add(mod);
	}
}

Check::Check(PercentType &luck, const vector<Modifier> &modifiers)
	: Check(luck)
{
	for(auto &mod : modifiers) {
		Add(mod);
	}
}

Check &Check::Add(const Modifier &mod)
{
	_modifiers.Add(mod);

	return *this;
}

Check &Check::Add(const function<void(const Check::Result &)> &cb)
{
	_callbacks.push_back(cb);

	return *this;
}

ModifierList &Check::Modifiers()
{
	return _modifiers;
}

const ModifierList &Check::Modifiers() const
{
	return _modifiers;
}

Check::Result::Result(const PercentType value, const PercentType chance, const PercentType roll, const PercentType luck, const ModifierList &l)
	: _value(value), _chance(chance), _roll(roll), _luck(luck)
{
	for(auto &&i : l) {
		for(auto &&m : i.second) {
			result_modifier t;

			t.current = m.Value();
			t.modifier = m;

			_modifiers.push_back(t);
		}
	}
}

Check::Result::operator bool() const
{
	return isSuccess();
}

bool Check::Result::isSuccess() const
{
	return _value > 0;
}

bool Check::Result::isCritical() const
{
	return _roll > 100 - _luck || _roll < _luck;
}

PercentType Check::Result::Value() const
{
	return _value;
}

PercentType Check::Result::Chance() const
{
	return _chance;
}

PercentType Check::Result::Luck() const
{
	return _luck;
}

size_t Check::Result::size() const
{
	return _modifiers.size();
}

vector<Check::Result::result_modifier>::iterator Check::Result::begin()
{
	return _modifiers.begin();
}

vector<Check::Result::result_modifier>::iterator Check::Result::end()
{
	return _modifiers.end();
}

void onSuccess::operator () (const Check::Result &result) const
{
	if(result)
		_cb(result);
}

void onFailure::operator () (const Check::Result &result) const
{
	if(!result)
		_cb(result);
}

void onCritical::operator () (const Check::Result &result) const
{
	if(result.isCritical())
		_cb(result);
}

onSuccess::onSuccess(const function <void(const Check::Result &)> &cb)
	: _cb(cb)
{}

onSuccess::onSuccess(const onSuccess &) = default;
onSuccess::onSuccess(onSuccess &&) = default;
onSuccess::~onSuccess() = default;

onFailure::onFailure(const function <void(const Check::Result &)> &cb)
	: _cb(cb)
{}

onFailure::onFailure(const onFailure &) = default;
onFailure::onFailure(onFailure &&) = default;
onFailure::~onFailure() = default;

onCritical::onCritical(const function <void(const Check::Result &)> &cb)
	: _cb(cb)
{}

onCritical::onCritical(const onCritical &) = default;
onCritical::onCritical(onCritical &&) = default;
onCritical::~onCritical() = default;
