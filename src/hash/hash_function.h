/*a total of 18 hash functions*/
#ifndef HASH_FUNCTION_H
#define HASH_FUNCTION_H

typedef unsigned int uint;
typedef unsigned char uchar;
typedef unsigned long long uint64;

#define mix(a,b,c) \
{ \
  a -= b; a -= c; a ^= (c>>13); \
  b -= c; b -= a; b ^= (a<<8); \
  c -= a; c -= b; c ^= (b>>13); \
  a -= b; a -= c; a ^= (c>>12);  \
  b -= c; b -= a; b ^= (a<<16); \
  c -= a; c -= b; c ^= (b>>5); \
  a -= b; a -= c; a ^= (c>>3);  \
  b -= c; b -= a; b ^= (a<<10); \
  c -= a; c -= b; c ^= (b>>15); \
}


unsigned int 
BOB(const char * str, unsigned int len);

unsigned int 
OAAT(const char * str, unsigned int len);

unsigned int 
Simple(const char * str, unsigned int len);

unsigned int 
SBOX(const char * str, unsigned int len);

//This is an integer hash function
unsigned int 
TWMX(unsigned int a);

unsigned int
Hsieh(const char * str, unsigned int len);

unsigned int 
RSHash(const char * str, unsigned int len);

unsigned int 
JSHash(const char * str, unsigned int len);

unsigned int 
BKDR(const char * str, unsigned int len);

unsigned int 
DJBHash (const char * str, unsigned int len);

unsigned int 
DEKHash(const char * str, unsigned int len);   

unsigned int
APHash(const char *str, unsigned int len);

unsigned int 
CRC32(const char * str,unsigned int len);

unsigned int
SDBM(const char * str, unsigned int len);

unsigned int
OCaml(const char *str, unsigned int len);

unsigned int
SML(const char *str, unsigned int len);

unsigned int
STL(const char *str, unsigned int len);

unsigned int
FNV32(const char *str, unsigned int len);

unsigned int
PJWHash (const char *str, unsigned int len);

//unsigned int
//MD5(const char *str, unsigned int len);

//unsigned int
//SHA1(const char *str, unsigned int len);


