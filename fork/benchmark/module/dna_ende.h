/*
*�ļ�����: dna_endec.h
*�ļ���ʶ: 
*ժ    Ҫ: ����ͨ�ñ��������. 
*
*��ǰ�汾: 1.0
*����:   �ܽ� HoraceZhou
*�������: 2006-05-13
*����������: 2007-07-10
*/

#ifndef __BASIC_ENDEC_HPP_
#define __BASIC_ENDEC_HPP_

#ifdef WIN32
	#include <windows.h>
#else
	#include <netinet/in.h>
#endif

#include <limits>
#include <functional>
#include <string.h>

struct htons_t: std::unary_function<unsigned short, unsigned short>
{
	unsigned short operator()(unsigned short& _X) const
	{
		return htons(_X); 
	}
};

struct ntohs_t: std::unary_function<unsigned short, unsigned short>
{
	unsigned short operator()(unsigned short& _X) const
	{
		return ntohs(_X); 
	}
};

struct htonl_t: std::unary_function<unsigned int, unsigned int>
{
	unsigned int operator()(unsigned int& _X) const
	{
		return htonl(_X); 
	}
};

struct ntohl_t: std::unary_function<unsigned int, unsigned int>
{
	unsigned int operator()(unsigned int& _X) const
	{
		return ntohl(_X); 
	}
};

// --------------------------------------------------------------------------------

// ��value��ʼ��valueLen�������ݣ����뵽destBufָ����ڴ�
// ���� -n ��ʾdestBufȱ�ٵĳ���Ϊn��>= 0 ��ʾ����ĳ���
// destBufLen < 0 ��ʾ����Ҫ����destBuf�ĳ��ȣ�destBuf���������
int EncodeMem(void *destBuf, int destBufLen, const void *value, size_t valueLen);

// value��ҪvalueLen��ô�������ݣ����srcBuf�����ݲ������򷵻ش���
// ���� -n ��ʾȱ�ٵĳ���Ϊn��>= 0 ��ʾ����ĳ���
// srcBufLen < 0 ��ʾ����Ҫ����srcBuf�ĳ���
int DecodeMem(const void *srcBuf, int srcBufLen, void *value, size_t valueLen);

// --------------------------------------------------------------------------------

template<typename _VTy>
inline int EncodeFixedlenType(void *destBuf, int destBufLen, _VTy value)
{
	return EncodeMem(destBuf, destBufLen, &value, sizeof(_VTy));
}

template<typename _VTy, class _Pt>
inline int EncodeFixedlenType_Fn(void *destBuf, int destBufLen, _VTy value)
{
	value = _Pt()(value);
	return EncodeMem(destBuf, destBufLen, &value, sizeof(_VTy));
}

template<typename _VTy>
inline int DecodeFixedlenType(const void *srcBuf, int srcBufLen, _VTy *value)
{
	return DecodeMem(srcBuf, srcBufLen, value, sizeof(_VTy));
}

template<typename _VTy, class _Pt>
inline int DecodeFixedlenType_Fn(const void *srcBuf, int srcBufLen, _VTy *value)
{
	int result = DecodeMem(srcBuf, srcBufLen, value, sizeof(_VTy));
	if (result > 0)
		*value = _Pt()(*value);
	return result;
}

// --------------------------------------------------------------------------------
// VC6 Bug: ����ģ�������û�г����ں����ġ��β��б���ʱ����ǩ���޷����ֲ�ͬʵ����
// �������������һ�� _LTy *noUse �����������������ں�������û���õġ�

// ���ܣ����ַ���str����LV��length-value����ʽ���뵽destBuf���ַ�����0��β���������롣
// ����ֵ��������ĳ��ȡ�
// ������
//     destBuf��Ŀ�껺����ָ��
//     destBufLen��Ŀ�껺����ָ�볤�ȡ��� < 0 ��ʾ����Ҫ����destBuf�ĳ��ȣ�destBuf���������
//     str��Դ�ַ�����������0��β���ַ�����������maxStrLen����>0��ʱ�������Ĳ��ֱ��ضϣ����Ե��������Լ��ж�str�����Ƿ񳬳���
//     maxStrLen������Դ�ַ�������ĳ��ȡ�

