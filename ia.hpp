#ifndef IA_HPP
#define IA_HPP

#include <vector>
#include <array>
#include <functional>
#include "inlinegt.hpp"

namespace ia {

struct offres {
	int surges,hits,range, pierce;
	offres(int s=0, int h=0, int r=0, int p=0) : surges(s), hits(h), range(r), pierce(p) {}

	offres &operator+=(const offres &r) {
		surges += r.surges;
		hits += r.hits;
		range += r.range;
		pierce += r.pierce;
		return *this;
	}

	offres &operator-=(const offres &r) {
		surges -= r.surges;
		hits -= r.hits;
		range -= r.range;
		pierce -= r.pierce;
		return *this;
	}

	offres operator+(const offres &r) const {
		offres ret(*this);
		return ret+=r;
	}

	offres operator-(const offres &r) {
		offres ret(*this);
		return ret-=r;
	}

	bool valid() const {
		return surges>=0 && hits>=0 && range>=0 && pierce>=0;
	}
};

ostream &operator<<(ostream &os, const offres &a) {
	return os << '(' << a.surges << ',' << a.hits << ',' << a.range << ',' << a.pierce << ')';
}

struct defres {
	int evades,blocks,dodges;
	defres(int e=0, int b=0, int d=0) : evades(e), blocks(b), dodges(d) {}
	defres &operator+=(const defres &r) {
		evades += r.evades;
		blocks += r.blocks;
		dodges += r.dodges;
		return *this;
	}

	defres &operator-=(const defres &r) {
		evades -= r.evades;
		blocks -= r.blocks;
		dodges -= r.dodges;
		return *this;
	}

	defres operator+(const defres &r) const {
		defres ret(*this);
		return ret+=r;
	}

	defres operator-(const defres &r) {
		defres ret(*this);
		return ret-=r;
	}

	bool valid() const {
		return evades>=0 && blocks>=0 && dodges>=0;
	}
};


ostream &operator<<(ostream &os, const defres &d) {
	return os << '(' << d.evades << ',' << d.blocks << ',' << d.dodges << ')';
}


template<typename FACET, size_t NFACE>
using die = std::array<FACET,NFACE>;

using offdie = die<offres,6>;
using defdie = die<defres,6>;

constexpr offdie red { {{0,1,0},{1,2,0},{0,2,0},{0,2,0},{0,3,0},{0,3,0}} };
constexpr offdie yellow { {{2,1,0},{1,0,0},{1,0,2},{0,1,2},{1,1,1},{0,2,1}} };
constexpr offdie green { {{0,2,1},{1,1,1},{1,0,1},{1,1,2},{0,2,2},{0,2,3}} };
constexpr offdie blue { {{1,0,2},{0,1,2},{1,1,3},{0,2,3},{0,2,4},{0,1,5}} };

constexpr defdie black { {{1,0,0},{0,1,0},{0,1,0},{0,2,0},{0,2,0},{0,3,0}} };
constexpr defdie white { {{0,0,0},{1,0,0},{0,1,0},{1,1,0},{1,1,0},{0,0,1}} };

template<typename DTYPE>
struct diepool {
	using dieT = DTYPE;
	using FACET = decltype(std::declvar<DTYPE>()[0]);
	std::vector<dieT> dice;

	template<typename... T>
	diepool(T &&...ds) : dice{forward<T>(ds)...} {}
	
	struct iterator {
		std::vector<int> sidenum;
		bool done; // needed for the "no dice" case
		const std::vector<dieT> &dice;

		FACET value;

		iterator(const std::vector<dieT> &ds)
				: sidenum(ds.size(),0), dice(ds), done(false) {
			for(auto d : ds) value += d[0];
		}

		const FACET &operator*() const {
			return value;
		}

		const FACET &operator->() const {
			return value;
		}

		iterator &operator++() {
			for(int i=0;i<sidenum.size();i++) {
				value -= dice[i][sidenum[i]];
				if (++(sidenum[i]) < dice[i].size()) {
					value += dice[i][sidenum[i]];
					return *this;
				}
				sidenum[i] = 0;
				value += dice[i][sidenum[i]];
			}
			done = true;
			return *this;
		}

		iterator operator++(int) {
			iterator ret(*this);
			++(*this);
			return ret;
		}

		bool operator==(const iterator &it) const {
			if (done != it.done) return false;
			if (done && it.done) return true;
			return sidenum==it.sidenum;
		}
		bool operator!=(const iterator &it) const {
			return !(*this==it);
		}
	};

	iterator begin() const {
		iterator ret(dice);
		return ret;
	}
	iterator end() const {
		iterator ret(dice);
		ret.done = true;
		return ret;
	}
};

struct attstatus {
	diepool<offdie> offdice;
	diepool<defdie> defdice;

	bool melee;
	int range;

	std::vector<offres> offroll;
	std::vector<bool> offrerolled;
	std::vector<defres> defroll;
	std::vector<bool> defrerolled;

	offroll totaloff;
	defroll totaldef;

