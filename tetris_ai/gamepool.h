#pragma once
#define _ALLOW_ITERATOR_DEBUG_LEVEL_MISMATCH

#include <string.h>
#include "tetris_gem.h"
#include "tetris_setting.h"
#include <cmath>
#include <algorithm>
#ifdef XP_RELEASE
#define AI_POOL_MAX_H 50
#else
#define AI_POOL_MAX_H 32
#endif
#define log1p(x) log(1+(x))

namespace AI {
    struct GameField;
    Gem& getGem( int number, int spin );
    int getComboAttack( int combo );
    void setAllSpin(bool allSpin);
    bool isEnableAllSpin();
    void setLockOut(bool enable);
    bool isLockOutEnable();
    void setSoftdrop( bool softdrop );
    bool softdropEnable();
    typedef __int64 uint64;
    void InitHashTable();
    uint64 hash(const GameField & pool);
#ifdef XP_RELEASE
    const int gem_add_y = 20;
#else
    const int gem_add_y = 6;
#endif
    const int gem_beg_x = 3;
    const int gem_beg_y = 0;

    struct GameField {
        signed char m_w, m_h;
        signed short combo;
        signed char b2b;
        int x_before_spin;
        int y_before_spin;
        signed char spin_dir;
        unsigned long m_w_mask;
        unsigned long m_row[AI_POOL_MAX_H];
        int m_hold;
        int m_pc_att;
        uint64 hashval;
        unsigned long *row;
        GameField () {
            row = &m_row[gem_add_y];
        }
        GameField ( const GameField& field ) {
            row = &m_row[gem_add_y];
            *this = field;
        }
        GameField (signed char w, signed char h) {
            row = &m_row[gem_add_y];
            reset(w, h);
        }
        int width() const { return m_w; }
        int height() const { return m_h; }
        void reset (signed char w, signed char h) {
            m_w = w;
            m_h = h;
            combo = 0;
            b2b = 0;
            m_hold = 0;
            m_w_mask = ( 1 << w ) - 1;
            for (int i = 0; i < AI_POOL_MAX_H; ++i) {
                m_row[i] = 0;
            }
            for (int i = gem_add_y + m_h + 1; i < AI_POOL_MAX_H; ++i) {
                m_row[i] = (unsigned)-1;
            }
        }
        GameField& operator = (const GameField& field) {
            memcpy( this, &field, (size_t)&row - (size_t)this );
            row = m_row + ( field.row - field.m_row );
            return *this;
        }
        bool operator == (const GameField& field) const {
            if ( m_w != field.m_w || m_h != field.m_h ) return false;
            if ( m_hold != field.m_hold ) return false;
            if ( combo != field.combo ) return false;
            if ( b2b != field.b2b ) return false;
            if ( row - m_row != field.row - field.m_row ) return false;
            for ( int i = 0; i <= m_h + gem_add_y; ++i ) {
                if ( m_row[i] != field.m_row[i] ) return false;
            }
            return true;
        }
        //__forceinline
        inline
        bool isCollide(int y, const Gem & gem) const {
            for ( int h = 3; h >= 0; --h ) {
                if ( y + h > m_h && gem.bitmap[h] ) return true;
                if ( row[y + h] & gem.bitmap[h] ) return true;
            }
            return false;
        }
        //__forceinline
        inline
        bool isCollide(int x, int y, const Gem & gem) const {
            Gem _gem = gem;
            for ( int h = 0; h < 4; ++h ) {
                if ( x < 0 ) {
                    if (gem.bitmap[h] & ( ( 1 << (-x) ) - 1 ) ) return true;
                    _gem.bitmap[h] >>= -x;
                } else {
                    if ( (gem.bitmap[h] << x) & ~m_w_mask ) return true;
                    _gem.bitmap[h] <<= x;
                }
                if ( y + h > m_h && _gem.bitmap[h] ) return true;
                if ( row[y + h] & _gem.bitmap[h] ) return true;
            }
            return false; //isCollide(y, _gem);
        }
        void reportXYRCoord(int x, int y, int r) {
            x_before_spin = x;
            y_before_spin = y;
            spin_dir = r;
        }
        bool wallkickTest(int& x, int& y, const Gem & gem, int spinclockwise) const {
            static int Iwallkickdata[4][2][4][2] = {
                { // O
                    { // R
                        { 2, 0},{-1, 0},{ 2,-1},{-1, 2},
                    },
                    { // L
                        { 1, 0},{-2, 0},{ 1, 2},{-2,-1},
                    },
                },
                { // L
                    { // O
                        {-1, 0},{ 2, 0},{-1,-2},{ 2, 1},
                    },
                    { // 2
                        { 2, 0},{-1, 0},{ 2,-1},{-1, 2},
                    },
                },
                { // 2
                    { // L
                        {-2, 0},{ 1, 0},{-2, 1},{ 1,-2},
                    },
                    { // R
                        {-1, 0},{ 2, 0},{-1,-2},{ 2, 1},
                    },
                },
                { // R
                    { // 2
                        { 1, 0},{-2, 0},{ 1, 2},{-2,-1},
                    },
                    { // O
                        {-2, 0},{ 1, 0},{-2, 1},{ 1,-2},
                    },
                },
            };
            static int wallkickdata[4][2][4][2] = {
                { // O
                    { // R
                        { 1, 0},{ 1, 1},{ 0,-2},{ 1,-2},
                    },
                    { // L
                        {-1, 0},{-1, 1},{ 0,-2},{-1,-2},
                    },
                },
                { // L
                    { // O
                        { 1, 0},{ 1,-1},{ 0, 2},{ 1, 2},
                    },
                    { // 2
                        { 1, 0},{ 1,-1},{ 0, 2},{ 1, 2},
                    },
                },
                { // 2
                    { // L
                        {-1, 0},{-1, 1},{ 0,-2},{-1,-2},
                    },
                    { // R
                        { 1, 0},{ 1, 1},{ 0,-2},{ 1,-2},
                    },
                },
                { // R
                    { // 2
                        {-1, 0},{-1,-1},{ 0, 2},{-1, 2},
                    },
                    { // O
                        {-1, 0},{-1,-1},{ 0, 2},{-1, 2},
                    },
                },
            };
            int (*pdata)[2][4][2] = wallkickdata;
            if ( gem.num == 1 ) pdata = Iwallkickdata;
            for ( int itest = 0; itest < 4; ++itest) {
                int dx = x + pdata[gem.spin][spinclockwise][itest][0];
                int dy = y + pdata[gem.spin][spinclockwise][itest][1];
                if ( ! isCollide(dx, dy, gem) ) {
                    x = dx; y = dy;
                    return true;
                }
            }
            return false;
        }
        void paste(int x, int y, const Gem & gem) {
            for ( int h = 0; h < gem.geth(); ++h ) {
                if (x >= 0)
                    row[y + h] |= gem.bitmap[h] << x;
                else
                    row[y + h] |= gem.bitmap[h] >> (-x);
            }
        }
        signed char isWallKickSpin(int x, int y, const Gem & gem) const {
            if ( isEnableAllSpin() ) {
                if ( isCollide( x - 1, y, gem )
                    && isCollide( x + 1, y, gem )
                    && isCollide( x, y - 1, gem )) {
                        return 1;
                }
            } else {
                if ( gem.num == 2 ) { //T
                    int cnt = 0;
                    if ( x < 0 || (row[y] & (1 << x))) ++cnt;
                    if ( x < 0 || y+2 > m_h || (row[y+2] & (1 << x))) ++cnt;
                    if ( x+2 >= m_w || (row[y] & (1 << (x+2)))) ++cnt;
                    if ( x+2 >= m_w || y+2 > m_h || (row[y+2] & (1 << (x+2)))) ++cnt;
                    if ( cnt >= 3 ) return cnt;
                }
            }
            return 0;
        }
        signed char WallKickValue(int gem_num, int x, int y, int spin, signed char wallkick_spin) const {
            const signed char kick_val = isWallKickSpin(x, y, getGem(gem_num, spin));
            if ( !kick_val || wallkick_spin == 0) {
                return wallkick_spin = 0;
            }
            if ( isEnableAllSpin() ) {
                if ( wallkick_spin == 2) {
                    wallkick_spin = 1;
                    Gem g = getGem(gem_num, spin);
                    for ( int dy = 0; dy < 4; ++dy ) { //KOS mini test
                        if ( g.bitmap[dy] == 0 ) continue;
                        if ( ((g.bitmap[dy] << x) | row[y+dy]) == m_w_mask ) continue;
                        wallkick_spin = 2;
                        break;
                    }
                }
            } else {
                wallkick_spin = 1;
				FILE* fp = fopen("tetris.log", "w");
				Gem g = getGem(gem_num, spin);
				Gem g_mod = getGem(gem_num, 2);
				Gem g_oppo = getGem(gem_num, (spin + 2) % 4);
				signed char back_x = spin == 1 ? (x - 1) : (x + 1);
				bool mini_check = isCollide(x - 1, y, g) && isCollide(x + 1, y, g) && !isCollide(x, y - 1, g);
				const signed char offset_x = x - x_before_spin;
				const signed char offset_y = (y - y_before_spin) * -1;
				fprintf(fp, "x: %d, y: %d, r: %c, spin: %d\n", offset_x, offset_y, spin_dir == 3 ? 'R' : 'L', spin);
				//              if ((mini_check && !isCollide(x, y, g_oppo)) || (mini_check && !isCollide(back_x, y, getGem(gem_num, 0))) || (spin % 2 != 0 && isCollide(x - 1, y, g) && isCollide(x + 1, y, g) && isCollide(x, y - 1, g) && isCollide(x, y, g_oppo))) {
				//                  fprintf(fp, "tmini\n");
				//                  fprintf(fp, "!isCollide(back_x, y - 1, getGem(gem_num, 0)): %d", !isCollide(back_x, y, getGem(gem_num, 0)));
				//                  wallkick_spin = 2;
				//              }
				//              else if (spin == 0) {
				//                  fprintf(fp, "tmini\n");
								  //wallkick_spin = 2;
				//              }
				//              else if (isCollide(x, y - 2, g) && isCollide(x, y, g_oppo) && !isCollide(x, y - 2, g_mod) && !isCollide(back_x, y - 2, g_mod)) {
				//                  fprintf(fp, "mini2 %d\n", kick_val);
				//                  wallkick_spin = 2;
				//              }
				//              else if (spin % 2 != 0 && !isCollide(x, y - 1, g) && !isCollide(x, y - 2, g)) {
				//                  fprintf(fp, "not a tspin\n");
				//                  wallkick_spin = 0;
				//              }

				if (spin == 0) {
					wallkick_spin = 3; // mini for clear_1, normal for clear_2
					fprintf(fp, "flat mini\n");
				}
				else if (offset_x == 0 && offset_y == 0 && !isCollide(x, y - 1, g)) {
					wallkick_spin = 2;
					fprintf(fp, "tspin mini on t2 slot\n");
				}
				else if (offset_x == (spin == 3 ? -1 : 1) && spin_dir == spin && offset_y == 0) {
					wallkick_spin = 2;
					fprintf(fp, "side tspin mini\n");
				}
				else if (offset_x == 0 && offset_y == -2 && spin_dir == (spin == 3 ? 1 : 3) && !isCollide(x, y - 1, g)) {
					wallkick_spin = 2;
					fprintf(fp, "polymer t2 mini");
				}
				fclose(fp);

            }
            return wallkick_spin;
        }
        int clearLines( signed char _wallkick_spin ) {
            int clearnum = 0;
            int h2 = m_h;
            for (int h = m_h; h >= -gem_add_y; --h) {
                if ( row[h] != m_w_mask) {
                    row[h2--] = row[h];
                } else {
                    ++ clearnum;
                }
            }
            for (int h = h2; h >= -gem_add_y; --h) {
                row[h] = 0;
            }
            if ( clearnum > 0 ) {
                ++combo;
                if ( clearnum == 4 ) {
                    ++b2b;
                } else if ( _wallkick_spin > 0 ) {
                    ++b2b;
                } else {
                    b2b = 0;
                }
            } else {
                combo = 0;
            }
            hashval = hash(*this);
            return clearnum;
        }
        int getPCAttack() const {
            return m_pc_att;
        }
        inline double mod1(double f) {
            while (f > 1) f--;
            return f;
        }
        int getAttack( int clearfull, signed char wallkick ) {
            int base_atk = 0, attack = 0;
            if (TETRIO_ATTACK_TABLE) {
                if (clearfull == 0) return 0;
                m_pc_att = 10;
                double raw = 0;
                // wallkick 2 = mini
                switch (clearfull) {
                case 0:
                    break;
                case 1:
                    switch (wallkick) {
                    case 0:
                    case 2:
                    case 3:
                        break;
                    default:
                        raw = 2;
                        break;
                    }
                    break;
                case 2:
                    switch (wallkick) {
                    case 0:
                        raw = 1;
                        break;
                    case 2:
                        raw = 2;
						break;
                    case 3:
                    default:
                        raw = 4;
                        break;
                    }
                    break;
                case 3:
                    switch (wallkick) {
                    case 0:
                        raw = 2;
                        break;
                    default:
                        raw = 6;
                        break;
                    }
                    break;
                case 4:
                    raw = 4;
                    break;
                }
                if (b2b > 1) {
                    double b = std::min(b2b - 1, 24);
                    raw += (floor(1 + log1p((b) * 0.8)) + (b == 1 ? 0 : (1 + mod1(log1p(b * 0.8))) / 3));
                }
                double c = std::min(combo - 1, 20);
                raw *= (1 + 0.25 * c);
                if (c > 1) raw = std::max(log1p(1.25 * c), raw);
                attack = floor(raw);
                int i = gem_add_y + m_h;
                for (; i >= 0; --i) {
                    if (m_row[i]) break;
                }
                if (i < 0) {
                    attack += m_pc_att; // pc
                }
            }
            else {
                m_pc_att = 6;
                if (clearfull > 1) {
                    if (clearfull < 4) {
                        attack = clearfull - 1;
                    }
                    else {
                        attack = clearfull;
                        if (b2b > 1) attack += 1;
                    }
                }
                if (clearfull > 0) {
                    if (wallkick) {
                        if (isEnableAllSpin()) {
                            attack += clearfull + 1;
                            if (wallkick == 2) { // mini
                                attack -= 1; // mini minus
                            }
                        }
                        else {
                            if (b2b > 1) attack += 1;
                            if (clearfull == 1) {
                                if (wallkick == 2) { // mini
                                    attack += 1;
                                }
                                else {
                                    attack += 2;
                                }
                            }
                            else {
                                attack += clearfull + 1;
                            }
                            if (clearfull == 3) {
                                if (b2b > 1) attack += 1;
                            }
                        }
                    }
                    attack += getComboAttack(combo);
                    {
                        int i = gem_add_y + m_h;
                        for (; i >= 0; --i) {
                            if (m_row[i]) break;
                        }
                        if (i < 0) {
                            attack += m_pc_att; // pc
                        }
                    }
                }
            }
            return attack;
        }
        void addRow( int rowdata ) {
            for ( int h = -gem_add_y + 1; h <= m_h; ++h ) {
                row[h-1] = row[h];
            }
            row[m_h] = rowdata;
        }
        void minusRow( int lines ) {
            //row += lines;
            //m_h -= lines;
        }
    };
}