// eg��
// char buf[25];
// EncodeLVString<short>(buf, 25, "abcde", -1)
// ִ�к󷵻�7��buf��� 05 00 61 62 63 64 65������05 00��short(5)��61 62 63 64 65��chars(abcde)
// 
template<typename _LTy>
int EncodeLVString(void *destBuf, int destBufLen, const char *str, int maxStrLen, _LTy *noUse = NULL)
{
	int realStrLen = (int)strlen(str);

#ifdef max
#undef max
#endif
	if ((size_t)realStrLen > (size_t)std::numeric_limits<_LTy>::max())
		realStrLen = (int)std::numeric_limits<_LTy>::max();

	_LTy strLen = 0; 
	if (((int)maxStrLen >= 0) && (realStrLen > maxStrLen))
		strLen = (_LTy)maxStrLen;
	else
		strLen = (_LTy)realStrLen;

	// ����Ҫ���鳤�ȣ�����鳤��
	bool checkLen = (destBufLen >= 0);
	if ((checkLen) && ((int)strLen + (int)sizeof(_LTy) > destBufLen))
		return destBufLen - (strLen + sizeof(_LTy));
	
	int codeLen = EncodeFixedlenType<_LTy>(destBuf, checkLen ? destBufLen : -1, strLen);
	if (codeLen < 0)
		return codeLen;
	int codeLen2 = EncodeMem((char *)destBuf+codeLen, checkLen ? destBufLen-codeLen : -1, str, strLen);
	if (codeLen2 < 0)
		return codeLen2;
	return codeLen + codeLen2;
}

template<typename _LTy, class _Pt>
int EncodeLVString_Fn(void *destBuf, int destBufLen, const char *str, int maxStrLen, _LTy *noUse = NULL)
{
	int realStrLen = (int)strlen(str);

#ifdef max
#undef max
#endif
	if ((size_t)realStrLen > (size_t)std::numeric_limits<_LTy>::max())
		realStrLen = (int)std::numeric_limits<_LTy>::max();

	_LTy strLen = 0; 
	if ((maxStrLen >= 0) && (realStrLen > maxStrLen))
		strLen = (_LTy)maxStrLen;
	else
		strLen = (_LTy)realStrLen;

	// ����Ҫ���鳤�ȣ�����鳤��
	bool checkLen = (destBufLen >= 0);
	if ((checkLen) && ((int)strLen + (int)sizeof(_LTy) > destBufLen))
		return destBufLen - (strLen + sizeof(_LTy));
	
	int codeLen = EncodeFixedlenType_Fn<_LTy, _Pt>(destBuf, checkLen ? destBufLen : -1, strLen); // <-------------
	if (codeLen < 0)
		return codeLen;
	int codeLen2 = EncodeMem((char *)destBuf+codeLen, checkLen ? destBufLen-codeLen : -1, str, strLen);
	if (codeLen2 < 0)
		return codeLen2;
	return codeLen + codeLen2;
}

// ���ܣ���srcBuf���ݣ���LV��length-value����ʽ���뵽str��str�����ַ�����󳤶�ΪmaxStrLen��������0��β����
// ����ֵ��������ĳ��ȡ�
// ������
//     srcBuf��Դ������ָ��
//     srcBufLen��Դ������ָ�볤�ȡ��� < 0 ��ʾ����Ҫ����srcBuf�ĳ��ȣ�srcBuf���������
//     str��Ŀ���ַ�����
//     maxStrLen��str���ܵ���󳤶ȣ�������0��β����

// ע�⣺
// maxStrLen: �������������������ַ���������Ӧ��maxStrLen+1

// eg:
// char str[MAXLEN+1];
// DecodeLVString(strBuf, strBufLen, str, MAXLEN);

template<typename _LTy>
int DecodeLVString(const void *srcBuf, int srcBufLen, char *str, int maxStrLen, _LTy *noUse = NULL)
{
	bool checkLen = (srcBufLen >= 0);
	if ((checkLen) && ((int)sizeof(_LTy) > srcBufLen))
		return srcBufLen - (int)sizeof(_LTy);

	_LTy strLen = 0; 
	int codeLen = DecodeFixedlenType<_LTy>(srcBuf, srcBufLen, &strLen);
	if (codeLen < 0)
		return codeLen;
//	if (strLen < (_LTy)0) // added 20070709
//		return -1;

	if ((maxStrLen >= 0) && ((int)strLen > maxStrLen))
		strLen = maxStrLen;
	if (checkLen) {
		srcBufLen -= codeLen;
		if (srcBufLen < 0)
			return srcBufLen;
	}
	
	srcBuf = (const char *)srcBuf + codeLen;
	int codeLen2 = DecodeMem(srcBuf, srcBufLen, str, strLen);
	if (codeLen2 < 0)
		return codeLen2;
	str[strLen] = 0;
	return codeLen + codeLen2;
}