	bool missed;
	int offdmg,offstrain;
	int defdmg,defstrain;

	template<typename DTYPE>
	struct access {
	};

	template<>
	struct access<offdie> {
		template<typename A>
		static auto dice(A &&as) const {
			return std::forward<A>(as).offdice;
		}
		template<typename A>
		static auto roll(A &&as) const {
			return std::forward<A>(as).offroll;
		}
		template<typename A>
		static auto rerolled(A &&as) const {
			return std::forward<A>(as).offrerolled;
		}
		template<typename A>
		static auto total(A &&as) const {
			return std::forward<A>(as).totaloff;
		}
		template<typename A>
		static auto dmg(A &&as) const {
			return std::forward<A>(as).offdmg;
		}
		template<typename A>
		static auto strain(A &&as) const {
			return std::forward<A>(as).offstrain;
		}
	};

	template<>
	struct access<defdie> {
		template<typename A>
		static auto dice(A &&as) const {
			return std::forward<A>(as).defdice;
		}
		template<typename A>
		static auto roll(A &&as) const {
			return std::forward<A>(as).defroll;
		}
		template<typename A>
		static auto rerolled(A &&as) const {
			return std::forward<A>(as).defrerolled;
		}
		template<typename A>
		static auto total(A &&as) const {
			return std::forward<A>(as).totaldef;
		}
		template<typename A>
		static auto dmg(A &&as) const {
			return std::forward<A>(as).defdmg;
		}
		template<typename A>
		static auto strain(A &&as) const {
			return std::forward<A>(as).defstrain;
		}
	};

	template<typename DTYPE>
	auto dice() const { return access::dice(*this); }
	template<typename DTYPE>
	auto dice() { return access::dice(*this); }
	template<typename DTYPE>
	auto roll() const { return access::roll(*this); }
	template<typename DTYPE>
	auto roll() { return access::roll(*this); }
	template<typename DTYPE>
	auto rerolled() const { return access::rerolled(*this); }
	template<typename DTYPE>
	auto rerolled() { return access::rerolled(*this); }
	template<typename DTYPE>
	auto total() const { return access::total(*this); }
	template<typename DTYPE>
	auto total() { return access::total(*this); }
	template<typename DTYPE>
	auto dmg() const { return access::dmg(*this); }
	template<typename DTYPE>
	auto dmg() { return access::dmg(*this); }
	template<typename DTYPE>
	auto strain() const { return access::strain(*this); }
	template<typename DTYPE>
	auto strain() { return access::strain(*this); }
		
};

enum struct chooser { none=0, max=1, min=2, rand=3 };

template<typename V>
struct attability {
	using sstate = inlinegt::sstate<V>;
	using contfn = std::function<void()>;
	chooser who;
	std::function<bool(const attstatus &)> enabled;
	std::function<void(sstate &,attstatus &,contfn)> exec;
	template<typename E, typename X>
	constexpr attability(chooser whopicks, E &&useable, X &&doit)
		: who(whopicks),
		  enabled(std::forward<E>(useable)),
		  exec(std::forward<X>(doit)) {}

