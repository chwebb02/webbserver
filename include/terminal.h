#ifndef _TERMINAL_H_
#define _TERMINAL_H_

#include "config.h"

int initTerminal(config_t *conf);

int terminalSendMessage(const char *msg);

void deleteTerminal();

#endif /* _TERMINAL_H_ */