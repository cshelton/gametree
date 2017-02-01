#ifndef FIGURES_HPP
#define FIGURES_HPP

#include "ia.hpp"

namespace ia {

// helpers

template<typename T, typename... Ts>
std::vector<T> tovec(T e1, Ts... es) {
	return std::vector<T>(std::move(e1),std::move(es)...);
}

template<typename V>
attablset<V> noabls() {
	return attablset<V>{attlst<V>{},attlst<V>{},attlst<V>[],attlst<V>{}};
}


// weapons
template<typename V>
weapon hairtriggerpistol() {
	return weapon<V>{dicepool<offdice>{blue,yellow},
		attablset<V>{
			attlst<V>{},
			attlst<V>{rerollalloff(chooser::off)},
			attlst<V>{surgedmg<1,1,V>,surgerange<2,1,V>},
			attlst<V>{}}};
}

template<typename V>
weapon<V> trooperblaster() {
	return weapon<V>{dicepool<offdice>{blue,green},noabls<V>()};
}

// non-heros

template<typename V>
figure<V> stormtrooper(bool neartrooper=false) {
	attlst<V> rerolls;
	if (neartrooper) rerolls.push_back(rerollanyoff(chooser::off));
	return figure<V>{3,0,4,
		attablset<V>{attlst<V>{},rerolls,
			attlst<V>{surgedmg<1,1,V>,surgerange<2,1,V>},
			attlst<V>{}},
		noabls<V>(),
		diepool<defdie>{black},
		tovec(trooperblaster())};
}

// heros

template<typename V>
figure<V> vinto_hero() {
	return figure<V>{11,4,4,
		noabls<V>(),noabls<V>(),
		diepool<defdie>{white},
		tovec(hairtriggerpistol())};
}



#endif
