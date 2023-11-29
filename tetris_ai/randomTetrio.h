#pragma once
#include <cmath>
// this is how tetrio generate new bag of minos
namespace AI {
	class RandomTetrio {
	public:
		unsigned s;
	public:
		RandomTetrio(unsigned seed = 0) {
			setSeed(seed);
		}
		void setSeed(unsigned seed) {
			s = seed % 2147483647;
		}
		unsigned next() {
			long long temp = s;
			s = (16807 * temp) % 2147483647;
			return s;
		}
		double nextFloat() {
			double tmp = next() - 1.0;
			return tmp / 2147483646.0;
		}
		void shuffleArray(int(&m)[7]) {
			int tmp1, len = 7;
			while (--len) {
				tmp1 = (int)floor(nextFloat() * (len + 1));
				int tmp = m[tmp1];
				m[tmp1] = m[len];
				m[len] = tmp;
			}
			return;
		}
		unsigned getCurrentSeed() {
			return s;
		}
	};

}