template<typename _LTy, class _Pt>
int DecodeLVString_Fn(const void *srcBuf, int srcBufLen, char *str, int maxStrLen, _LTy *noUse = NULL)
{
	bool checkLen = (srcBufLen >= 0);
	if ((checkLen) && ((int)sizeof(_LTy) > srcBufLen))
		return srcBufLen - (int)sizeof(_LTy);

	_LTy strLen = 0; 
	int codeLen = DecodeFixedlenType_Fn<_LTy, _Pt>(srcBuf, srcBufLen, &strLen); // <-------------
	if (codeLen < 0)
		return codeLen;
//	if (strLen < (_LTy)0)  // added 20070709
//		return -1;

	if ((maxStrLen >= 0) && ((int)strLen > maxStrLen))
		strLen = maxStrLen;
	if (checkLen) {
		srcBufLen -= codeLen;
		if (srcBufLen < 0)
			return srcBufLen;
	}
	
	srcBuf = (const char *)srcBuf + codeLen;
	int codeLen2 = DecodeMem(srcBuf, srcBufLen, str, strLen);
	if (codeLen2 < 0)
		return codeLen2;
	str[strLen] = 0;
	return codeLen + codeLen2;
}

// --------------- Encode ---------------
#define _EncodeInt8 EncodeFixedlenType<char>
#define _EncodeUInt8 EncodeFixedlenType<unsigned char>

#define _EncodeInt16 EncodeFixedlenType<short>
#define _EncodeUInt16 EncodeFixedlenType<unsigned short>
#define _EncodeNUInt16 EncodeFixedlenType_Fn<unsigned short, htons_t>

#define _EncodeInt32 EncodeFixedlenType<int>
#define _EncodeUInt32 EncodeFixedlenType<unsigned int>
#define _EncodeNUInt32 EncodeFixedlenType_Fn<unsigned int, htonl_t>

#define _EncodeUInt8LVString EncodeLVString<unsigned char>
#define _EncodeUInt16LVString EncodeLVString<unsigned short>
#define _EncodeNUInt16LVString EncodeLVString_Fn<unsigned short, htons_t>
#define _EncodeUInt32LVString EncodeLVString<unsigned int>
#define _EncodeNUInt32LVString EncodeLVString_Fn<unsigned int, htonl_t>

int EncodeBitStr(char *destBuf, unsigned char fromBitPos, const char *bits, int maxBitCount);
// Encode һ��0��β���ַ���
// maxStrLen ������0��β�ĳ���
int EncodeString(void *destBuf, int destBufLen, const char *str, int maxStrLen);

// --------------- Decode ---------------
#define _DecodeInt8 DecodeFixedlenType<char>
#define _DecodeUInt8 DecodeFixedlenType<unsigned char>

#define _DecodeInt16 DecodeFixedlenType<short>
#define _DecodeUInt16 DecodeFixedlenType<unsigned short>
#define _DecodeNUInt16 DecodeFixedlenType_Fn<unsigned short, ntohs_t>

#define _DecodeInt32 DecodeFixedlenType<int>
#define _DecodeUInt32 DecodeFixedlenType<unsigned int>
#define _DecodeNUInt32 DecodeFixedlenType_Fn<unsigned int, ntohl_t>
// new
//#define _DecodeNInt32(srcBuf, srcBufLen, value) _DecodeNInt32(srcBuf, srcBufLen, (unsigned int *)value)

#define _DecodeUInt8LVString DecodeLVString<unsigned char>
#define _DecodeUInt16LVString DecodeLVString<unsigned short>
#define _DecodeNUInt16LVString DecodeLVString_Fn<unsigned short, ntohs_t>
#define _DecodeUInt32LVString DecodeLVString<unsigned int>
#define _DecodeNUInt32LVString DecodeLVString_Fn<unsigned int, ntohl_t>

int DecodeBitStr(const char *srcBuf, unsigned char fromBitPos, char *bits, int maxBitCount);
// Decode һ��0��β���ַ���
// maxStrLen ������0��β�ĳ���
int DecodeString(const void *srcBuf, int srcBufLen, char *str, int maxStrLen);

// --------------- CStreamEncoder ---------------
//*
class CStreamHandler
{
public:
	enum {
		MAX_BOOKMARK = 10,
	};
	struct stBookmark 
	{
		unsigned int offset;
		unsigned char bitpos;
	};

public:
	CStreamHandler();
	CStreamHandler(void *Buf, int BufLen, unsigned char BitPos = 0);
	virtual ~CStreamHandler();

public:
	void SetBuffer(void *Buf, int BufLen, unsigned char BitPos = 0);
	void * GetBuffer(int *bufLen = NULL, unsigned char *bitPos = NULL);
	void * GetCurBuffer(int *leftBufLen = NULL, unsigned char *bitPos = NULL);
	unsigned int GetCurLen(unsigned char *bitLen = NULL);

	// MoveBytes ����ִ��align����������Ҫalign�����ڵ��ú���ǰִ��AlignBits;
	// ���� 0 �ɹ�������ʧ��
	int MoveBytes(int byteCount);
	// ���� 0 �ɹ�������ʧ��
	int MoveBits(int bitCount);
	