////////////////
unsigned int 
BOB1(const char * str, unsigned int len)
{
	//register ub4 a,b,c,len;
	unsigned int a,b,c;
	unsigned int initval = 2;
	/* Set up the internal state */
	//len = length;
	a = b = 0x9e3779b9;  /* the golden ratio; an arbitrary value */
	c = initval;         /* the previous hash value */

	/*---------------------------------------- handle most of the key */
	while (len >= 12)
	{
		a += (str[0] +((unsigned int)str[1]<<8) +((unsigned int)str[2]<<16) +((unsigned int)str[3]<<24));
		b += (str[4] +((unsigned int)str[5]<<8) +((unsigned int)str[6]<<16) +((unsigned int)str[7]<<24));
		c += (str[8] +((unsigned int)str[9]<<8) +((unsigned int)str[10]<<16)+((unsigned int)str[11]<<24));
		mix(a,b,c);
		str += 12; len -= 12;
	}

	/*------------------------------------- handle the last 11 bytes */
	c += len;
	switch(len)              /* all the case statements fall through */
	{
		case 11: c+=((unsigned int)str[10]<<24);
		case 10: c+=((unsigned int)str[9]<<16);
		case 9 : c+=((unsigned int)str[8]<<8);
		/* the first byte of c is reserved for the length */
		case 8 : b+=((unsigned int)str[7]<<24);
		case 7 : b+=((unsigned int)str[6]<<16);
		case 6 : b+=((unsigned int)str[5]<<8);
		case 5 : b+=str[4];
		case 4 : a+=((unsigned int)str[3]<<24);
		case 3 : a+=((unsigned int)str[2]<<16);
		case 2 : a+=((unsigned int)str[1]<<8);
		case 1 : a+=str[0];
		/* case 0: nothing left to add */
	}
	mix(a,b,c);
	/*-------------------------------------------- report the result */
	return c;
}
unsigned int 
BOB2(const char * str, unsigned int len)
{
	//register ub4 a,b,c,len;
	unsigned int a,b,c;
	unsigned int initval = 31;
	/* Set up the internal state */
	//len = length;
	a = b = 0x9e3779b9;  /* the golden ratio; an arbitrary value */
	c = initval;         /* the previous hash value */

	/*---------------------------------------- handle most of the key */
	while (len >= 12)
	{
		a += (str[0] +((unsigned int)str[1]<<8) +((unsigned int)str[2]<<16) +((unsigned int)str[3]<<24));
		b += (str[4] +((unsigned int)str[5]<<8) +((unsigned int)str[6]<<16) +((unsigned int)str[7]<<24));
		c += (str[8] +((unsigned int)str[9]<<8) +((unsigned int)str[10]<<16)+((unsigned int)str[11]<<24));
		mix(a,b,c);
		str += 12; len -= 12;
	}

	/*------------------------------------- handle the last 11 bytes */
	c += len;
	switch(len)              /* all the case statements fall through */
	{
		case 11: c+=((unsigned int)str[10]<<24);
		case 10: c+=((unsigned int)str[9]<<16);
		case 9 : c+=((unsigned int)str[8]<<8);
		/* the first byte of c is reserved for the length */
		case 8 : b+=((unsigned int)str[7]<<24);
		case 7 : b+=((unsigned int)str[6]<<16);
		case 6 : b+=((unsigned int)str[5]<<8);
		case 5 : b+=str[4];
		case 4 : a+=((unsigned int)str[3]<<24);
		case 3 : a+=((unsigned int)str[2]<<16);
		case 2 : a+=((unsigned int)str[1]<<8);
		case 1 : a+=str[0];
		/* case 0: nothing left to add */
	}
	mix(a,b,c);
	/*-------------------------------------------- report the result */
	return c;
}
unsigned int 
BOB3(const char * str, unsigned int len)
{
	//register ub4 a,b,c,len;
	unsigned int a,b,c;
	unsigned int initval = 73;
	/* Set up the internal state */
	//len = length;
	a = b = 0x9e3779b9;  /* the golden ratio; an arbitrary value */
	c = initval;         /* the previous hash value */

	/*---------------------------------------- handle most of the key */
	while (len >= 12)
	{
		a += (str[0] +((unsigned int)str[1]<<8) +((unsigned int)str[2]<<16) +((unsigned int)str[3]<<24));
		b += (str[4] +((unsigned int)str[5]<<8) +((unsigned int)str[6]<<16) +((unsigned int)str[7]<<24));
		c += (str[8] +((unsigned int)str[9]<<8) +((unsigned int)str[10]<<16)+((unsigned int)str[11]<<24));
		mix(a,b,c);
		str += 12; len -= 12;
	}

	/*------------------------------------- handle the last 11 bytes */
	c += len;
	switch(len)              /* all the case statements fall through */
	{
		case 11: c+=((unsigned int)str[10]<<24);
		case 10: c+=((unsigned int)str[9]<<16);
		case 9 : c+=((unsigned int)str[8]<<8);
		/* the first byte of c is reserved for the length */
		case 8 : b+=((unsigned int)str[7]<<24);
		case 7 : b+=((unsigned int)str[6]<<16);
		case 6 : b+=((unsigned int)str[5]<<8);
		case 5 : b+=str[4];
		case 4 : a+=((unsigned int)str[3]<<24);
		case 3 : a+=((unsigned int)str[2]<<16);
		case 2 : a+=((unsigned int)str[1]<<8);
		case 1 : a+=str[0];
		/* case 0: nothing left to add */
	}
	mix(a,b,c);
	/*-------------------------------------------- report the result */
	return c;
}
unsigned int 
BOB4(const char * str, unsigned int len)
{
	//register ub4 a,b,c,len;
	unsigned int a,b,c;
	unsigned int initval = 127;
	/* Set up the internal state */
	//len = length;
	a = b = 0x9e3779b9;  /* the golden ratio; an arbitrary value */
	c = initval;         /* the previous hash value */

	/*---------------------------------------- handle most of the key */
	while (len >= 12)
	{
		a += (str[0] +((unsigned int)str[1]<<8) +((unsigned int)str[2]<<16) +((unsigned int)str[3]<<24));
		b += (str[4] +((unsigned int)str[5]<<8) +((unsigned int)str[6]<<16) +((unsigned int)str[7]<<24));
		c += (str[8] +((unsigned int)str[9]<<8) +((unsigned int)str[10]<<16)+((unsigned int)str[11]<<24));
		mix(a,b,c);
		str += 12; len -= 12;
	}

	/*------------------------------------- handle the last 11 bytes */
	c += len;
	switch(len)              /* all the case statements fall through */
	{
		case 11: c+=((unsigned int)str[10]<<24);
		case 10: c+=((unsigned int)str[9]<<16);
		case 9 : c+=((unsigned int)str[8]<<8);
		/* the first byte of c is reserved for the length */
		case 8 : b+=((unsigned int)str[7]<<24);
		case 7 : b+=((unsigned int)str[6]<<16);
		case 6 : b+=((unsigned int)str[5]<<8);
		case 5 : b+=str[4];
		case 4 : a+=((unsigned int)str[3]<<24);
		case 3 : a+=((unsigned int)str[2]<<16);
		case 2 : a+=((unsigned int)str[1]<<8);
		case 1 : a+=str[0];
		/* case 0: nothing left to add */
	}
	mix(a,b,c);
	/*-------------------------------------------- report the result */
	return c;
}
unsigned int 
BOB5(const char * str, unsigned int len)
{
	//register ub4 a,b,c,len;
	unsigned int a,b,c;
	unsigned int initval = 179;
	/* Set up the internal state */
	//len = length;
	a = b = 0x9e3779b9;  /* the golden ratio; an arbitrary value */
	c = initval;         /* the previous hash value */

	/*---------------------------------------- handle most of the key */
	while (len >= 12)
	{
		a += (str[0] +((unsigned int)str[1]<<8) +((unsigned int)str[2]<<16) +((unsigned int)str[3]<<24));
		b += (str[4] +((unsigned int)str[5]<<8) +((unsigned int)str[6]<<16) +((unsigned int)str[7]<<24));
		c += (str[8] +((unsigned int)str[9]<<8) +((unsigned int)str[10]<<16)+((unsigned int)str[11]<<24));
		mix(a,b,c);
		str += 12; len -= 12;
	}

	/*------------------------------------- handle the last 11 bytes */
	c += len;
	switch(len)              /* all the case statements fall through */
	{
		case 11: c+=((unsigned int)str[10]<<24);
		case 10: c+=((unsigned int)str[9]<<16);
		case 9 : c+=((unsigned int)str[8]<<8);
		/* the first byte of c is reserved for the length */
		case 8 : b+=((unsigned int)str[7]<<24);
		case 7 : b+=((unsigned int)str[6]<<16);
		case 6 : b+=((unsigned int)str[5]<<8);
		case 5 : b+=str[4];
		case 4 : a+=((unsigned int)str[3]<<24);
		case 3 : a+=((unsigned int)str[2]<<16);
		case 2 : a+=((unsigned int)str[1]<<8);
		case 1 : a+=str[0];
		/* case 0: nothing left to add */
	}
	mix(a,b,c);
	/*-------------------------------------------- report the result */
	return c;
}
unsigned int 
BOB6(const char * str, unsigned int len);
unsigned int 
BOB7(const char * str, unsigned int len);
unsigned int 
BOB8(const char * str, unsigned int len);
unsigned int 
BOB9(const char * str, unsigned int len);
unsigned int 
BOB10(const char * str, unsigned int len);
unsigned int 
BOB11(const char * str, unsigned int len);
unsigned int 
BOB12(const char * str, unsigned int len);
unsigned int 
BOB13(const char * str, unsigned int len);
unsigned int 
BOB14(const char * str, unsigned int len);
unsigned int 
BOB15(const char * str, unsigned int len);
unsigned int 
BOB16(const char * str, unsigned int len);
uint64 
BOB64(const char * str, unsigned int len);

#endif
