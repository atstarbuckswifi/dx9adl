/*
  quaternion.h
*/

#ifndef __QUATERNION_H__
#define __QUATERNION_H__

#ifndef QXM

typedef struct _QUATERNION_ {
  FLOAT x, y, z, w;
} QUATERNION;

#define QFV(q, qv) do{ \
  (q).x = (qv).x, (q).y = (qv).y, (q).z = (qv).z, (q).w = 1.f; \
}while(0)

#define QTV(qv, q) do{ \
  (qv).x = (q).x, (qv).y = (q).y, (qv).z = (q).z; \
}while(0)

typedef struct _QUATERNIONIC_MATRIX_ {
  FLOAT aa, ba, ca, da;
  FLOAT ab, bb, cb, db;
  FLOAT ac, bc, cc, dc;
  FLOAT ad, bd, cd, dd;
} QUATERNIONIC_MATRIX;

// sizeof(QUATERNIONIC_MATRIX) == sizeof(D3DMATRIX) == sizeof(D3DXMATRIXA16)
typedef struct _QQMATRIX_ {
  QUATERNIONIC_MATRIX *transform;
  QUATERNIONIC_MATRIX *rotation;
  QUATERNIONIC_MATRIX *scale;
  QUATERNIONIC_MATRIX *translate;
} QQMATRIX;

#if 1 // transposed matrix reversed multiply (q * m0 * m1 * m2)

#define QMM(mq, mq0, mq1) do{ \
mq.aa = mq0.aa * mq1.aa + mq0.ba * mq1.ab + mq0.ca * mq1.ac + mq0.da * mq1.ad;\
mq.ba = mq0.aa * mq1.ba + mq0.ba * mq1.bb + mq0.ca * mq1.bc + mq0.da * mq1.bd;\
mq.ca = mq0.aa * mq1.ca + mq0.ba * mq1.cb + mq0.ca * mq1.cc + mq0.da * mq1.cd;\
mq.da = mq0.aa * mq1.da + mq0.ba * mq1.db + mq0.ca * mq1.dc + mq0.da * mq1.dd;\
mq.ab = mq0.ab * mq1.aa + mq0.bb * mq1.ab + mq0.cb * mq1.ac + mq0.db * mq1.ad;\
mq.bb = mq0.ab * mq1.ba + mq0.bb * mq1.bb + mq0.cb * mq1.bc + mq0.db * mq1.bd;\
mq.cb = mq0.ab * mq1.ca + mq0.bb * mq1.cb + mq0.cb * mq1.cc + mq0.db * mq1.cd;\
mq.db = mq0.ab * mq1.da + mq0.bb * mq1.db + mq0.cb * mq1.dc + mq0.db * mq1.dd;\
mq.ac = mq0.ac * mq1.aa + mq0.bc * mq1.ab + mq0.cc * mq1.ac + mq0.dc * mq1.ad;\
mq.bc = mq0.ac * mq1.ba + mq0.bc * mq1.bb + mq0.cc * mq1.bc + mq0.dc * mq1.bd;\
mq.cc = mq0.ac * mq1.ca + mq0.bc * mq1.cb + mq0.cc * mq1.cc + mq0.dc * mq1.cd;\
mq.dc = mq0.ac * mq1.da + mq0.bc * mq1.db + mq0.cc * mq1.dc + mq0.dc * mq1.dd;\
mq.ad = mq0.ad * mq1.aa + mq0.bd * mq1.ab + mq0.cd * mq1.ac + mq0.dd * mq1.ad;\
mq.bd = mq0.ad * mq1.ba + mq0.bd * mq1.bb + mq0.cd * mq1.bc + mq0.dd * mq1.bd;\
mq.cd = mq0.ad * mq1.ca + mq0.bd * mq1.cb + mq0.cd * mq1.cc + mq0.dd * mq1.cd;\
mq.dd = mq0.ad * mq1.da + mq0.bd * mq1.db + mq0.cd * mq1.dc + mq0.dd * mq1.dd;\
}while(0)

#define Q3M(mq, mq0, mq1, mq2) do{ \
  QUATERNIONIC_MATRIX mtmp; \
  QMM(mtmp, mq0, mq1); \
  QMM(mq, mtmp, mq2); \
}while(0)

