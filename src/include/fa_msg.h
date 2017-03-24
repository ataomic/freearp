#ifndef FA_MSG_H
#define FA_MSG_H

typedef struct fa_msg {
	unsigned short type;
	unsigned short op;
	unsigned char data[4];
};

#endif

