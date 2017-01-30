#ifndef INLINEGT_HPP
#define INLINEGT_HPP

#include <limits>
#include <utility>
#include <stdexcept>

namespace inlinegt {

template<typename V=double>
struct sstate {
	sstate(const V &minv = std::numeric_limits<V>::lowest(),
			const V &maxv = std::numeric_limits<V>::max()) :
			done(false), alpha(minv), beta(maxv),minval(minv),maxval(maxv) {}
	bool done;
	V ret,alpha,beta,minval,maxval;
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

	void updateexp(V &sum, int &count) {
		if (!done) throw std::logic_error("value not set at leaf");
		if (count) sum += ret;
		else sum=ret;
		count++;
		done = false;
	}

	bool updateexp(V &sum, int &count, int n) {
		if (!done) throw std::logic_error("value not set at leaf");
		if (count>=n) throw std::logic_error("number of elements in randpick too low");
		if (count) sum += ret;
		else sum=ret;
		count++;
		done = false;
		auto mn = (sum+(n-count)*minval)/n;
		auto mx = (sum+(n-count)*maxval)/n;
		return mx<=alpha || mn>=beta;
	}

	void setreturn(V val) {
		ret = val;
		done = true;
	}

	void setreturn(V val, V oldlim, bool ismax) {
		if (ismax) alpha = oldlim;
		else beta = oldlim;
		ret = val;
		done = true;
	}
};

}

// get number of arguments with __NARG__
#define __NARG__(...)  __NARG_I_(__VA_ARGS__,__RSEQ_N())
#define __NARG_I_(...) __ARG_N(__VA_ARGS__)
#define __ARG_N( \
      _1, _2, _3, _4, _5, _6, _7, _8, _9,_10, \
     _11,_12,_13,_14,_15,_16,_17,_18,_19,_20, \
     _21,_22,_23,_24,_25,_26,_27,_28,_29,_30, \
     _31,_32,_33,_34,_35,_36,_37,_38,_39,_40, \
     _41,_42,_43,_44,_45,_46,_47,_48,_49,_50, \
     _51,_52,_53,_54,_55,_56,_57,_58,_59,_60, \
     _61,_62,_63,N,...) N
#define __MULTARG__(...)  __MULTARG_I_(__VA_ARGS__,__ONEORMORE_N())
#define __MULTARG_I_(...) __ARG_N(__VA_ARGS__)
#define __RSEQ_N() \
     63,62,61,60,                   \
     59,58,57,56,55,54,53,52,51,50, \
     49,48,47,46,45,44,43,42,41,40, \
     39,38,37,36,35,34,33,32,31,30, \
     29,28,27,26,25,24,23,22,21,20, \
     19,18,17,16,15,14,13,12,11,10, \
     9,8,7,6,5,4,3,2,1,0
#define __ONEORMORE_N() \
     2,2,2,2,                   \
     2,2,2,2,2,2,2,2,2,2, \
     2,2,2,2,2,2,2,2,2,2, \
     2,2,2,2,2,2,2,2,2,2, \
     2,2,2,2,2,2,2,2,2,2, \
     2,2,2,2,2,2,2,2,2,2, \
     2,2,2,2,2,2,3,2,1,0

// general definition for any function name
#define _VFUNC_(name, n) name##n
#define _VFUNC(name, n) _VFUNC_(name, n)
#define VFUNC(func, ...) _VFUNC(func, __NARG__(__VA_ARGS__)) (__VA_ARGS__)

#define _MFUNC_(name, n) name##n
#define _MFUNC(name, n) _MFUNC_(name, n)
#define MFUNC(func, ...) _MFUNC(func, __MULTARG__(__VA_ARGS__)) (__VA_ARGS__)


#define COMMA ,

#define addstmt(stmt) \
	if (bool inlinegt_ignore=false) {} \
	else for (stmt; !inlinegt_ignore; inlinegt_ignore=true)

#define doafter(stmt) \
	for(int inlinegt_after=0;inlinegt_after<=1;inlinegt_after++) \
	if (inlinegt_after) { stmt; } \
	else

#define maxpick(state,loopdef) \
	addstmt(auto inlinegt_oldalpha = state.alpha COMMA myret = state.alpha) \
	doafter(state.setreturn(myret,inlinegt_oldalpha,true)) \
	for(bool inlinegt_break=false;!inlinegt_break;inlinegt_break=true) \
		if (inlinegt_break) {} \
		else for(loopdef) \
			if (inlinegt_break) break; \
			else doafter(inlinegt_break = state.update(myret,true)) \

#define minpick(state,loopdef) \
	addstmt(auto inlinegt_oldbeta = state.beta COMMA myret = state.beta) \
	doafter(state.setreturn(myret,inlinegt_oldbeta,false)) \
	for(bool inlinegt_break=false;!inlinegt_break;inlinegt_break=true) \
		if (inlinegt_break) {} \
		else for(loopdef) \
			if (inlinegt_break) break; \
			else doafter(inlinegt_break = state.update(myret,false)) \


#define inlinegt_localize1(varname) \
	addstmt(auto inlinegt_save = varname) \
	doafter(varname = inlinegt_save)

#define inlinegt_localize2(varname,...) \
	addstmt(auto inlinegt_save = varname) \
	doafter(varname = inlinegt_save) \
	inlinegt_localize1(__VA_ARGS__)

#define inlinegt_localize3(varname,...) \
	addstmt(auto inlinegt_save = varname) \
	doafter(varname = inlinegt_save) \
	inlinegt_localize2(__VA_ARGS__)

#define inlinegt_localize4(varname,...) \
	addstmt(auto inlinegt_save = varname) \
	doafter(varname = inlinegt_save) \
	inlinegt_localize3(__VA_ARGS__)

#define inlinegt_localize5(varname,...) \
	addstmt(auto inlinegt_save = varname) \
	doafter(varname = inlinegt_save) \
	inlinegt_localize4(__VA_ARGS__)

#define localize(...) VFUNC(inlinegt_localize, __VA_ARGS__)
	
#define inlinegt_randpick2(state,loopdef) \
	addstmt(decltype(state.alpha) inlinegt_sum)\
	addstmt(int inlinegt_count = 0)\
	addstmt(bool havereturn=false)\
	doafter(state.setreturn(inlinegt_sum/inlinegt_count))\
	for(loopdef) \
		doafter(state.updateexp(inlinegt_sum,inlinegt_count))

#define inlinegt_randpick3(state,n,loopdef) \
	addstmt(decltype(state.alpha) inlinegt_sum)\
	addstmt(int inlinegt_count = 0)\
	addstmt(bool havereturn=false)\
	doafter(state.setreturn(inlinegt_sum/inlinegt_count))\
	for(bool inlinegt_break=false;!inlinegt_break;inlinegt_break=true) \
		if (inlinegt_break) {} \
		else for(loopdef) \
			if (inlinegt_break) break; \
			else doafter(inlinegt_break = state.updateexp(inlinegt_sum,inlinegt_count,n))

#define randpick(...) VFUNC(inlinegt_randpick, __VA_ARGS__)
#endif
