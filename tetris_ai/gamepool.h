#pragma once
#define _ALLOW_ITERATOR_DEBUG_LEVEL_MISMATCH

#include <string.h>
#include "tetris_gem.h"
#include "tetris_setting.h"
#include <math.h>
#ifdef XP_RELEASE
#define AI_POOL_MAX_H 50
#else
#define AI_POOL_MAX_H 32
#endif

namespace AI {
    struct GameField;
    Gem& getGem( int number, int spin );
    int getComboAttack( int combo );
    void setAllSpin(bool allSpin);
    bool isEnableAllSpin();
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
    const int gem_beg_y = 1;

    struct GameField {
        signed char m_w, m_h;
        signed short combo;
        signed char b2b;
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
                    if ( cnt >= 3 ) return 1;
                }
            }
            return 0;
        }
        signed char WallKickValue(int gem_num, int x, int y, int spin, signed char wallkick_spin) const {
            if ( ! isWallKickSpin( x, y, getGem(gem_num, spin) ) ) {
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
                if ( wallkick_spin == 2 ) {
                    if ( ! isCollide( x, y, getGem(gem_num, spin^2) ) ) {
                        wallkick_spin = 1; // not t-mini
                    }
                }
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
        int getAttack( int clearfull, signed char wallkick ) {
            int attack = 0;

            if (TETRIO_ATTACK_TABLE) {
                int base_atk = 0;
                auto get_attack = [&](int base_atk, int combo, int b2b) {
                    double atk = base_atk;
                    if (--b2b > 0) {
                        double f = log1p(b2b * 0.8);
                        atk += floor(1 + f) + (b2b > 1 ? 0 : (1 + f) / 3);
                    }
                    atk *= (1 + 0.25 * --combo);
                    if (combo > 1) {
                        double comboLog = log1p(1.25 * combo);
                        atk = atk > comboLog ? atk : comboLog;
                    }
                    return static_cast<int>(floor(atk));
                    };
                switch (clearfull) {
                case 0:
                    break;
                case 1:
                    switch (wallkick) {
                    case 0:
                        break;
                    case 2:
                        break;
                    default:
                        base_atk = 2;
                        break;
                    }
                    break;
                case 2:
                    switch (wallkick) {
                    case 0:
                        base_atk = 1;
                        break;
                    default:
                        base_atk = 4;
                        break;
                    }
                    break;
                case 3:
                    switch (wallkick) {
                    case 0:
                        base_atk = 2;
                        break;
                    default:
                        base_atk = 6;
                        break;
                    }
                    break;
                case 4:
                    base_atk = 4;
                    break;
                }
                if (clearfull != 0)
                    attack = get_attack(base_atk, combo, b2b);
                    {
                        int i = gem_add_y + m_h;
                        for (; i >= 0; --i) {
                            if (m_row[i]) break;
                        }
                        if (i < 0) {
                            attack += 10; // pc
                        }
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
