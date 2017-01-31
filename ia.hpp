#ifndef IA_HPP
#define IA_HPP

#include <vector>
#include <array>
#include <functional>
#include "inlinegt.hpp"

namespace ia {

struct offres {
	int surges,hits,range,pierces;
	offres(int s=0, int h=0, int r=0, int p=0) : surges(s), hits(h), range(r), pierces(p) {}

	offres &operator+=(const offres &r) {
		surges += r.surges;
		hits += r.hits;
		range += r.range;
		pierces += r.pierces;
		return *this;
	}

	offres &operator-=(const offres &r) {
		surges -= r.surges;
		hits -= r.hits;
		range -= r.range;
		pierces -= r.pierces;
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
		return surges>=0 && hits>=0 && range>=0 && pierces>=0;
	}
};

ostream &operator<<(ostream &os, const offres &a) {
	return os << '(' << a.surges << ',' << a.hits << ',' << a.range << ',' << a.pierces << ')';
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

struct attstatus;

enum struct chooser { none=0, off=1, def=2, rand=3 };

template<typename V>
struct attability {
	using sstate = inlinegt::sstate<V>;
	using contfn = std::function<void()>;

	chooser who;
	std::function<bool(const attstatus &)> enabled;
	std::function<void(sstate &,attstatus &,contfn)> execabil;

	template<typename E, typename X>
	constexpr attability(chooser whopicks, E &&useable, X &&doit)
		: who(whopicks),
		  enabled(std::forward<E>(useable)),
		  execabil::forward<X>(doit)) {}

	void exec(sstate &ss, attstatus &as, contfn cont) {
		if (!enabled(as)) {
			cont();
			return;
		}
		switch (who) {
			case chooser::none: {
				execabilas,cont);
			} break;
			case chooser::off
			case chooser::def {
				if ((who==chooser::off)==as.offmax) {
					maxpick(ss,int use = 0;use<2;use++)
						if (use) execabilcont);
						else cont();
				} else {
					minpick(ss,int use = 0;use<2;use++)
						if (use) execabilcont);
						else cont();
				}
			} break;
			case chooser::rand { // strange... probably never used
				randpick(ss,int use = 0;use<2;use++) {
					if (use) execabilcont);
					else cont();
				}
			} break;
		}
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

	void setdice(std::vector<offres> oroll, std::vector<defres> droll) {
		offroll = std::move(oroll);
		offrerolled.resize(offroll.size());
		std::fill(offrerolled.begin(),offrerolled.end(),false);
		defroll = std::move(droll);
		defrerolled.resize(defroll.size());
		std::fill(defrerolled.begin(),defrerolled.end(),false);
	}

	offroll totaloff;
	defroll totaldef;

	void sumup() {
		totaloff = offres();
		for(auto &r : offroll)
			totaloff += r;
		totaldef = defres();
		for(auto &r : defroll)
			totaldef += r;
	}

	bool missed;
	int offdmg=0,offstrain=0;
	int defdmg=0,defstrain=0;

	void resolve() {
		// if melee, assume that reach is sufficient
		missed = (!melee && range>totaloff.range) || totaldef.dodge>0;
		if (!missed) {
			int totalblk = std::max(0,totaldef.blocks - totaloff.pierces);
			defdmg += std::max(0,totaloff.hits - totalblk);
		}
	}

	bool offmax=true;

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


template<typename SS, typename A1, typename A2, typename F>
void exelst(SS &ss, attstatus &as, A1 &&b, A2 &&e,F f) {
	if (b==e) f();
	else b->exec(ss,as,[&]() { exelst(ss,as,b+1,e,f) });
}
template<typename SS, typename R, typename F>
void exelst(SS &ss, attstatus &as, R &&r,F f) {
	exelst(ss,as,begin(r),end(r),std::move(f));
}

template<typename SS, typename F>
void exelsts(SS &ss, attstatus &as, F f) {
	f();
}

template<typename SS, typename F, typename L1, typename... Ls>
void exelsts(SS &ss, attstatus &as, F f, L1 &&l1, Ls &&...ls) {
	exelst(ss,as,std::forward<L1>(l1),[&]() {
			exelsts(ss,as,f,std::forward<Ls>(ls)...);
	});
}

template<typename V=double>
struct attablset {
	using abllst = std::vector<attability<V>>;
	abllst preroll,postroll,postsum,postatt;
	// there are a number of other nuanced difference I'm ignoring for now
};

template<typename V=double>
struct weapon {
	diepool<offdie> offdice;
	attablset<V> abls;
};

