#ifndef IA_HPP
#define IA_HPP

#include <vector>
#include <array>

namespace ia {

struct attackresult {
	int surges,hits,range;
	attackresult(int s=0, int h=0, int r=0) : surges(s), hits(h), range(r) {}

	attackresult &operator+=(const attackresult &r) {
		surges += r.surges;
		hits += r.hits;
		range += r.range;
		return *this;
	}

	attackresult &operator-=(const attackresult &r) {
		surges -= r.surges;
		hits -= r.hits;
		range -= r.range;
		return *this;
	}

	attackresult operator+(const attackresult &r) const {
		attackresult ret(*this);
		return ret+=r;
	}

	attackresult operator-(const attackresult &r) {
		attackresult ret(*this);
		return ret-=r;
	}

	bool valid() const {
		return surges>=0 && hits>=0 && range>=0;
	}
};

ostream &operator<<(ostream &os, const attackresult &a) {
	return os << '(' << a.surges << ',' << a.hits << ',' << a.range << ')';
}

struct defenseresult {
	int evades,blocks,dodges;
	defenseresult(int e=0, int b=0, int d=0) : evades(e), blocks(b), dodges(d) {}
	defenseresult &operator+=(const defenseresult &r) {
		evades += r.evades;
		blocks += r.blocks;
		dodges += r.dodges;
		return *this;
	}

	defenseresult &operator-=(const defenseresult &r) {
		evades -= r.evades;
		blocks -= r.blocks;
		dodges -= r.dodges;
		return *this;
	}

	defenseresult operator+(const defenseresult &r) const {
		defenseresult ret(*this);
		return ret+=r;
	}

	defenseresult operator-(const defenseresult &r) {
		defenseresult ret(*this);
		return ret-=r;
	}

	bool valid() const {
		return evades>=0 && blocks>=0 && dodges>=0;
	}
};


ostream &operator<<(ostream &os, const defenseresult &d) {
	return os << '(' << d.evades << ',' << d.blocks << ',' << d.dodges << ')';
}


template<typename FACET, size_t NFACE>
using die = std::array<FACET,NFACE>;

using attdie = die<attackresult,6>;
using defdie = die<defenseresult,6>;

constexpr attdie red
			{ {{0,1,0},{1,2,0},{0,2,0},{0,2,0},{0,3,0},{0,3,0}} };
constexpr attdie yellow
			{ {{2,1,0},{1,0,0},{1,0,2},{0,1,2},{1,1,1},{0,2,1}} };
constexpr attdie green
			{ {{0,2,1},{1,1,1},{1,0,1},{1,1,2},{0,2,2},{0,2,3}} };
constexpr attdie blue
			{ {{1,0,2},{0,1,2},{1,1,3},{0,2,3},{0,2,4},{0,1,5}} };

constexpr defdie black
			{ {{1,0,0},{0,1,0},{0,1,0},{0,2,0},{0,2,0},{0,3,0}} };
constexpr defdie white
			{ {{0,0,0},{1,0,0},{0,1,0},{1,1,0},{1,1,0},{0,0,1}} };

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

struct attackstatus {
	diepool<attdie> attdice;
	diepool<defdie> defdice;

	bool melee;
	int range;

	std::vector<attackresult> attroll;
	std::vector<defenseresult> defroll;

	bool missed;
	int defdmg,defstrain;
	int attdmg,attstrain;
};

}
