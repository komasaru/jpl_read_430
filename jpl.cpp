#include "jpl.hpp"

namespace jpl_read_430 {

// 定数
static constexpr char         kFBin[]    = "JPLEPH";        // バイナリファイル名
static constexpr unsigned int kKsize     = 2036;            // KSIZE
static constexpr unsigned int kRecl      =    4;            // 1レコード = KSIZE * 4
                                                            // ヘッダは2レコードで構成
static constexpr unsigned int kPosTtl    =    0;            // 位置: TTL
static constexpr unsigned int kPosCnam   =  252;            // 位置: CNAM
static constexpr unsigned int kPosSs     = 2652;            // 位置: SS
static constexpr unsigned int kPosNcon   = 2676;            // 位置: NCON
static constexpr unsigned int kPosAu     = 2680;            // 位置: AU
static constexpr unsigned int kPosEmrat  = 2688;            // 位置: EMRAT
static constexpr unsigned int kPosIpt    = 2696;            // 位置: IPT(IPT13以外)
static constexpr unsigned int kPosNumde  = 2840;            // 位置: NUMDE
static constexpr unsigned int kPosIpt2   = 2844;            // 位置: IPT13
static constexpr unsigned int kPosCnam2  = 2856;            // 位置: CNAM2(CNAMの続き)
static constexpr unsigned int kPosCval   = kKsize * kRecl;  // 位置: CVAL
static constexpr unsigned int kReclTtl   =   84;            // レコード長: TTL
static constexpr unsigned int kReclCnam  =    6;            // レコード長: CNAM
static constexpr unsigned int kReclSs    =    8;            // レコード長: SS
static constexpr unsigned int kReclNcon  =    4;            // レコード長: NCON
static constexpr unsigned int kReclAu    =    8;            // レコード長: AU
static constexpr unsigned int kReclEmrat =    8;            // レコード長: EMRAT
static constexpr unsigned int kReclIpt   =    4;            // レコード長: IPT
static constexpr unsigned int kReclNumde =    4;            // レコード長: IPT
static constexpr unsigned int kReclCval  =    8;            // レコード長: CVAL
static constexpr unsigned int kCntTtl    =    3;            // 件数: TTL
static constexpr unsigned int kCntCnam   =  400;            // 件数: CNAM
static constexpr unsigned int kCntSs     =    3;            // 件数: SS
static constexpr unsigned int kCntIpt    =   12;            // 件数: IPT（IPT13以外）

/*
 * @brief      コンストラクタ
 *
 * @param[in]  ユリウス日 (double)
 */
Jpl::Jpl(double jd) {
  this->jd = jd;
  ifs.open(kFBin, std::ios::binary);
  if (!ifs) {
    std::cout << "[ERROR] " << kFBin
              << " could not be found in this directory!" << std::endl;
    exit(EXIT_FAILURE);
  }
  // vector 用メモリ確保
  ttls.reserve(3);
  cnams.reserve(800);
  sss.reserve(3);
  ipts.reserve(13);
  cvals.reserve(572);  // DE430 で NCON = 572 であることを前提に
  coeffs.reserve(13);
}

/*
 * @brief  デストラクタ
 *
 * @param  <none>
 */
Jpl::~Jpl() { ifs.close(); }

/*
 * @brief   バイナリファイル読み込み
 *
 * @param   <none>
 * @return  <none>
 */
void Jpl::read_bin() {

  try {
    // ヘッダ（1レコード目）
    get_ttl(ttls);      // TTL  (タイトル)
    get_cnam(cnams);    // CNAM (定数名)(400+400件)
    get_ss(sss);        // SS   (ユリウス日(開始,終了),分割日数)
    get_ncon(ncon);     // NCON (定数の数)
    get_au(au);         // AU   (天文単位)
    get_emrat(emrat);   // EMRAT(地球と月の質量比)
    get_ipt(ipts);      // IPT  (オフセット,係数の数,サブ区間数)(水星〜月の章動,月の秤動)
    get_numde(numde);   // NUMDE(DEバージョン番号)
    // ヘッダ（2レコード目）
    get_cval(cvals);    // CVAL (定数値)
    // レコードインデックス取得
    idx = static_cast<int>(jd - sss[0]) / sss[2];
    // 係数取得（対象のインデックス分（全惑星分）を取得）
    get_coeff(coeffs);  // COEFF（係数）
  } catch (...) {
    throw;
  }
}

/*
 * @brief       取得: TTL
 *              - 84 byte * 3
 *
 * @param[ref]  TTL 一覧 (vector<string>)
 */
void Jpl::get_ttl(std::vector<std::string>& ttls) {
  try {
    get_str_list(kPosTtl, kReclTtl, kCntTtl, ttls);
  } catch (...) {
    throw;
  }
}

/*
 * @brief       取得: CNAM
 *              - 6 byte * (400 + 400)
 *
 * @param[ref]  CNAM 一覧 (vector<string>)
 */
void Jpl::get_cnam(std::vector<std::string>& cnams) {
  std::vector<std::string> cnam2s;   // CNAM2 (6 byte * 400)

  try {
    get_str_list(kPosCnam,  kReclCnam, kCntCnam, cnams );  // 最初の400件
    get_str_list(kPosCnam2, kReclCnam, kCntCnam, cnam2s);  // 後部の400件
    std::copy(cnam2s.begin(), cnam2s.end(), std::back_inserter(cnams));
  } catch (...) {
    throw;
  }
}

/*
 * @brief       取得: SS
 *              - 8 byte * 3
 *
 * @param[ref]  SS 一覧 (vector<double>)
 */
void Jpl::get_ss(std::vector<double>& sss) {
  try {
    get_dbl_list(kPosSs, kReclSs, kCntSs, sss);
  } catch (...) {
    throw;
  }
}

/*
 * @brief       取得: NCON
 *              - 4 byte * 1
 *
 * @param[ref]  NCON (unsigned int)
 */
void Jpl::get_ncon(unsigned int& ncon) {
  try {
    get_val<unsigned int>(kPosNcon, kReclNcon, ncon);
  } catch (...) {
    throw;
  }
}

/*
 * @brief       取得: AU
 *              - 8 byte * 1
 *
 * @param[ref]  AU (double)
 */
void Jpl::get_au(double& au) {
  try {
    get_val<double>(kPosAu, kReclAu, au);
  } catch (...) {
    throw;
  }
}

/*
 * @brief       取得: EMRAT
 *              - 8 byte * 1
 *
 * @param[ref]  EMRAT (double)
 */
void Jpl::get_emrat(double& emrat) {
  try {
    get_val<double>(kPosEmrat, kReclEmrat, emrat);
  } catch (...) {
    throw;
  }
}

/*
 * @brief       取得: NUMDE
 *              - 4 byte * 1
 *
 * @param[ref]  NUMDE (unsigned int)
 */
void Jpl::get_numde(unsigned int& numde) {
  try {
    get_val<unsigned int>(kPosNumde, kReclNumde, numde);
  } catch (...) {
    throw;
  }
}

/*
 * @brief       取得: IPT
 *
 * @param[ref]  値一覧 (vector<vector<unsigned int>>)
 */
void Jpl::get_ipt(std::vector<std::vector<unsigned int>>& vals) {
  unsigned int i;
  unsigned int j;
  unsigned int pos  = kPosIpt;
  unsigned int recl = kReclIpt;
  std::vector<unsigned int> ary;
  unsigned int buf;

  try {
    // IPT13 以外
    for (i = 0; i < kCntIpt; ++i) {
      ary.clear();
      for (j = 0; j < 3; ++j) {
        ifs.seekg(pos);
        ifs.read((char*)&buf, recl);
        ary.push_back(buf);
        pos += recl;
      }
      vals.push_back(ary);
    }
    // IPT13
    ary.clear();
    pos = kPosIpt2;
    for (j = 0; j < 3; ++j) {
      ifs.seekg(pos);
      ifs.read((char*)&buf, recl);
      ary.push_back(buf);
      pos += recl;
    }
    vals.push_back(ary);
  } catch (...) {
    throw;
  }
}

/*
 * @brief       取得: CVAL
 *              - 8 byte * NCON
 *
 * @param[ref]  CVAL 一覧 (vector<double>)
 */
void Jpl::get_cval(std::vector<double>& cvals) {
  try {
    get_dbl_list(kPosCval, kReclCval, ncon, cvals);
  } catch (...) {
    throw;
  }
}

/*
 * @brief       取得: COEFF
 *              - 8 byte * ?
 *              - 地球・章動のみ要素数が 2 で、その他の要素数は 3
 *
 * @param[ref]  値一覧 (vector<vector<vector<vector<double>>>>)
 */
void Jpl::get_coeff(
    std::vector<std::vector<std::vector<std::vector<double>>>>& vals) {
  unsigned int i;
  unsigned int j;
  unsigned int k;
  unsigned int l;
  double buf;
  std::vector<double> ary_a;                 // 該当インデックス分全て
  std::vector<double> ary;                   // 該当惑星分のみ
  std::vector<double> ary_w_1;               // 作業用
  std::vector<std::vector<double>> ary_w_2;  // 作業用
  std::vector<std::vector<std::vector<double>>> ary_p;  // 作業用
  unsigned int offset;
  unsigned int cnt_coeff;
  unsigned int cnt_sub;
  unsigned int n;
  unsigned int pos  = kKsize * kRecl * (2 + idx);
  unsigned int recl = 8;

  try {
    // 該当インデックス分全て取得
    for (i = 0; i < kKsize / 2; ++i) {
      ifs.seekg(pos);
      ifs.read((char*)&buf, recl);
      ary_a.push_back(buf);
      pos += recl;
    }

    // Julian Day (start, end)
    for (i = 0; i < 2; ++i) { jds[i] = ary_a[i]; }

    // 全惑星分
    for (i = 0; i < 13; ++i) {
      // 該当惑星分のみ抽出
      offset    = ipts[i][0];
      cnt_coeff = ipts[i][1];
      cnt_sub   = ipts[i][2];
      n = 3;
      if ((i + 1) == 12) { n = 2; }
      ary.clear();
      for (j = offset - 1; j < offset - 1 + cnt_coeff * n * cnt_sub; ++j) {
        ary.push_back(ary_a[j]);
      }

      // 3次元配列化
      // [サブ区間数, 要素数(3 or 2), 係数の数]
      ary_p.clear();
      for (j = 0; j < ary.size() / (cnt_coeff * n); ++j) {
        ary_w_2.clear();
        for (k = 0; k < n; ++k) {
          ary_w_1.clear();
          for (l = 0; l < cnt_coeff; ++l) {
            ary_w_1.push_back(ary[j * cnt_coeff * n + k * cnt_coeff + l]);
          }
          ary_w_2.push_back(ary_w_1);
        }
        ary_p.push_back(ary_w_2);
      }
      vals.push_back(ary_p);
    }
  } catch (...) {
    throw;
  }
}

/*
 * @brief       取得: (unsigned int|double) 型 1件 template
 *
 * @param[in]   レコード位置 (unsigned int)
 * @param[in]   レコード長 (unsigned int)
 * @param[ref]  値 (class T)
 */
template <class T>
void Jpl::get_val(unsigned int pos, unsigned int recl, T& val) {
  try {
    ifs.seekg(pos);
    ifs.read((char*)&val, recl);
  } catch (...) {
    throw;
  }
}

/*
 * @brief       取得: vector<double> 型
 *
 * @param[in]   レコード位置 (unsigned int)
 * @param[in]   レコード長 (unsigned int)
 * @param[in]   件数 (unsigned int)
 * @param[ref]  値一覧 (vector<double>)
 */
void Jpl::get_dbl_list(
    unsigned int pos, unsigned int recl, unsigned int cnt,
    std::vector<double>& vals) {
  unsigned int i;
  double       buf;

  try {
    for (i = 0; i < cnt; ++i) {
      ifs.seekg(pos);
      ifs.read((char*)&buf, recl);
      vals.push_back(buf);
      pos += recl;
    }
  } catch (...) {
    throw;
  }
}

/*
 * @brief       取得: vector<string> 型
 *              (後のスペースは trim)
 *
 * @param[in]   レコード位置 (unsigned int)
 * @param[in]   レコード長 (unsigned int)
 * @param[in]   件数 (unsigned int)
 * @param[ref]  値一覧 (vector<string>)
 */
void Jpl::get_str_list(
    unsigned int pos, unsigned int recl, unsigned int cnt,
    std::vector<std::string>& vals) {
  unsigned int i;
  char*        buf;
  std::string  str;

  try {
    for (i = 0; i < cnt; ++i) {
      ifs.seekg(pos);
      buf = new char[recl];
      ifs.read(buf, recl);
      str = buf;
      vals.push_back(str.erase(str.find_last_not_of(" ") + 1));
      pos += recl;
      delete buf;
    }
  } catch (...) {
    throw;
  }
}

}  // namespace jpl_read_430