	void exec(sstate &ss, attstatus &as, contfn cont) {
		if (!enabled(as)) {
			cont();
			return;
		}
		switch (who) {
			case chooser::none: {
				exec(ss,as,cont);
			} break;
			case chooser::max {
				maxpick(ss,int use = 0;use<2;use++) {
					if (use) exec(as,cont);
					else cont();
				}
			} break;
			case chooser::min {
				minpick(ss,int use = 0;use<2;use++) {
					if (use) exec(as,cont);
					else cont();
				}
			} break;
			case chooser::rand { // strange... probably never used
				randpick(ss,int use = 0;use<2;use++) {
					if (use) exec(as,cont);
					else cont();
				}
			} break;
		}
	}
};

template<typename DTYPE,typename V=double>
constexpr attability<V> rerollany(chooser who) {
	struct reroll {
		chooser who;
		constexpr reroll(chooser w) : who(w) {}
		constexpr bool possible(const attstatus &as) const {
			for(auto b : as.rerolled<DTYPE>())
				if (!b) return true;
			return false;
		}

		constexpr void doit(attability<V>::sstate &ss, attstatus &as, attability<V>::contfn f,int i) const {
			as.rerolled<DTYPE>()[i] = true;
			auto saved = as.roll<DTYPE>()[i];
			randpick(ss,auto newd : as.dice<DTYPE>()[i]) {
				as.roll<DTYPE>()[i] = newd;
				f();
			}
			as.roll<DTYPE>()[i] = saved;
			as.rerolled<DTYPE>()[i] = false;
		}

		constexpr void exec(attability<V>::sstate &ss, attstatus &as, attability<V>::contfn f) const {
			switch(who) {
				case chooser::none: { // what?  perhaps only 1 die?  pick first
					for(int i=0;i<as.rerolled<DTYPE>().size();i++)
						if (!as.rerolled<DTYPE>()[i]) doit(ss,as,f,i);
				} break;
				case chooser::max: {
					maxpick(ss,int i=0;i<2;i++)
						if (!i) f();
						else maxpick(ss,int d=0;d<as.rerolled<DTYPE>().size();d++)
							if (!as.rerolled<DTYPE()[i]) doit(ss,as,f,i);
				} break;
				case chooser::min: {
					maxpick(ss,int i=0;i<2;i++)
						if (!i) f();
						else minpick(ss,int d=0;d<as.rerolled<DTYPE>().size();d++)
							if (!as.rerolled<DTYPE>()[i]) doit(ss,as,f,i);
				} break;
				case chooser::rand: { // strange but I guess possible
					randpick(ss,int d=0;d<as.rerolled<DTYPE>().size();d++)
						if (!as.rerolled<DTYPE>()[i]) doit(ss,as,f,i);
				} break;
			}
		}
	};

	reroll abl(who);
	return attability<V>(who,alb.possible,abl.exec);
}

template<typename DTYPE,typename V=double>
constexpr attability<V> rerollall(chooser who) {
	struct reroll {
		chooser who;
		constexpr reroll(chooser w) : who(w) {}
		constexpr bool possible(const attstatus &as) const {
			for(auto b : as.rerolled<DTYPE>())
				if (b) return false;
			return true;
		}

		constexpr void doit(attability<V>::sstate &ss, attstatus &as, attability<V>::contfn f) const {
			for(bool &b : as.rerolled<DTYPE>()) b = true;
			auto saved = as.roll<DTYPE>();
			randpick(ss,auto newroll : as.dice<DTYPE>()) {
				as.roll<DTYPE>() = newroll;
				f();
			}
			as.roll<DTYPE>() = saved;
			for(bool &b : as.rerolled<DTYPE>()) b = false;
		}

		constexpr void exec(attability<V>::sstate &ss, attstatus &as, attability<V>::contfn f) const {
			switch(who) {
				case chooser::none: { // what?  perhaps only 1 die?  pick first
					doit(ss,as,f);
				} break;
				case chooser::max: {
					maxpick(int i =0;i<2;i++)
						if (!i) f();
						else doit(ss,as,f,i);
				} break;
				case chooser::min: {
					minpick(int i =0;i<2;i++)
						if (!i) f();
						else doit(ss,as,f,i);
				} break;
				case chooser::rand: { // very strange but I guess possible
					randpick(int i =0;i<2;i++)
						if (!i) f();
						else doit(ss,as,f,i);
				} break;
			}
		}
	};

	reroll abl(who);
	return attability<V>(who,alb.possible,abl.exec);
}
					
template<typename V, typename... S>
constexpr attability<V> dietotalmod(chooser who, S... s) {
	struct mod {
		offres offmod;
		defres defmod;
		constexpr mod(offres offdelta, defres defdelta)
			: offmod(offdelta), defmod(defdelta) {}
		constexpr attmod(int surges, int hits, int range, int pierce,
				int evades, int blocks, int dodges)
			: offmod(surges,hits,range,pierce), defmod(evades,blocks,dodges) {}

		constexpr bool possible(const attstatus &as) const {
			return (as.totaloff+offmod).valid() && (as.totaldef+defmod).valid();
		}
		constexpr bool exec(attability<V>::sstate &ss, abilityattstatus &as, attability<V>::contfn f) const {
			as.totaloff += offmod;
			as.totaldef += defmod;
			f();
			as.totaloff -= offmod;
			as.totaldef -= defmod;
		}
	};

	mod abl(s...);
	return ability<V>(who,abl.possible,abl.exec);
}

template<int NDMG=1, int NSURGE=1, typename V=double>
constexpr surgedmg = dietotalmod<V>(chooser::max,-NSURGE,NDMG,0,0,0,0,0);
template<int RANGE=1, int NSURGE=1, typename V=double>
constexpr surgerange = dietotalmod<V>(chooser::max,-NSURGE,0,RANGE,0,0,0,0);
template<int NPIERCE=1, int NSURGE=1, typename V=double>
constexpr surgepierce = dietotalmod<V>(chooser::max,-NSURGE,0,0,NPIERCE,0,0,0);

template<int NBLOCK, int NEVADE, typename V=double>
constexpr blocktoevade = dietotalmod<V>(chooser::min,0,0,0,0,NEVADE,-NBLOCK,0);
template<int NEVADE, int NBLOCK, typename V=double>
constexpr evadetoblock = dietotalmod<V>(chooser::min,0,0,0,0,-NEVADE,NBLOCK,0);

template<chooser WHO=chooser::max,typename V=double>
constexpr reroll1off = rerollany<offdie,V>(WHO);
template<chooser WHO=chooser::min,typename V=double>
constexpr reroll1def = rerollany<defdie,V>(WHO);

template<chooser WHO=chooser::max,typename V=double>
constexpr rerollalloff = rerollall<offdie,V>(WHO);
template<chooser WHO=chooser::min,typename V=double>
constexpr rerollalldef = rerollall<defdie,V>(WHO);



}
