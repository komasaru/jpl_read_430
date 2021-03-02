#ifndef JPL_READ_430_JPL_HPP_
#define JPL_READ_430_JPL_HPP_

#include <cstdlib>   // for EXIT_XXXX
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

namespace jpl_read_430 {

class Jpl {
  double        jd;    // ユリウス日
  std::ifstream ifs;   // バイナリファイル

  void get_ttl(std::vector<std::string>&);       // 取得: TTL
  void get_cnam(std::vector<std::string>&);      // 取得: CNAM
  void get_ss(std::vector<double>&);             // 取得: SS
  void get_ncon(unsigned int&);                  // 取得: NCON
  void get_au(double&);                          // 取得: AU
  void get_emrat(double&);                       // 取得: EMRAT
  void get_numde(unsigned int&);                 // 取得: NUMDE
  void get_ipt(std::vector<std::vector<unsigned int>>&);   // 取得: IPT
  void get_cval(std::vector<double>&);           // 取得: CVAL
  void get_coeff(std::vector<std::vector<std::vector<std::vector<double>>>>&);
                                                 // 取得: COEFF
  template <class T>
  void get_val(unsigned int, unsigned int, T&);  // 取得: 値1件(template)
  void get_dbl_list(
      unsigned int, unsigned int, unsigned int,
      std::vector<double>&);                     // 取得: vector<double> 型
  void get_str_list(
      unsigned int, unsigned int, unsigned int,
      std::vector<std::string>&);                // 取得: vector<string> 型

public:
  std::vector<std::string>               ttls;    // TTL   (84 byte *   3)
  std::vector<std::string>               cnams;   // CNAM  ( 6 byte * 800)
  std::vector<double>                    sss;     // SS    ( 8 byte *   3)
  unsigned int                           ncon;    // NCON  ( 4 byte *   1)
  double                                 au;      // AU    ( 8 byte *   1)
  double                                 emrat;   // EMRAT ( 8 byte *   1)
  unsigned int                           numde;   // NUMDE ( 4 byte *   1)
  std::vector<std::vector<unsigned int>> ipts;    // IPT   ( 4 byte * 13 * 3)
  std::vector<double>                    cvals;   // SS    ( 8 byte * NCON)
  unsigned int                           idx;     // レコードインデックス
  std::vector<std::vector<std::vector<std::vector<double>>>> coeffs;
                                         // COEFF ( 8 byte *   ?)
                                         // 全惑星分の cnt_sub * (2 or 3) * cnt_coeff
                                         // （4次元配列）
  double                                 jds[2];  // JD (開始、終了)

  Jpl(double);      // コンストラクタ
  ~Jpl();           // デストラクタ
  void read_bin();  // バイナリファイル読み込み
};

}  // namespace jpl_read_430

#endif

