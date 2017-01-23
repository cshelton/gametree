#include <iostream>
#include <vector>
#include "gt.hpp"

using namespace std;
using namespace gametree;

struct cval {
	double val;
	constexpr cval(double d) : val(d) {}
	constexpr double operator()() const { return val; }
};

template<std::size_t I>
struct gg;

template<typename T, typename... Ts>
struct consttup {
	T t;
	consttup<Ts...> ts;

	template<typename S, typename... Ss>
	constexpr consttup(S &&s, Ss &&...ss)
		: t(std::forward<S>(s)), ts(std::forward<Ss>(ss)...) {}

	
	
	template<std::size_t I>
	constexpr auto get() const { return gg<I>::get(*this); }

	//template<std::size_t I>
	//constexpr auto get() { return gg<I>::get(*this); }
};

// inefficient...
template<std::size_t I>
struct gg {
	template<typename CT>
	constexpr static auto get(const CT &t) {
		return gg<I-1>::get(t.ts);
	}
	template<typename CT>
	constexpr auto get(CT &t) {
		return gg<I-1>::get(t.ts);
	}
};

template<>
struct gg<0> {
	template<typename CT>
	constexpr auto get(const CT &t) {
		return t.t;
	}
	template<typename CT>
	constexpr auto get(CT &t) {
		return t.t;
	}
};

template<std::size_t ...>
struct seq { };

template<std::size_t N, std::size_t ...S>
struct gens : gens<N-1, N-1, S...> { };

template<std::size_t ...S>
struct gens<0, S...> {
  typedef seq<S...> type;
};

template<typename F, typename... Ts>
struct app {
	F f;
	consttup<Ts...> params;

	template<typename FF, typename... TTs>
	app(FF &&ff, TTs &&... tts) : f(std::forward<FF>(ff)), params(std::forward<TTs>(tts)...) {}

	constexpr auto operator()() const {
		return callf(typename gens<sizeof...(Ts)>::type());
	}

	template<std::size_t ...S>
	constexpr auto callf(seq<S...>) {
		return f(params.get<S>()...);
	}
};

template<typename F, typename... Ts>
auto call(F &&f, Ts &&...ts) {
	return app<F,Ts...>(std::forward<F>(f),std::forward<Ts>(ts)...);
}

int main(int argc, char **argv) {
	constexpr double val = expeval(cval(1.0),cval(3.0),cval(4.3));
	cout << val << endl;

	cout << maxeval(call(expeval,cval(1.0),cval(5.0),cval(2.2)),
				call(expeval(cval(0.5),cval(5.5))))
		<< endl;

	vector<cval> v{cval{1.0},cval{3.0},cval{5.3}};
	cout << maxeval(v) << endl;
}