	// �����ƶ���bit��
	int AlignBits();
	
protected:
	// ����������ȡ��ǩ
	stBookmark * GetBookmark(int idx);
	// ���idx=-1, ��ʾ�Զ����䡣��������
	int SetBookmark(int idx, unsigned int offset, unsigned char bitpos = 0);
	// �ѵ�ǰλ�ü��뵽��ǩ
//	stBookmark * AddCurToBookmark(int idx);

	char *m_Buf;
	int m_BufLen;  // ��ǰ��[ʣ��]����byte, ����˵��m_destBuf�ĳ���
	unsigned char m_BitPos;  // ��¼��ǰpos��bitָ����һλ 0-7

	char *m_curBuf;  // ��ǰָ��
	int m_curBufLen; // ��ǰ��[ʣ��]����byte, ���m_curBitPos>0, ����ʵֻ��(m_curBufLen-1)�ֽڼ�(8-m_curBitPos)λ����, ����Ȼ��ʾΪm_curBufLen
	unsigned char m_curBitPos;  // ��¼��ǰpos��bitָ����һλ 0-7
	
	bool m_checkLen;

private:
	stBookmark m_bookmarks[MAX_BOOKMARK];
};
//*/

// --------------- CStreamEncoder ---------------
class CStreamEncoder : public CStreamHandler
{
public:
	CStreamEncoder();
	CStreamEncoder(void *Buf, int BufLen, unsigned char BitPos = 0);

public:
	int EncodeMem(const void *value, size_t valueLen);
	
	int EncodeInt8(char value);
	int EncodeUInt8(unsigned char value);

	int EncodeInt16(short value);
	int EncodeUInt16(unsigned short value);
	int EncodeNUInt16(unsigned short value);

	int EncodeInt32(int value);
	int EncodeUInt32(unsigned int value);
	int EncodeNUInt32(unsigned int value);

	// maxStrLen �ǲ�����0��β�ĳ���
	int EncodeNullTermString(const char *str, int maxStrLen);
	int EncodeUInt8LVString(const char *str, int maxStrLen);
	int EncodeUInt16LVString(const char *str, int maxStrLen);
	int EncodeNUInt16LVString(const char *str, int maxStrLen);
	int EncodeUInt32LVString(const char *str, int maxStrLen);
	int EncodeNUInt32LVString(const char *str, int maxStrLen);

	// ���ر������bit��
	// bitCount: bit�ĳ��ȣ�����>=0
	//eg: EncodeBits("101111011100", 12)
	int EncodeBitStr(const char *bits, int bitCount);
	
public:
	int EncodeChar(char value) {
		return EncodeInt8(value);
	}
	int EncodeShort(short value) {
		return EncodeNUInt16((unsigned short)value);
	}
	int EncodeInt(int value) {
		return EncodeNUInt32((unsigned int)value);
	}
	int EncodeString(const char *str, int maxStrLen) {
		return EncodeNUInt16LVString(str, maxStrLen);
	}
};

// --------------- CStreamDecoder ---------------
class CStreamDecoder : public CStreamHandler
{
public:
	CStreamDecoder();
	CStreamDecoder(const void *Buf, int BufLen, unsigned char BitPos = 0);;

public:
	int DecodeMem(void *value, size_t valueLen);
	
	int DecodeInt8(char *value);
	int DecodeUInt8(unsigned char *value);

	int DecodeInt16(short *value);
	int DecodeUInt16(unsigned short *value);
	int DecodeNUInt16(unsigned short *value);

	int DecodeInt32(int *value);
	int DecodeUInt32(unsigned int *value);
	int DecodeNUInt32(unsigned int *value);

	// maxStrLen ���>0����ʾ������0��β����󳤶ȣ�����str�Ļ�����������Ҫ������maxStrLen+1
	int DecodeNullTermString(char *str, int maxStrLen);
	int DecodeUInt8LVString(char *str, int maxStrLen);
	int DecodeUInt16LVString(char *str, int maxStrLen);
	int DecodeNUInt16LVString(char *str, int maxStrLen);
	int DecodeUInt32LVString(char *str, int maxStrLen);
	int DecodeNUInt32LVString(char *str, int maxStrLen);

	// ���ر������bit��
	// bitCount: bit�ĳ��ȣ�����>=0
	// ע�⣺������bits�ַ�����β�Զ����0
	//eg: DecodeBits(bits, 12); bits[12] = 0;
	int DecodeBitStr(char *bits, int bitCount);

public:
	int DecodeChar(char *value) {
		return DecodeInt8(value);
	}
	int DecodeShort(short *value) {
		return DecodeNUInt16((unsigned short *)value);
	}
	int DecodeInt(int *value) {
		return DecodeNUInt32((unsigned int *)value);
	}
	int DecodeString(char *str, int maxStrLen) {
		return DecodeNUInt16LVString(str, maxStrLen);
	}
};

#endif