#define QXM(dq, sq, mq) do{ \
  dq.x = sq.x * (mq).aa + sq.y * (mq).ab + sq.z * (mq).ac + sq.w * (mq).ad; \
  dq.y = sq.x * (mq).ba + sq.y * (mq).bb + sq.z * (mq).bc + sq.w * (mq).bd; \
  dq.z = sq.x * (mq).ca + sq.y * (mq).cb + sq.z * (mq).cc + sq.w * (mq).cd; \
  dq.w = sq.x * (mq).da + sq.y * (mq).db + sq.z * (mq).dc + sq.w * (mq).dd; \
}while(0)

#else // compatible to Julia matrix (value is same as above) (m2 * m1 * m0 * q)

#define QMM(mq, mq0, mq1) do{ \
mq.aa = mq0.aa * mq1.aa + mq0.ab * mq1.ba + mq0.ac * mq1.ca + mq0.ad * mq1.da;\
mq.ab = mq0.aa * mq1.ab + mq0.ab * mq1.bb + mq0.ac * mq1.cb + mq0.ad * mq1.db;\
mq.ac = mq0.aa * mq1.ac + mq0.ab * mq1.bc + mq0.ac * mq1.cc + mq0.ad * mq1.dc;\
mq.ad = mq0.aa * mq1.ad + mq0.ab * mq1.bd + mq0.ac * mq1.cd + mq0.ad * mq1.dd;\
mq.ba = mq0.ba * mq1.aa + mq0.bb * mq1.ba + mq0.bc * mq1.ca + mq0.bd * mq1.da;\
mq.bb = mq0.ba * mq1.ab + mq0.bb * mq1.bb + mq0.bc * mq1.cb + mq0.bd * mq1.db;\
mq.bc = mq0.ba * mq1.ac + mq0.bb * mq1.bc + mq0.bc * mq1.cc + mq0.bd * mq1.dc;\
mq.bd = mq0.ba * mq1.ad + mq0.bb * mq1.bd + mq0.bc * mq1.cd + mq0.bd * mq1.dd;\
mq.ca = mq0.ca * mq1.aa + mq0.cb * mq1.ba + mq0.cc * mq1.ca + mq0.cd * mq1.da;\
mq.cb = mq0.ca * mq1.ab + mq0.cb * mq1.bb + mq0.cc * mq1.cb + mq0.cd * mq1.db;\
mq.cc = mq0.ca * mq1.ac + mq0.cb * mq1.bc + mq0.cc * mq1.cc + mq0.cd * mq1.dc;\
mq.cd = mq0.ca * mq1.ad + mq0.cb * mq1.bd + mq0.cc * mq1.cd + mq0.cd * mq1.dd;\
mq.da = mq0.da * mq1.aa + mq0.db * mq1.ba + mq0.dc * mq1.ca + mq0.dd * mq1.da;\
mq.db = mq0.da * mq1.ab + mq0.db * mq1.bb + mq0.dc * mq1.cb + mq0.dd * mq1.db;\
mq.dc = mq0.da * mq1.ac + mq0.db * mq1.bc + mq0.dc * mq1.cc + mq0.dd * mq1.dc;\
mq.dd = mq0.da * mq1.ad + mq0.db * mq1.bd + mq0.dc * mq1.cd + mq0.dd * mq1.dd;\
}while(0)

#define Q3M(mq, mq0, mq1, mq2) do{ \
  QUATERNIONIC_MATRIX mtmp; \
  QMM(mtmp, mq2, mq1); \
  QMM(mq, mtmp, mq0); \
}while(0)

#define QXM(dq, sq, mq) do{ \
  dq.x = (mq).aa * sq.x + (mq).ab * sq.y + (mq).ac * sq.z + (mq).ad * sq.w; \
  dq.y = (mq).ba * sq.x + (mq).bb * sq.y + (mq).bc * sq.z + (mq).bd * sq.w; \
  dq.z = (mq).ca * sq.x + (mq).cb * sq.y + (mq).cc * sq.z + (mq).cd * sq.w; \
  dq.w = (mq).da * sq.x + (mq).db * sq.y + (mq).dc * sq.z + (mq).dd * sq.w; \
}while(0)

#endif // transposed matrix reversed multiply

#endif

#endif // __QUATERNION_H__
