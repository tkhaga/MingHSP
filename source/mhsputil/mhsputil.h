
int GetNum(const char *p);
void ChangeExt(unsigned char *fname, const char *ext);

void InitIO(unsigned char *p, unsigned long offset);
void IOseek(long dest);
void *IOgetPtr();
unsigned long getLongBE();
unsigned short getShortBE();
unsigned char getChar();

unsigned long getLongLE(unsigned char *p);
unsigned short getShortLE(unsigned char *p);
