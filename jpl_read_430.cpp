/***********************************************************
  JPLEPH(JPL の DE430 バイナリデータ)読み込み
  : データの読み込みテストのため、対象日時・天体番号は定数で設定している
       1: 水星 (Mercury)
       2: 金星 (Venus)
       3: 地球 - 月の重心 (Earth-Moon barycenter)
       4: 火星 (Mars)
       5: 木星 (Jupiter)
       6: 土星 (Saturn)
       7: 天王星 (Uranus)
       8: 海王星 (Neptune)
       9: 冥王星 (Pluto)
      10: 月（地心） (Moon (geocentric))
      11: 太陽 (Sun)
      12: 地球の章動（1980年IAUモデル） (Earth Nutations in longitude and obliquity (IAU 1980 model))
      13: 月の秤動 (Lunar mantle libration)

    DATE        AUTHOR       VERSION
    2020.12.23  mk-mode.com  1.00 新規作成

  Copyright(C) 2020 mk-mode.com All Rights Reserved.

  ----------------------------------------------------------
  引数 : 天体番号(1-13) ユリウス日
         * ユリウス日無指定なら現在(システム日時)と判断。
         * 天体番号も無指定なら太陽(11)と判断。
***********************************************************/
#include "jpl.hpp"

#include <cstdlib>   // for EXIT_XXXX
#include <iomanip>
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
  static constexpr unsigned int kNoSun = 11;         // 天体番号（太陽）
  static constexpr double       kJd0   = 2458849.5;  // 2000-01-01 UTC
  namespace ns = jpl_read_430;
  unsigned int astr = kNoSun;  // 天体番号
  double       jd   = kJd0;    // ユリウス日
  unsigned int i;

  try {
    if (argc > 1) {
      // 天体番号取得
      astr = atoi(argv[1]);
      if (astr < 1 || astr > 13) {
        std::cout << "[USAGE] ./jpl_read_430 [ASTR_NO [JULIAN_DAY]]"
                  << std::endl
                  << "        !!! ASTR_NO must be between 1 and 13 !!!"
                  << std::endl;
        return EXIT_FAILURE;
      }
      // ユリウス日取得
      if (argc > 2) { jd = atof(argv[2]); }
    }

    // バイナリファイル読み込み
    ns::Jpl o_jpl(jd);
    o_jpl.read_bin();

    // 結果出力
    // （単に取得した情報を確認するためだけのものなので、
    //   視認性は考慮していない）
    std::cout << "---" << std::endl;
    std::cout << " Astro No. : " << astr << std::endl;
    std::cout << "Julian Day : " << std::fixed << std::setprecision(1)
              << jd << std::endl;
    std::cout << "=== TTL" << std::endl;
    for (i = 0; i < 3; ++i) {
      std::cout << o_jpl.ttls[i] << std::endl;
    }
    std::cout << "=== CNAM" << std::endl;
    for (i = 0; i < 800; ++i) {
      std::cout << o_jpl.cnams[i] << std::endl;
    }
    std::cout << "=== SS" << std::endl;
    for (i = 0; i < 3; ++i) {
      std::cout << o_jpl.sss[i] << std::endl;
    }
    std::cout << "=== NCON" << std::endl;
    std::cout << o_jpl.ncon << std::endl;
    std::cout << "=== AU" << std::endl;
    std::cout << o_jpl.au << std::endl;
    std::cout << "=== EMRAT" << std::endl;
    std::cout << std::fixed << std::setprecision(14)
              << o_jpl.emrat << std::endl;
    std::cout << "=== NUMDE" << std::endl;
    std::cout << o_jpl.numde << std::endl;
    std::cout << "=== IPT" << std::endl;
    for (i = 0; i < 13; ++i) {
      std::cout << "["
                << o_jpl.ipts[i][0] << ", "
                << o_jpl.ipts[i][1] << ", "
                << o_jpl.ipts[i][2] << "]" << std::endl;
    }
    std::cout << "=== CVAL" << std::endl;
    for (i = 0; i < o_jpl.ncon; ++i) {
      std::cout << std::scientific << o_jpl.cvals[i] << std::endl;
    }
    std::cout << "=== Record Index" << std::endl;
    std::cout << o_jpl.idx << std::endl;
    std::cout << "=== Julian Day (start, end)" << std::endl;
    std::cout << std::fixed << std::setprecision(1)
              <<  o_jpl.jds[0] << ", " << o_jpl.jds[1] << std::endl;
    std::cout << "=== COEFF" << std::endl;
    std::cout << std::fixed << std::setprecision(14);
    for (auto& v_0 : o_jpl.coeffs[astr - 1] ) {
      for (auto& v_1 : v_0) {
        for (auto& v_2 : v_1) {
          std::cout << v_2 << std::endl;
        }
        std::cout << "-" << std::endl;
      }
    }
    //for (i = 0; i < 13; ++i) {
    //  std::cout << o_jpl.coeffs[i].size() << std::endl;
    //}
  } catch (...) {
      std::cerr << "EXCEPTION!" << std::endl;
      return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

