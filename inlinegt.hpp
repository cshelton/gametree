#ifndef INLINEGT_HPP
#define INLINEGT_HPP

#include <limits>
#include <utility>
#include <stdexcept>

template<typename V=double>
struct sstate {
	sstate(const V &minv = std::numeric_limits<V>::lowest(),
			const V &maxv = std::numeric_limits<V>::max()) :
			done(false), alpha(minv), beta(maxv) {}
	bool done;
	V ret,alpha,beta;
	template<typename S>
	void set(S &&value) { ret = std::forward<S>(value); done=true; }

	bool update(V &val, bool ismax) {
		if (!done) throw std::logic_error("value not set at leaf");
		if (ismax) {
			val = std::max(val,ret);
			alpha = std::max(val,alpha);
		} else {
			val = std::min(val,ret);
			beta = std::min(val,beta);
		}
		done = false;
		return alpha>=beta;
	}

	void setreturn(V val, V oldlim, bool ismax) {
		if (ismax) alpha = oldlim;
		else beta = oldlim;
		ret = val;
		done = true;
	}
};

#define COMMA ,

#define addstmt(stmt) \
	if (bool inlinegt_ignore=false) {} \
	else for (stmt; !inlinegt_ignore; inlinegt_ignore=true)

#define doafter(stmt) \
	for(int inlinegt_after=0;inlinegt_after<=1;inlinegt_after++) \
	if (inlinegt_after) { stmt; } \
	else

#define maxpick(state,loopdef) \
	addstmt(auto oldalpha = state.alpha COMMA myret = state.alpha) \
	doafter(state.setreturn(myret,oldalpha,true)) \
	for(bool inlinegt_break=false;!inlinegt_break;inlinegt_break=true) \
		if (inlinegt_break) {} \
		else for(loopdef) \
			if (inlinegt_break) break; \
			else doafter(inlinegt_break = state.update(myret,true)) \

#define minpick(state,loopdef) \
	addstmt(auto oldbeta = state.beta COMMA myret = state.beta) \
	doafter(state.setreturn(myret,oldbeta,false)) \
	for(bool inlinegt_break=false;!inlinegt_break;inlinegt_break=true) \
		if (inlinegt_break) {} \
		else for(loopdef) \
			if (inlinegt_break) break; \
			else doafter(inlinegt_break = state.update(myret,false)) \

#endif