template<typename V=double>
struct figure {
	std::vector<weapon<V>> weapons;
	diepool<defdie> defdice;

	int health,strainleft;

	attablset<V> offabls;
	attablset<V> defabls;
};

template<typename F, typename V> // In this case, F should return the value (assume offense to maximize) -- it will be passed nothing
V resolveattack(figure<V> &offfig, figure<V> &deffig, int range, bool melee, F f) {
	attstatus as;
	inlinegt::sstate<V> ss;
	as.melee = melee;
	as.range = range;
	as.defdice = deffig.defdice;
	maxpick(ss,auto &w : offfig.weapons) {
		as.offdice = w.offdice;
		exelsts(ss,as,[&]() {
			randpick(auto &oroll : as.offdice)
				randpick(auto &droll : as.defdice) {
					as.setdice(oroll,droll);
					exelsts(ss,as,[&]() {
						as.sumup();
						exelsts(ss,as,[&]() {
							as.resolve();
							int oh=offfig.health,os=offfig.strainleft;
							int dh=deffig.health,ds=deffig.strainleft;

							int offs = min(strain,as.offstrain);
							offfig.strainleft -= offs;
							offfig.health =
								max(0,offfig.health-as.offdmg
									-(as.offstrain-offs));

							int defs = min(strain,as.defstrain);
							deffig.strainleft -= defs;
							deffig.health =
								max(0,deffig.health-as.defdmg
									-(as.defstrain-defs));
							as.set(f());

							offfig.health=oh; offfig.strainleft=os;
							deffig.health=dh; deffig.strainleft=ds;
						},
						w.albs.postsum,offfig.offalbs.postsum,deffig.defabls.postsum);
					},
					w.albs.postroll,offfig.offalbs.postroll,deffig.defabls.postroll);
				}
		},
		w.albs.preroll,offfig.offalbs.preroll,deffig.defabls.preroll);
	}
}



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
				case chooser::off
				case chooser::def {
					if ((who==chooser::off)==as.offmax) {
						maxpick(ss,int d=0;d<as.rerolled<DTYPE>().size();d++)
							if (!as.rerolled<DTYPE()[i]) doit(ss,as,f,i);
					} else {
						minpick(ss,int d=0;d<as.rerolled<DTYPE>().size();d++)
							if (!as.rerolled<DTYPE>()[i]) doit(ss,as,f,i);
					}
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
		constexpr bool possible(const attstatus &as) const {
			for(auto b : as.rerolled<DTYPE>())
				if (b) return false;
			return true;
		}

		constexpr void exec(attability<V>::sstate &ss, attstatus &as, attability<V>::contfn f) const {
			for(bool &b : as.rerolled<DTYPE>()) b = true;
			auto saved = as.roll<DTYPE>();
			randpick(ss,auto newroll : as.dice<DTYPE>()) {
				as.roll<DTYPE>() = newroll;
				f();
			}
			as.roll<DTYPE>() = saved;
			for(bool &b : as.rerolled<DTYPE>()) b = false;
		}
	};

	reroll abl;
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
constexpr surgedmg = dietotalmod<V>(chooser::off,-NSURGE,NDMG,0,0,0,0,0);
template<int RANGE=1, int NSURGE=1, typename V=double>
constexpr surgerange = dietotalmod<V>(chooser::off,-NSURGE,0,RANGE,0,0,0,0);
template<int NPIERCE=1, int NSURGE=1, typename V=double>
constexpr surgepierce = dietotalmod<V>(chooser::off,-NSURGE,0,0,NPIERCE,0,0,0);

template<int NBLOCK, int NEVADE, typename V=double>
constexpr blocktoevade = dietotalmod<V>(chooser::def,0,0,0,0,NEVADE,-NBLOCK,0);
template<int NEVADE, int NBLOCK, typename V=double>
constexpr evadetoblock = dietotalmod<V>(chooser::def,0,0,0,0,-NEVADE,NBLOCK,0);

template<chooser WHO=chooser::off,typename V=double>
constexpr reroll1off = rerollany<offdie,V>(WHO);
template<chooser WHO=chooser::def,typename V=double>
constexpr reroll1def = rerollany<defdie,V>(WHO);

template<chooser WHO=chooser::off,typename V=double>
constexpr rerollalloff = rerollall<offdie,V>(WHO);
template<chooser WHO=chooser::def,typename V=double>
constexpr rerollalldef = rerollall<defdie,V>(WHO);

}
